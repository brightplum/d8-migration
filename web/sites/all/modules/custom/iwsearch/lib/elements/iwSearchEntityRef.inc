<?php
/**
 * @file
 * Contains the functionality for the entity reference field element.
 */

class iwSearchEntityRef extends iwSearchAllowedList {

  public $multi = TRUE;


  /**
   * non-PHPDOC
   * @see iwSearchValue::getValueType().
   */
  public function getValueType() {
    return 'entityref';
  }

  /**
   * Retreive the entity type which is set
   * as the target for this reference field.
   * 
   * @return string
   *  The name of the entity type used as a
   *  target for this field value.
   */
  public function getTargetType() {
    $field = field_info_field($this->fieldName);
    return $field['settings']['target_type'];
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
        
      $entity_info = entity_get_info($field['settings']['target_type']);
      if (!empty($field) && !empty($entity_info)) {
        $src_type = $this->provider->getEntityType();
        
        // Get the list of allowed values.
        $cid = "iwsearch:field_allowed:{$this->fieldName}:{$src_type}";
        if ($cache = cache_get($cid)) {
          $allowed = $cache->data;
        }
        else {
          $idKey     = $entity_info['entity keys']['id'];
          $tablename = _field_sql_storage_tablename($field);
          $colname   = _field_sql_storage_columnname($this->fieldName, 'target_id');
          $labelCol  = $entity_info['entity keys']['label'];

          $query = db_select($tablename, 'fld');
          $query->innerJoin($entity_info['base table'], 'ent', "$colname=ent.{$idKey}");
          $query->fields('ent', array($idKey, $labelCol))
            ->condition('fld.entity_type', $src_type)
            ->condition('ent.status', 0, '<>') // Only active entities?
            ->orderBy($labelCol); 

          $allowed = $query->execute()->fetchAllKeyed();
          cache_set($cid, $allowed, 'cache');
        }
      }
    }
    
    return $allowed;
  }
}
