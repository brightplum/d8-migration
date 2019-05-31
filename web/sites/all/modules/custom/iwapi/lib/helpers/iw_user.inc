<?php
/**
 * @file iw_user.inc
 *  Helper class for users (permissions, roles, etc).
 */

class iw_user extends iw_helper {
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::get_info()
   */
  function get_info() {
    return array(
      'maps' => array('roles'),
      'import' => array('roles', /*'permissions', 'users'*/),
      'export' => array('roles'),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::get_map()
  */
  function get_map($parser, $name) {
    switch ($name) {
      case 'roles':
        $def = array(
          'mappers' => array(
            '@rid' => new iw_db_mapper('role', 'rid', array('name')),
          ),
          'writers' => array(
            new iw_role_writer('role', array('name')),
          ),
        );
        return $this->create_datamap($parser, $def);
    }
    return FALSE;
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::get_export_options()
   */
  function get_export_options($name) {
    switch ($name) {
      case 'roles':
        return user_roles(TRUE);
    }
    return array();
  }
  
  /**
   * (non-PHPdoc)
   * @see iw_helper::import()
   */
  function import($name, array $package)  {
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
   * Export a set or roles that can be later imported.
   * - rid fields are left out, due to the fact they can
   *  be different accross different systems.
   * 
   * @param array $conditions
   *  If set, only return roles in the conditions array.
   * @return array
   *  The role definitions exported.
   */
  function export_roles(array $conditions = array()) {
    $roles = user_roles(TRUE);
    if (!empty($conditions)) {
      $roles = array_intersect($roles, $conditions);
    }
    
    // Fetch the full information for these roles, user_roles()
    //  does not return a full entry (missing weight field).
    $rs = db_select('role', 'r')
      ->fields('r', array('name', 'weight'))
      ->condition('name', $roles)
      ->execute();
    
    $values = array();
    while ($value = $rs->fetchAssoc()) {
      $values[$value['name']] = $value;
    }
    
    // Return the packaged set of data
    return empty($values) ? array() : array(
      array(
        'helper' => 'iw_user',
        'type' => 'roles',
        'requires' => array('user'),
        'data' => $values,
      ),
    );
  }
}