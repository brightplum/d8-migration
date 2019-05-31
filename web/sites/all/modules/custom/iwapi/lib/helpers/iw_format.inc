<?php
/**
 * @file iw_filter.inc
 *  Helper functions for managing, and importing filters.
 */

class iw_format extends iw_helper {

  /**
   * (non-PHPdoc)
   * @see iw_helper::get_info()
   */
  function get_info() {
    return array(
      'maps' => array('formats', 'wysiwyg'),
      'import' => array('formats', 'wysiwyg'),
      'export' => array('formats', 'wysiwyg'),
    );
  }
  
  /**
   * Translate filter_format information into the
   *  role permission name. This is a form that can
   *  be used by iw_mapper to get the permission name.
   * 
   * @param $format
   *  Either the format_id or the format object.
   * @return
   *  The permission name for this filter if it exists.
   */
  function format_permission_name($format) {
    if (!is_object($format)) {
      $format = filter_format_load($format);
    }
    return filter_permission_name($format);
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::export_options()
   */
  function get_export_options($name) {
    $formats = filter_formats();
    switch ($name) {
      case 'formats':
      case 'wysiwyg':
        return array_keys($formats);
        
    }
    return array();
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::import()
   */
  function import($name, array $package) {
    static $datamaps;

    // Check to see that this package requirements have been met
    if (!$this->check_required($package)) {
      return FALSE;
    }

    // Build this here, because get_maps could be returning different
    // parser type versions, but we only allow the array type here. So
    // get_map() returns different versions, but this is always the same.
    if (!isset($datamaps[$name])) {
      $datamaps[$name] = $this->get_map('iw_array_parser', $name);
    }
    $datamap = $datamaps[$name];
    
    // Ensure that datamap we attempt to use is the right type
    if ($datamap instanceof iw_datamap) {
      $datamap->clear_sources();
      $datamap->add_source($package['data']);
      $datamap->execute();
      return TRUE;
    }
    return FALSE;
  }
  
  /**
   * Generate an array representing the desired filter information
   *  including enabled formats, user roles and general settings.
   * 
   * @param array $conditions
   *  A list of formats to export, if empty or null, will return
   *  all available formats.
   */
  protected function export_formats(array $conditions = array()) {
    $role_mapper = new iw_db_mapper('role', 'name', array('rid'), TRUE);
    
    $requires = array();
    $formats = filter_formats();
    if (isset($conditions) && !empty($conditions)) {
      $conditions = array_combine($conditions, $conditions);
      $formats = array_intersect_key($formats, $conditions);
    }
    
    // Cycle through each filter format and build a exportable set of formats
    foreach ($formats as $fid => $format) {
      $values = (array)$format;
      $values['module'] = 'filter';
      
      // Add only enabled filters to the formats
      $filters = filter_list_format($fid);
      foreach ($filters as $filter) {
        if ($filter->status) {
          $requires[$filter->module] = $filter->module;
          $values['filters'][$filter->name] = array(
            'status' => $filter->status,
            'weight' => $filter->weight,
            'settings' => $filter->settings,
          );
        }
      }
      
      // Check if permissions are available, and who currently has them
      if (($perm_name = filter_permission_name($format)) !== FALSE) {
        $rid = array('rid' => array());
        $rs = db_query('SELECT rid FROM {role_permission} p WHERE permission=:perm', array(':perm' => $perm_name));
        while ($value = $rs->fetchAssoc()) {
          $rid['rid'][] = $value['rid'];
        }
        $role_mapper->bind($rid, $rid);
        $values['roles'] = $rid['name'];
      }
      else {
        $values['roles'] = array();
      }
      $formats[$fid] = $values;
    }
    
    // Export the package info and data.
    return empty($values) ? array() : array(
      array(
        'helper' => 'iw_format',
      	'type' => 'formats',
        'requires' => array_values($requires),
      	'data' => $formats,
      ),
    );
  }
  
  /**
   * Export the current WYSIWYG settings
   * 
   * @param array $conditions
   *  Names of items to include, if this array is empty
   *  all WYSIWYG profiles will be exported.
   */
  protected function export_wysiwyg(array $conditions) {
    $formats = filter_formats();
    if (isset($conditions) && !empty($conditions)) {
      $conditions = array_combine($conditions, $conditions);
      $formats = array_intersect_key($formats, $conditions);
    }
    
    $profiles = array();
    foreach ($formats as $format) {
      $profiles[$format->format] = (array)wysiwyg_get_profile($format->format);
    }
    
    // Return the packaged set of data
    return empty($profiles) ? array() : array(
      array(
      	'helper' => 'iw_format',
        'type' => 'wysiwyg',
      	'requires' => array('wysiwyg'),
      	'data' => $profiles,
      ),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::get_map()
   */
  function get_map($parser_class, $name) {
    switch ($name) {
      case 'formats':
        $def = array(
        	'mappers' => array(
          	'permission' => array(
            	'mapper' => new iw_mapper('permission', array('format')),
            	'process' => array(array($this, 'format_permission_name')),
            ),
          	'@rid' => new iw_db_mapper('role', 'rid', array('roles' => 'name'), TRUE),
          ),
        	'writers' => array(
            new iw_format_writer(),
            new iw_permission_writer(),
          ),
        );
        return $this->create_datamap($parser_class, $def);
      case 'wysiwyg':
        $def = array(
          'mappers' => array(
            'settings' => array(
              'mapper' => new iw_mapper('settings', array('settings')),
              'process' => array('serialize'),
            ),
          ),
          'writers' => array(
            new iw_db_writer('wysiwyg', array('format')),
          ),
        );
        return $this->create_datamap($parser_class, $def);
    }
    return FALSE;
  }
}