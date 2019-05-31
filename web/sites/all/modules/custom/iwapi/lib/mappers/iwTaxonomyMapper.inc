<?php
/**
 * @file
 * Create a taxonomy mapper
 */

class iwTaxonomyMapper extends iwMapper {
  
  protected $vocab;
  protected $allowCreate;
  protected $storeFlat;

  protected $parentKey;
  protected $termKey;
  
  function __construct(array $def) {
    parent::__construct($def);
    
    if (!empty($def['vid'])) {
      $this->vocab = $def['vid'];
    }
    elseif ($def['vocabulary']) {
      $vocab = taxonomy_vocabulary_machine_name_load($def['vocabulary']);
      $this->vocab = $vocab->vid;
    }
    else {
      throw new InvalidArgumentException(t('Taxonomy mapper is missing vocabulary.'));
    }
    
    $this->storeFlat   = isset($def['storeFlat']) ? $def['storeFlat'] : FALSE;
    $this->allowCreate = $def['allowCreate'];

    $this->parentKey   = isset($def['parentKey']) ? $def['parentKey'] : 'term_parent';
    $this->termKey     = isset($def['termKey']) ? $def['termKey']     : 'term_name';
  }
  
  /**
   * Translate the individual fields into taxonomy terms.
   */
  function translate(array $raw) {
    $values = $this->extractValues($raw);
    
    if (count($values) > 1) {
      $value  = $values[$this->termKey];
      $parent = $values[$this->parentKey];
    }
    else {
      $value  = reset($values);
      $parent = 0;
    }
    
    if (empty($value)) {
      return NULL;
    }
    
    $results = array();
    $terms = explode(',', $value);
    foreach (array_map('trim', $terms) as $term) {
      $query = db_select('taxonomy_term_data', 'td')
        ->fields('td', array('tid'))
        ->condition('vid', $this->vocab)
        ->condition('name', $term, 'LIKE');
      
      if ($parent != 0) {
        $query->innerJoin('taxonomy_term_hierarchy', 'h', 'td.tid=h.tid');
        $query->condition('h.parent', $parent);
      }
      
      $tid = (int)$query->execute()->fetchField();
      if (empty($tid)) {
        if ($this->allowCreate && !empty($term)) {
          $data = (object)array(
            'vid'  => $this->vocab,
            'name' => $term,
            'parent' => $parent,
          );

          if (taxonomy_term_save($data)) {
            $tid = $data->tid;
          }
        }
        
        if (empty($tid)) {
          drupal_set_message(t('Term @name not found.', array('@name' => $term)));
          continue;
        }
      }
      
      // Store as a flat value or as an array with 'tid';
      $results[] = $this->storeFlat ? $tid : array('tid' => $tid);
    }

    return $this->storeFlat ? reset($results) : $results; // if only one, return just a top level item.
  }
}
