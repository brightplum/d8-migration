<?php


/**
 * 
 * 
 */
class iwDbSearchProvider extends iwSearchProvider implements iwIIndexedSearchProvider {
  
  // The generated hash code to mark changes that require re-indexing.
  protected $_hash;
  
  // Database table used to use as an index table.
  protected $table;
  
  
  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::resetBuild()
   */
  protected function resetBuild() {
    $this->_hash = NULL;
    $this->compiled = NULL;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIIndexedSearchProvider::computeHash()
   */
  function computeHash() {
    if (!isset($this->_hash)) {
      $copy = $this->elements;
  
      // Order of the elements don't matter to an index table, but we
      // don't want to hurt the original array, since order does matter
      ksort($copy);
      $this->_hash = md5(serialize($copy));
    }
    return $this->_hash;
  }

  /**
   * (non-PHPdoc)
   * @see iwIIndexedSearchProvider::getSchema()
   */
  function getSchema() {
    $this->compile();
  
    $t = get_t();
    $schema = array(
      'description' => $t('Search Index table for @type:@bundle', array(
        '@type' => $this->entityType,
        '@bundle' => $this->bundle,
      )),
      'fields' => array(
        'entity_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => $t('Unique identifier for entity being referenced.'),
        ),
      ),
    );
  
    // Iterate through the list of elements and build them.
    foreach ($this->compiled['element'] as $name => $element) {
      if ($name != 'keywords') {
        $element->setElementSchema($schema);
      }
    }
  }
  
  /**
   * Callback to index an entity into our index
   * tables if needed.
   *
   * @param stdClass|iwEntity $entity
   *  The entity instance to push into the indexing table.
   */
  function indexEntity(array $entities, $limit = 50) {
    $this->compile();
  
    $data = array();
    foreach ($this->compiled['elements'] as $name => $element) {
      if ($name !== 'keywords') {
        $element->indexElement($entity, $data);
      }
    }
    return $data;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::execute()
   */
  function execute(array $criteria) {
    $criteria = array_filter($criteria, 'drupal_strlen');
    
    if (empty($criteria)) {
      $query = $this->defaultSearchQuery();
    }
    else {
      if (!empty($criteria['keywords'])) {
        $info = entity_get_info($this->entityType);
        $keys = $info['entity keys'];
        
        $query = db_select('search_index', 'i', array('target' => 'slave'))
          ->extend('PagerDefault')->limit($this->resultsPerPage)
          ->extend('SearchQuery');
        
        // Add the search query elements 
        $query->join($this->table, 'ent', 'ent.entity_id = i.sid');
        $query->searchExpression($criteria['keywords'], $this->entityType);
        if (!$query->executeFirstPass()) {
          return array();
        }
      }
      else {
        $query= db_select($this->table, 'ent')
          ->fields('ent', array('entity_id'))
          ->extend('PagerDefault')->limit($this->resultsPerPage);
      }
      
      // TODO: Handle all the additional criteria.
    }
    
    // Get all the entity ids
    $ids = $query->execute()->fetchCol();
    return entity_load($this->entityType, $ids);
  }
}
