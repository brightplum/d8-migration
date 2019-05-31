<?php
/**
 * @file iw_content.inc
 *  Contains the class helper for creating taxonomy vocabulary and terms
 */

class iw_content extends iw_helper {

  /**
   * (non-PHPdoc)
   * @see iw_helper::get_info()
   */
  function get_info() {
    return array(
      'import' => array('types', 'fields', /*'taxonomy', 'content'*/),
      'export' => array('types', 'fields', /*'taxonomy', 'content'*/),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::get_export_options()
   */
  function get_export_options($name) {
    switch ($name) {
      case 'fields':
        return array_keys(field_info_fields());
      case 'types':
        $type_names = array();
        $types = node_type_get_types();
        foreach ($types as $type) {
          if (isset($type->type) && $type->module === 'node') {
            $type_names[] = $type->type;
          }
        }
        return $type_names;
    }
    return array();
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::import()
   */
  function import($name, array $package) {
    $import_method = 'import_'. $name;
    if ($this->check_required($package) && method_exists($this, $import_method)) {
      return $this->{$import_method}($package['data']);
    }
    return FALSE;
  }
  
  /**
   * Create fields defined in parameter array.
   * 
   * @param $fields
   *  Field defintions to attempt importing.
   */
  function import_fields($fields) {
    foreach ($fields as $field) {
      try {
        if (!empty($field['field_name'])) {
          $msg_params = array('%field_name' => $field['field_name']);
          if (!isset($field['entity_types'])) {
            $field['entity_types'] = array('node');
          }
          if ($field_info = field_info_field($field['field_name'])) {
            drupal_set_message(t('Updating field: %field_name', $msg_params));
            field_update_field($field);
          }
          else {
            drupal_set_message(t('Creating new field: %field_name', $msg_params));
            field_create_field($field);
          }
        }
      }
      catch (FieldException $e) {
        drupal_set_message($e->getMessage());
      }
    }
  }
  
  /**
   * Import a set of node types, along with their field instances
   *  found in the $type['fields'] index.
   * 
   * @param $types
   *  An array of array structures that represent node type definitions
   *  and their respective field instances. The field definitions are
   *  assumed to have already been imported.
   */
  function import_types($types) {
    foreach ($types as $type) {
      $fields = is_array($type['fields']) ? $type['fields'] : array();
      unset($type['fields']);
      
      // Restore / combine node options if they exist
      if (isset($type['addl_setup']) && is_array($type['addl_setup'])) {
        foreach ($type['addl_setup'] as $var_name => $var_values) {
          variable_set($var_name, $var_values);
        }
      }
      unset($type['addl_setup']);
      
      // Fetch and restore comment settings, so many separate fields.
      if (module_exists('comment') && isset($type['comment_options'])) {
        foreach ($type['comment_options'] as $name => $value) {
          variable_set("{$name}_{$type['type']}", $value);
        }
      }
      unset($type['node_options']);
      unset($type['comment_options']);
      
      $status = node_type_save((object)$type);
      if ($status === SAVED_NEW || $status=== SAVED_UPDATED) {
        foreach ($fields as $instance) {
          $instance['entity_type'] = 'node';
          $instance['bundle'] = $type['type'];
          if (field_info_instance('node', $instance['field_name'], $type['type'])) {
            field_update_instance($instance);
          }
          else {
            field_create_instance($instance);
          }
        }
        drupal_set_message(t('Install/updated node type "%name".', array('%name' => $type['name'])));
      }
      else {
        drupal_set_message(t('Failed to install node type "%name".', array('%name' => $type['name'])));
      }
    }
  }
  
  
  /**
   * Export a set of field definitions.
   * 
   * @param array $inc_fields
   *  If set and not empty, only include these fields.
   *  Specified by field_name.
   */
  function export_fields(array $inc_fields = array()) {
    $requires = array('field' => 'field');
    $data = array();
    
    $fields = field_info_fields();
    if (isset($inc_fields) && !empty($inc_fields)) {
      $inc_fields = array_combine($inc_fields, $inc_fields);
      $fields = array_intersect_key($fields, $inc_fields);  
    }
    
    // Keep only this set of fields from the info. Things like indexes, 
    //  and storage methodology we'll leave up to the receiving system.
    $allowed = array(
      'field_name' => TRUE,
      'type' => TRUE,
      'translatable' => TRUE,
      'settings' => TRUE,
      'cardinality' => TRUE,
      'locked' => TRUE,
    );
    
    // record each field
    foreach ($fields as $name => $field) {
      if (isset($field['module'])) {
        $requires[$field['module']] = $field['module'];
      }
      $data[$name] = array_intersect_key($field, $allowed);
    }
    
    // Allows us to export more than one set of exports.
    return empty($data) ? array() : array(
      array(
      	'helper' => 'iw_content',
      	'type' => 'fields',
      	'requires' => $requires,
      	'data' => $data,
      ),
    );
  }
  
  /**
   * Export node type definitions along with used fields
   *  and field instance information.
   * 
   * @param array $node_types
   *  An array of node types to filter by. If empty we'll
   *  export all node types recognized by the node module.
   *  
   * @return array
   *  Of iw_helper package formats.
   */
  function export_types(array $node_types = array()) {
    $requires = array('node' => 'node');
    $fields = array();
    $data = array();
    
    $types = node_type_get_types();
    if (isset($node_types) && !empty($node_types)) {
      $node_types = array_combine($node_types, $node_types);
      $types = array_intersect_key($types, $node_types);
    }
    
    // Cycle through node definitions, and check for requirements
    //  and field instances that need to get imported as well.
    foreach ($types as $type => $def) {
      if ($def->module === 'node') {
        $data[$type] = (array)$def;
        $data[$type]['addl_setup'] = $this->get_node_type_options($type);
        
        // Get node instance fields
        $data[$type]['fields'] = array();
        $instances = field_info_instances('node', $type);
        foreach ($instances as $instance) {
          unset($instance['id']);
          unset($instance['deleted']);
          unset($instance['field_id']);
          unset($instance['entity_type']);
          unset($instance['bundle']);
          
          // Check if the field instance has any module requirements
          // add them to the list of modules required if there are any.
          if (isset($instance['widget']['module'])) {
            $mod = $instance['widget']['module'];
            $requires[$mod] = $mod;
          }
          if (is_array($instance['display'])) {
            foreach ($instance['display'] as $disp) {
              if (isset($disp['module'])) {
                $requires[$disp['module']] = $disp['module'];
              }
            }
          }

          // Record the field used to list of fields to export as well.
          //  Also record the instance to the node type's 'fields' index.
          $fields[] = $instance['field_name'];
          $data[$type]['fields'][$instance['field_name']] = $instance;
        }
      }
    }
    
    // Export fields used with the nodes
    $packages = empty($fields) ? array() : $this->export_fields($fields);
    if (!empty($data)) {
      $packages[] = array(
        'helper' => 'iw_content',
        'type' => 'types',
        'requires' => array_keys($requires),
        'data' => $data,
      );
    }
    return $packages;
  }
  
  /**
   * 
   * @param $type
   *  The node type
   *  
   * @return array
   *  
   */
  function get_node_type_options($type) {
    $options = array();
    
    // Get general node type settings.
    $params = array(':pattern' => 'node_%_'. $type);
    $rs = db_query('SELECT name, value FROM {variable} WHERE name LIKE :pattern', $params);
    while ($var = $rs->fetchObject()) {
      if (preg_match("#^node_(?:options|preview|submitted)_{$type}$#", $var->name) > 0) {
        $options[$var->name] = unserialize($var->value);
      }
    }
    
    // Get comment settings
    if (module_exists('comment')) {
      $param = array(':pattern' => 'comment_%_'. $type);
      $rs = db_query('SELECT name, value FROM {variable} WHERE name LIKE :pattern', $params);
      
      while ($var = $rs->fetchObject()) {
        $options[$var->name] = unserialize($var->value);
      }
    }
    return $options;
  }
}

