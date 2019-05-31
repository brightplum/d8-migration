<?php
/**
 * @file
 * Create a wrapper that 
 */


class iwTextReplaceMapper extends iwMapper {

  protected $patterns = array();
  protected $replace  = array();


  /**
   * Generate a new nested mapper item.
   */
  function __construct(array $def) {
    parent::__construct($def);
    
    if (!empty($def['patterns']) && !empty($def['replace'])) {
      $this->patterns = $def['patterns'];
      $this->replace  = $def['replace'];
    }    
  }
      
  /**
   * Find and possibly replace text from values
   * before returning them.
   *
   * @param array $raw
   *  Values used as raw input to get translated.
   */
  function translate(array $raw) {
    $value = parent::translate($raw);
    
    if (is_array($value)) {
      foreach ($value as &$item) {
        $item = preg_replace($this->patterns, $this->replace, $item);
      }
      unset($item);
    }
    else {
      $value = preg_replace($this->patterns, $this->replace, $value);
    }
    
    return $value;
  }
  
   /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    $data = parent::packData();
    
    // Add new elements to the file.
    $data['patterns'] = $this->patterns;
    $data['replace']  = $this->replace;
    
    return $data;
  }
}