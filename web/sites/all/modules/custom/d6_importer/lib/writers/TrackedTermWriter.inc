<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


class TrackedTermWriter extends iwWriter {
    
  function __construct($mode = self::ALLOW_ANY) {
    $this->mode = $mode;
    
    $this->ids      = array('tid' => 'tid');
    $this->required = array('name' => 'name', 'vid' => 'vid');
    $this->allowed  = array(
      'orig_tid' => 'orig_tid',
      'vid'  => 'vid',
      'name' => 'name',
      'description' => 'description',
      'format' => 'format',
      'weight' => 'weight',
      'parent' => 'parent',
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    $query = db_select('taxonomy_term_data', 't')
      ->condition('name', $data['name'], 'LIKE')
      ->condition('vid', $data['vid']);
    
    if (!empty($data['parent'])) {
      $query->leftJoin('taxonomy_term_hierarchy', 'h', 'h.tid=t.tid');
      $query->condition('h.parent', $data['parent']);
    }

    $query->addExpression('COUNT(1)');
    return ((int)$query->execute()->fetchField()) > 0;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) { 
    $errors = $this->validate($data);
    if ($errors) {
      return FALSE;
    }
    $data = array_intersect_key($data, $this->allowed);
    $exists = $this->exists($data);
    
    if (($exists && ($this->mode & self::ALLOW_UPDATE) !== 0) || 
      (!$exists && ($this->mode & self::ALLOW_INSERT) !== 0)) {

      $term = (object)$data;
      taxonomy_term_save($term);
      
      if (!empty($data['orig_tid'])) {
        // Save a lookup between the old and new file ID.
        db_merge('d6_importer_lookup')
          ->key(array('old' => $data['orig_tid'], 'data_type' => 'term'))
          ->fields(array(
            'data_type' => 'term',
            'old'  => $data['orig_tid'],
            'entity_type' => 'taxonomy_term',
            'id'   => $term->tid,
          ))->execute();
      }
      
      return $term;
    }
    return FALSE;
  }
}