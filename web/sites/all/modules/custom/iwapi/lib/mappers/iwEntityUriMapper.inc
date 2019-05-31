<?php
/**
 * @file
 * Mapping an entity ID and type into the url for the entity.
 */


class iwEntityUriMapper extends iwMapper {
  
  
  public $entityType = 'node';
  
  
  function __construct(array $def) {
    parent::__construct($def);
    
    if (isset($def['entityType'])) {
      $this->entityType = $def['entityType'];
    }
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
    if (($id = reset($values))) {
      $entities = entity_load($this->entityType, array($id));
    
      if (!empty($entities)) {
        $uri = entity_uri($this->entityType, reset($entities));
        return $uri['path'];
      }
    }
    return NULL;
  }
}
