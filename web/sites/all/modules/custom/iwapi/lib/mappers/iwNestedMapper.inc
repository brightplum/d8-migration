<?php
/**
 * @file
 * Contains a mapper which nests a value into a field.
 */

class iwNestedMapper extends iwMapper {
  
  /**
   * Generate a new nested mapper item.
   */
  function __construct(array $def) {    
    $input = reset($def['inputs']);
    $name  = is_array($input) ? $input['handler'] : $input;
    
    // Drill down into the source
    if (preg_match('/(?:\[\w*\])+$/', $name, $matches)) {
      $name = substr($name, 0, -strlen($matches[0]));
      $def['inputs'] = array(
        is_array($input) ? (array('handler' => $name) + $input) : $name
      );
      
      $this->srcIndexes = preg_split(self::IndexRegex, trim($matches[0], '[] \'"'));
    }
    
    parent::__construct($def);
  }
      
  /**
   * Take the known input values and translate them
   * into a single result value. If $this->multi is
   * TRUE, the value is an array of data entries.
   *
   * @param array $raw
   *  Values used as raw input to get translated.
   */
  function translate(array $raw) {
    if (!$this->isValid()) {
      throw new iwMapperException(iwMapperException::UNBUILT_MAPPER);
    }

    $values = $this->extractValues($raw);
    $value = reset($values);
    return (empty($this->srcIndexes) || !is_array($value))
      ? $value : drupal_array_get_nested_value($value, $this->srcIndexes);
  }
}
