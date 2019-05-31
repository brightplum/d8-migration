<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

class iwD6TermReader extends iwDbReader {
    
  // Taxonomy terms fields that should transfer directly into the new content.
  static protected $TermKeys = array('tid', 'vid', 'name', 'description', 'weight');
  
  /**
   * (non-PHPDOC)
   * @see iwIPackage::getPackageProps().
   */
  public static function getPackageProps() {
    return array(
      'vocab' => array('label' => t('Source Node type')),
    ) + parent::getPackageProps();
  }
  
  
  // --------------------------
  // Instance fields
  // --------------------------
  
  public $name = 'default';
  
  protected $id;
  protected $type;
  
  
  // --------------------------
  // Instance methods
  // --------------------------
  
  public function __construct(array $data = array()) {
    parent::__construct($data);
    
    // Setup the common data mapping.
    $this->vocab = $data['vocab'];
    $this->idKey = 'orig_tid'; 
  }
  
  
  /**
   * (non-PHPDOC)
   * @see iwIReader::setSource()
   */
  public function setSource($src) {
    $this->id     = 0;
    $this->count  = 0;
    $this->buffer = array();
    
    $src += array('host' => 'localhost');
    Database::addConnectionInfo('iw_import', $this->name, $src);
    
    // Prepare the old and new vocabulary settings
    $vocab = $this->getConnection()->select('vocabulary', 'v')
      ->fields('v')
      ->condition('name', $this->vocab, 'LIKE')
      ->execute()->fetchObject();
    
    $this->vid = $vocab->vid;

    $machine_name = preg_replace('#[\s\-]+#', '_', strtolower($vocab->name));
    $this->dstVocab = taxonomy_vocabulary_machine_name_load($machine_name);
    if (empty($this->dstVocab)) {
      unset($vocab->vid, $vocab->module);

      $vocab->machine_name = $machine_name;
      taxonomy_vocabulary_save($vocab);
      $this->dstVocab = $vocab;
    }
    
    $this->total = $this->getTotal();
    
    db_merge('d6_importer_lookup')
      ->key(array('data_type' => 'vocabulary', 'old' => $this->vid))
      ->fields(array(
        'data_type' => 'vocabulary',
        'old' => $this->vid,
        'entity_type' => 'taxonomy_vocabulary',
        'id' => $this->dstVocab->vid,
      ))->execute();
  }
  
  protected function getTotal() {   
    // Get the count information for the data about to get loaded.
    $count_query = $this->getConnection()->select('term_data', 't')
      ->condition('vid', $this->vid);
    
    $count_query->addExpression('COUNT(*)', 'count');
    return $count_query->execute()->fetchField();
  }
  
 
  /**
   * (non-PHPdoc)
   * @see iwIReader::getKeys()
   */
  function getKeys() {    
    $keys = static::$TermKeys;
    array_unshift($keys, 'orig_tid');
    
    return $keys;
  }
  
    
  // ========================
  // Special query handling
  // ========================
  
  /**
   * Make a query and buffer the data.
   * 
   * @param type $bufferSize
   *  Size of the data to buffer from the
   *  source database.
   */
  function bufferData($bufferSize = 50) {
    $this->buffer = array();
    
    if (!isset($this->query)) {
      $this->query = $this->getConnection()->select('term_data', 't')
        ->fields('t', static::$TermKeys)
        ->condition('vid', $this->vid)
        ->range(0, $bufferSize)
        ->orderby('t.tid');
      
      $this->query->leftJoin('term_hierarchy', 'h', 't.tid=h.tid');
      $this->query->fields('h', array('parent'));
    }

    // Pull items from where we left off.
    $query = clone $this->query;
    $rs = $query->condition('t.tid', $this->id, '>')->execute();
    
    while ($obj = $rs->fetchAssoc()) {
      $obj['orig_tid'] = $obj['tid'];
      $obj['vid']      = $this->dstVocab->vid;
      
      unset($obj['tid']);
      $this->buffer[$obj['orig_tid']] = $obj;
    }
  }
}