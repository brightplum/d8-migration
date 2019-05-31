<?php
/**
 * @file
 * Contains the base related content handler for the
 */


class iwRelatedContent {
    
  const DEFAULT_SIZE = 4;


  protected $type;
  protected $entity;


  /**
   * Create a new related content finder.
   */
  function __construct($entity, $entity_type) {
    $this->type   = $entity_type;
    $this->entity = $entity;
  }
  
  /**
   * Find related content based on the entity type
   * associated with this object / instance.
   * 
   * @return array
   *  Returns an array where the first index is the
   *  entity type machine name and the second index
   *  is an array of {node}.nid for the nodes that
   *  match the criteria. 
   */
  function findRelated() {
    $size    = variable_get(IWSEARCH_RELATED_VAR_SIZE, static::DEFAULT_SIZE);
    $bundles = array_filter(variable_get(IWSEARCH_RELATED_NODE_RESULT_TYPES, array()));
      
    $query = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('status', NODE_PUBLISHED)
      ->orderBy('n.created', 'DESC')
      ->range(0, $size);
    
    if (!empty($bundles)) {    
      $query->condition('type', $bundles);
    }
    
    if ($this->type == 'node') {
      list($id, $rev, $bundle) = entity_extract_ids($this->type, $this->entity);
      
      $query->condition('nid', $id, '<>');
    }
  
    return array('node', $query->execute()->fetchCol());
  }
}



class iwSolrRelatedContent extends iwRelatedContent {

  /**
   * (non-PHPDOC)
   * @see iwRelatedContent::findRelated()
   */
  function findRelated() {
    $size = variable_get(IWSEARCH_RELATED_VAR_SIZE, static::DEFAULT_SIZE);
    $sort = variable_get(IWSEARCH_RELATED_VAR_SORT, array()) + array(
      'field' => 'ds_created',
      'dir'   => 'desc',
    );

    $ids         = array();
    $search      = iwsearch_instance_load('related_content');
    $criteria    = $this->getCriteria($search);
    $resultType  = $search->getEntityType();
    $entity_info = entity_get_info($resultType);
    
    if ($sort['field'] === 'random') {
      $sort['field'] = 'random_' . rand(1, 2000);
    }
    
    if (isset($criteria) && $entity_info) {
      $settings = array(
        'pageSize' => $size,
        'page' => 0,
        'sort' => array("$sort[field] $sort[dir]"),
      );

      $idKey = $entity_info['entity keys']['id'];
      $id = $this->entity->{$idKey};

      // Make sure to exclude itself.
      if ($resultType == $this->type) {
        $settings['filters']['-entity_id'] = $id;
      }

      try {
        $resp = $search->execute($criteria, $settings);
        foreach ($resp['entities'] as $result) {
          $ids[$result->entity_id] = $result->entity_id;
        }
      }
      catch (Exception $e) {
        watchdog_exception('iwsearch_related', $e);
      }

      // Make sure this item is completely removed.
      if ($resultType == $this->type && isset($ids[$id])) {
        unset($ids[$id]);
      }
    }

    return array($resultType, $ids);
  }
  
  /**
   * Create an array of search criteria based on the
   * entity and the search provider.
   * 
   * @param iwSearchProvider $search
   *  Loaded iwSearchProvider which will execute the
   *  search for the related content info.
   * @return array
   *  Returns an array of key / value pairs which
   *  are filters to apply when doing the search.
   */
  function getCriteria(iwSearchProvider $search) {
    $entity     = $this->entity;
    $properties = iwapi_get_entity_properties($this->type);
  
    $criteria = array();
    foreach ($search->elements as $name => $element) {
      $values = NULL;

      if (key_exists($name, $properties)) {
        $criteria[$name] = $properties[$name]->getValue($entity);
      }
      elseif (!empty($entity->{$name}) && $field = field_info_field($name)) {
        $items = field_get_items($this->type, $entity, $name);
  
        switch ($field['type']) {
          case 'taxonomy_term_reference':
            $key = 'tid';
            break;
          case 'user_reference':
            $key = 'uid';
            break;
          case 'node_reference':
            $key = 'nid';
            break;
          default:
            $key = 'value';
        }
 
        array_walk($items, '_iwapi_extract_array_field', $key);
        $criteria[$name] = $element->multi ? $items : reset($items);
      }
      else {
        return NULL; // Missing search criteria / can't : treat like no matches.
      }
    }

    return $criteria;
  }
}
