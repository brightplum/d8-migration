<?php
/**
 * @file
 * Filter through text markup and look for various links that need to be updated.
 */


class iwD6MarkupMapper extends iwMapper {

  public $imgPath = '';
  public $imgRepl = '';
  
  public $linkPath = '';
  public $linkRepl = '';
 
  
  function __construct(array $def) {
    parent::__construct($def);
    
    if (!empty($def['imgPath'])) {
      $this->imgPath = $def['imgPath'];
      $this->imgRepl = !empty($def['imgRepl']) ? $def['imgRepl'] : '/';
      
      $this->regex[] = '#(<img.*? src=["\'])'. $this->imgPath .'([^"\']*["\'][^>]*)>#im';
      $this->repl[]  = '$1'. $this->imgRepl .'$2>';
    }
    
    if (!empty($def['linkPath'])) {
      $this->linkPath = $def['linkPath'];
      $this->linkRepl = !empty($def['linkRepl']) ? $def['linkRepl'] : '/';
      
      $this->regex[] = '#(<a.*? href=["\'])/?'. $this->linkPath .'([^"\']*["\'][^>]*)>#im';
      $this->repl[]  = '$1'. $this->linkRepl .'$2>';
    }
  }
  
  /**
   * (non-PHPDOC)
   * @see iwMapper::translate().
   */
  function translate(array $raw) {      
    $values = $this->extractValues($raw);
    $value = reset($values);
    
    if (!empty($this->regex)) {
      if (is_string($value)) {
        $value = preg_replace($this->regex, $this->repl, $value);
      }
      else {
        foreach (array('summary', 'value') as $name) {
          if (!empty($value[$name])) {
            $value[$name] = preg_replace($this->regex, $this->repl, $value[$name]);
          }
        }
      }
    }
    
    return $value;
  }
  

  /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    $data = parent::packData();
    
    $data['imgPath'] = $this->imgPath;
    $data['imgRepl'] = $this->imgRepl;
    
    $data['linkPath'] = $this->linkPath;
    $data['linkRepl'] = $this->linkRepl;
 
    return $data;
  }
}