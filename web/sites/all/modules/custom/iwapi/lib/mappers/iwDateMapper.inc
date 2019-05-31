<?php
/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


class iwDateMapper extends iwMapper {


  // -------------------------------
  // Instance methods
  // -------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwMapper::translate()
   */
  function translate(array $raw) {
    if (!$this->isValid()) {
      throw new iwMapperException(iwMapperException::UNBUILT_MAPPER);
    }
    
    $values = $this->extractValues($raw);
    if ($date = reset($values)) {
      return strtotime($date);
    }

    return 0;
  }
}