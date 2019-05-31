<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


class DisqusWriter extends iwWriter {
  
  function __construct(array $values = array()) {
    parent::__construct($values);
    
    $this->allowed['nid']      = 'nid';
    $this->allowed['orig_nid'] = 'orig_nid';
    
    $this->required = $this->allowed;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    if (!empty($data['nid'])) {
      $query = db_select('disqus', 'd')
        ->condition('nid', $data['nid']);
      
      $query->addExpression('COUNT(*)');
      return (bool)$query->execute()->fetchField();
    }
    
    return FALSE;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    if (empty($data['orig_nid']) || empty($data['nid'])) {
      return FALSE; // skip items missing needed informationation.
    }

    $exists = $this->exists($data);
    if ($exists) {
      if (($this->mode & iwWriter::ALLOW_UPDATE) === 0) {
        drupal_set_message(t("Entry for Disqus node $data[nid] already exists."), 'error');
        return FALSE;
      }
    }
    elseif (($this->mode & iwWriter::ALLOW_INSERT) === 0) {
      drupal_set_message(t("Entry for Disqus node $data[nid] already exists."), 'error');
      return FALSE;
    }

    db_merge('disqus')
      ->key(array('nid' => $data['nid']))
      ->insertFields(array(
        'nid' => $data['nid'],
        'status' => 1,
        'identifier' => "node/$data[orig_nid]",
      ))->updateFields(array(
        'nid' => $data['nid'],
        'identifier' => "node/$data[orig_nid]",
      ))->execute();
  }
}
