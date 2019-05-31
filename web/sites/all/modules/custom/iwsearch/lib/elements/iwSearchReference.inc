<?php
/**
 * @file
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


class iwSearchNodeRef extends iwSearchAllowedList {

  public $multi = TRUE;

  
  /**
   * non-PHPDOC
   * @see iwSearchValue::getValueType().
   */
  public function getValueType() {
    return 'node_reference';
  }
  
  /**
   * non-PHPDOC
   * @see iwSearchAllowedList::getAllowedValues().
   */
  public function getAllowedValues() {
    static $allowed;
    
    if (!isset($allowed)) {
      $allowed = array();
      $field   = field_info_field($this->fieldName);
      
      if (!empty($field)) {
        $src_type = $this->provider->getEntityType();
        
        // Get the list of allowed values.
        $cid = "iwsearch:field_allowed:{$this->fieldName}:{$src_type}";
        if ($cache = cache_get($cid)) {
          $allowed = $cache->data;
        }
        else {  
          $tablename = _field_sql_storage_tablename($field);
          $colname = _field_sql_storage_columnname($this->fieldName, 'nid');
          
          $query = db_select($tablename, 'fld');
          $query->innerJoin('node', 'n', "fld.{$colname}=n.nid");
          $query->fields('n', array('nid', 'title'))
            ->condition('fld.entity_type', $src_type)
            ->condition('n.status', 0, '<>')
            ->orderBy('n.title');

          $allowed = $query->execute()->fetchAllKeyed();
          cache_set($cid, $allowed);
        }
      }
      
    }
    
    return $allowed;
  }
}
