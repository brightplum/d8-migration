<?php
/**
 * @file
 * Base class for using field group handlers to group fields.
 */


class iwFieldGroupController {
  
  const CssName = 'field-wrapper';
  
  /**
   * Maintains a cache of fieldgroup settings that
   * were already loaded previously.
   * 
   * @var array
   */
  static private $_cache = array();
  
  /**
   * Reset the internal object cache for a given
   * object type or bundle combination.
   *  
   * @param string $type
   *  Name of the entity type for which to reset the cache.
   * @param string $bundle
   *  The entity bundle for which to reset the cache.
   * @param string $view_mode
   *  The 
   */
  static function ResetCache($scope, $name = NULL) {
    if (isset($scope)) {
      if (isset($name)) {
        // Reset a specific item.
        unset(static::$_cache[$scope][$name]);
      }
      else {
        // Reset the entire scope.
        unset(static::$_cache[$scope]);
      }
    }
    else {
      unset(static::$_cache); // reset everything!
    }
  }

  /**
   * Get the scope name relating to the entity
   * bundle and view mode of a field group.
   *
   * @param string $entity_type
   *  The entity type machine name.
   * @param string $bundle
   *  Name of the bundle the scope will apply to.
   * @param string $view
   *  Name of the view mode for which this scope
   *  is built for.
   * @return string
   *  The scope name that applies to these parameters.
   */
  static function GetScope($entity_type, $bundle, $view) {
    return strtolower("$entity_type|$bundle|$view");
  }
  
  /**
   * 
   * @param type $scope
   * @return type
   */
  static function ParseScope($scope) {
    return explode('|', $scope);
  }
  
  /**
   * Get the list of all wrapper class name and label pairs 
   * for wrappers that can be supported by this controller.
   * 
   * This allows every wrapper controller to have its own
   * set of allowed wrapper types while keeping the global
   * scope clean.
   * 
   * @return array
   *  Name value pairs of wrapper classes that can be
   *  used by this controller type. The array key is
   *  the class name and value is description text.
   */
  static function GetWrapperTypes() {
    static $wrappers;
    
    if (!isset($wrappers)) {
      $wrappers = _iwapi_extract_field(iwapi_get_class_info('wrapper'));
      
      if ($cache = cache_get('iwformatter:wrapper_types')) {
        $wrappers += $cache->data;
      }
      else {
        $group_wrappers = module_invoke_all('iwformatter_wrapper_types');
        cache_set('iwformatter:wrapper_types', $group_wrappers);
        $wrappers += $group_wrappers;
      }
 
      asort($wrappers); // make the list sorted
    }
    
    return $wrappers; // Return the set of valid field wrappers.
  }
  
  /**
   * Load field group settings for a given entity type
   * and bundle combination.
   * 
   * @param object $entity
   *  A loaded entity object which we wish to locate
   *  fieldgroup settings for.
   * @param string $type
   *  The entity type for which $entity belongs.
   */
  static function LoadContainers($type, $bundle, $view_mode = 'default') {
    $scope = static::GetScope($type, $bundle, $view_mode);
    
    // If not already loaded, try find that information.
    if (!isset(static::$_cache[$scope])) {
      $rs = db_select('iwapi_containers', 'container')
        ->fields('container')
        ->condition('scope', $scope)
        ->execute();
      
      static::$_cache[$scope] = array();
      while ($group = $rs->fetchAssoc()) {
        // Add additional data to the top level
        if ($data = unserialize($group['data'])) {
          $group += $data;
        }
        unset($group['data']);
        
        $groupName = 'wrapper_' . $group['name'];
        static::$_cache[$scope][$groupName] = new $group['class_name'](get_called_class(), $group);
      }
    }

    // Return all the built fieldgroup configurations.
    return static::$_cache[$scope];
  }
  
  /**
   * Loads a single field grouping item.
   * 
   * @param string $name
   *  Name of the content wrapper, unique for this scope.
   * @param string $type
   *  The machine name of the entity type used.
   * @param string $bundle
   *  The machine name of the bundle.
   * @param string $view
   *  The view mode.
   * @return iwContentWrapper
   *  A loaded instance of the content wrapper
   *  matching the scope and 
   */
  static function LoadContainer($name, $type, $bundle, $view = 'default') {
    $scope = static::GetScope($type, $bundle, $view);

    $groupName = 'wrapper_'. $name;
    if (!isset(static::$_cache[$scope][$groupName])) {
      $group = db_select('iwapi_containers', 'container')
        ->fields('container')
        ->condition('scope', $scope)
        ->condition('name', $name)
        ->execute()->fetchAssoc();

      if ($group && class_exists($group['class_name'])) {
        if ($data = unserialize($group['data'])) {
          $group += $data;
        }
        unset($group['data']);
        
        return new $group['class_name'](get_called_class(), $group);
      }
    }
    return FALSE;
  }
}
