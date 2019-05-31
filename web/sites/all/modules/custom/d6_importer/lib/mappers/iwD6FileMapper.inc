<?php
/**
 * @file
 * Actually copy the files to new mapped locations and create managed_file records.
 */


class iwD6FileMapper extends iwMapper {
  
  public $modifyPath = NULL;
  public $destPath   = NULL;
  
  /**
   * 
   * @param array $def
   *  Array of settings to use with
   */
  function __construct(array $def = array()) {
    parent::__construct($def);
    
    foreach (array('modifyPath', 'destPath') as $key) {
      if (!empty($def[$key])) {
        $this->{$key} = $def[$key];
      }
    }
  }
  
  /**
   * (non-PHPDOC)
   * @see iwMapper::translate().
   */
  function translate(array $raw) {
    $values = $this->extractValues($raw);
    $value = reset($values);

    if (!empty($this->modifyPath)) {
      $modify = $this->modifyPath + array(
        'regex' => '//',
        'replace' => '',
        'prefix' => '',
      );
      $value = $modify['prefix'] . preg_replace($modify['regex'], $modify['replace'], $value);
    }
    
    return $value;
  }
  

  /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    $data = parent::packData();
    
    $data['modifyPath'] = $this->modifyPath;
    return $data;
  }
}
