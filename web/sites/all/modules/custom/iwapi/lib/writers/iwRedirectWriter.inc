<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

class iwRedirectWriter extends iwWriter {
   
  /**
   * Report available package properties
   */
  static function getPackageProps() {
    return array(
      'mode' => array('label' => t('Write mode')),
      'skipMissing'  => array('label' => t('Skip any missing')),
      'skipExisting' => array('label' => t('Skip existing sources')),
    );
  }


  // ===============================
  // Instance properties
  // ===============================
  
  public $skipMissing  = FALSE;
  public $skipExisting = FALSE;
  

  /**
   * Create an instance of writer that writes data
   * to a database table. Writer will try to automatically
   * determine some of its properties based on schema info.
   */
  function __construct(array $values = array()) {
    parent::__construct($values);

    $this->required = array('source' => 'source', 'redirect' => 'redirect');
    $this->allowed  = $this->required + array(
      'rid'      => 'rid',
      'type'     => 'type',
      'uid'      => 'uid',
      'language' => 'language',
    );
    
    $this->ids = array('rid' => 'rid');
  }

  /**
   * (non-PHPdoc)
   * @see iw_writer::exists()
   */
  function exists(array $data) {
    if (!isset($data['rid'])) {
      return FALSE;
    }
    
    $query = db_select('redirect', 'dir')
      ->condition('rid', $data['rid']);
    
    $query->addExpression('COUNT(*)');
    return $query->execute()->fetchField() > 0;
  }

  /**
   * (non-PHPdoc)
   * @see iw_writer::save()
   */
  function save(array &$data) {
    global $user;
    
    $data = array_filter($data);
    $error = $this->validate($data);    
    if (!empty($error)) {
      if (!$this->skipMissing) {
        drupal_set_message($error);
      }
      return FALSE;
    }
    
    if (array_diff_key($this->required, $data)) {
      return FALSE;
    }
    
    if ($this->skipExisting) {
      $exists = db_select('redirect', 'r')
        ->condition('source', db_like($data['source']), 'LIKE');
      
      $exists->addExpression('COUNT(1)');
      if ((bool)$exists->execute()->fetchField()) {
        return TRUE;
      }
    }
    
    $redirect = (object) array(
      'type' => 'redirect',
      'uid' => $user->uid,
      'status_code' => 0,
      'source_options' => array(),
      'redirect_options' => array(),
    );
    
    foreach ($this->allowed as $field) {
      if (isset($data[$field])) {
        $redirect->{$field} = $data[$field];
      }
    }
    redirect_save($redirect);
    
    return FALSE;
  }
}