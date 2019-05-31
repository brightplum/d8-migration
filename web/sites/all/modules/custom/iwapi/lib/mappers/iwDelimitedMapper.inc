<?php
/**
 * @file
 * Contains a mapper that handles values which should be split on a delimiter. 
 */

class iwDelimitedMapper extends iwMapper {

  // List the available decoding formats that can be used.
  static function getDecoderInfo() {
    return array(
      'json' => 'drupal_json_decode',
      'php'  => 'unserialize', 
    );
  }
  
  
  // -------------------------------
  // Instance variables
  // -------------------------------
  
  protected $delimiter = '|';
  protected $decoder   = FALSE;


  // -------------------------------
  // Instance methods
  // -------------------------------
  
  /**
   * Load the settings for a limited mapper type.
   */
  function __construct(array $def) {
    parent::__construct($def);

    // We only want this item to be 1 character.
    $delimiter = trim($def['delimiter']);
    $this->delimiter = $delimiter[0];

    // Check for allowed set of decoders.
    $decoders = self::getDecoderInfo();
    $this->decoder = isset($decoders[$decoder]) ? $decoder : FALSE;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwMapper::translate()
   */
  function translate(array $raw) {
    $value  = parent::translate($raw);
    $values = explode($this->delimiter, $value);
    
    if ($this->decoder && function_exists($this->decoder)) {
      foreach ($values as &$value) {
        if ($tmp = $func($value)) {
          $value = $tmp;
        }
      }
      unset($value);
    }
    else {
      $values = array_map('trim', $values);
    }
 
    return $values;
  }

  /**
   * (non-PHPdoc)
   * @see iwMapper::packData()
   */
  function packData() {
    $data = parent::packData();
    $data['decoder']   = $this->decoder;
    $data['delimiter'] = $this->delimiter;
    
    return $data;
  }
}
