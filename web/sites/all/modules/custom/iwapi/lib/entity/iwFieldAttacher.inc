<?php

/**
 * Attaches/exports field instances to entity types
 * it will also package and create actual fields as
 * well if possible and needed by the instance.
 */
class iwFieldAttacher extends iwEntityAttacher {
	
  /**
   * (non-PHPdoc)
   * @see IEntityAttacher::attach()
   */
  function attach(array $src, $force = FALSE) {
    // First determine which fields are still missing from our bundle.
    $current = field_info_instances($this->type, $this->bundle);
    $fields  = $force ? $src : array_diff_key($src, $current);
    
    // Add the fields to the entity
    foreach ($fields as $field_name => $data) {
      try {
        // Check for missing field definitions and add them if available.
        $field_info = field_info_field($field_name);
        if (empty($field_info)) {
          $data['field']['field_name'] = $field_name;
          field_create_field($data['field']);
        }
        
        // install the field instance to this bundle.
        if (!empty($data['instance']) && is_array($data['instance'])) {
          $data['instance']['entity_type'] = $this->type;
          $data['instance']['bundle']      = $this->bundle;
     
          // Install if the instance is new, otherwise only update if we want to force settings.
          empty($current[$field_name])
            ? field_create_instance($data['instance'])
            : field_update_instance($data['instance']);
        }
      }
      catch (Exception $e) {
        $t_params = array('%msg' => $e->getMessage(), '%field' => $field_name);
        drupal_set_message(t('Unable to install %field : %msg', $t_params));
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::options()
   */
  function allowedValues() {
    $instances = field_info_instances($this->type, $this->bundle);
    return empty($instances) ? array() : _iwapi_extract_field($instances);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::export()
   */
  function export() {
    $fields = array();

    // These are the only fields we really want from the field_info array.
    $allowed = array('field_name', 'type', 'translatable', 'cardinality', 'locked', 'settings');
    $allowed = array_fill_keys($allowed, TRUE);
    
    // Iterate through the field instances and record the selected ones.
    $instances = field_info_instances($this->type, $this->bundle);
    foreach (array_intersect_key($instances, $this->values) as $name => $inst) {
      if ($field_info = field_info_field_by_id($inst['field_id'])) {
        unset($inst['id'], $inst['field_id']);
        unset($inst['entity_type'], $inst['bundle']);

        $fields[$name] = array(
          'instance' => $inst,
          'field' => array_intersect_key($field_info, $allowed),
        );
      }
    }
    return $fields;
  }
}
