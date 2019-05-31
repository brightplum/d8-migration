<?php
/**
 * Contains the global and management functions for CSP workflows.
 */

class CspController {

  // --------------------------------
  // Static properties and handling.
  // --------------------------------
    
  static private $_cache     = array();
  static private $_allLoaded = FALSE;
  
  static function GetEntityVariable($type) {
    return variable_get("csp_{$type}_bundles", array());
  }
  
  
  /**
   * Load a new instance of the CSP workflow with the provided ID.
   * 
   * @param int $flow
   *  The ID of the flow structure to load from the database.
   * @return CspFlow | FALSE
   *  If a matching flow is found, return a CspFlow instance
   * otherwise FALSE (boolean).
   */
  static function Load($flow) {
    $flow = intval($flow);  // Ensure reference to an integer value.
    
    if ($flow <= 0) {
      return FALSE;
    }
    
    // Try to load from the database if the flow was not found.
    if (!isset(static::$_cache[$flow])) {
      $row = db_select('csp_flows', 'fl')->fields('fl')
        ->condition('fl.flow', $flow)
        ->execute()->fetchAssoc();

      if ($row) {
        $row += unserialize($row['data']);
        $handler = $row['handler'];
        
        static::$_cache[$flow] = new $handler($row);
      }
      else {
        static::$_cache[$flow] = FALSE;
      }
    }
    return static::$_cache[$flow];
  }
  
  /**
   * Load multiple workflows all at once, keyed by ID.
   * 
   * @param array $ids
   *  The ID of the flow structure to load from the database.
   * @return array
   *  Return all the available workflows with the passed in $ids.
   *  Will return an empty array if no matches were found.
   */
  static function LoadMultiple(array $ids) {
    $ids = array_combine($ids, $ids);
 
    $loaded  = array_intersect_key(static::$_cache, $ids);
    $missing = array_diff_key($ids, $loaded);

    if (!empty($missing)) {
      $rs = db_select('csp_flows', 'fl')->fields('fl')
        ->condition('fl.flow', $missing)
        ->execute();

      while ($row = $rs->fetchAssoc()) {
        $row += unserialize($row['data']);
        $handler = $row['handler'];
        
        static::$_cache[$row['flow']] = $loaded[$row['flow']] = new $handler($row);
      }
    }
    
    return $loaded;
  }
  
  /**
   * Get all available workflow object.
   * 
   * @return array
   *  Containing all the loaded workflow objects.
   */
  static function LoadAll() {
    if (!static::$_allLoaded) {
      $rs = db_select('csp_flows', 'fl')->fields('fl')->execute();

      while ($row = $rs->fetchAssoc()) {
        $row += unserialize($row['data']);
        
        $handler = $row['handler'];
        static::$_cache[$row['flow']] = new $handler($row);
      }
  
      static::$_allLoaded = TRUE;
    }

    return static::$_cache;
  }
  
  /**
   * Flush either a single entry (iff $flow param not null)
   * or clear the entire cache of workflow instances.
   * 
   * @param int|NULL $flow
   *  The ID of the flow to flush from the cache.
   */
  static function ResetCache($flow = NULL) {
    static::$_allLoaded = FALSE;

    if (isset($flow)) {
      unset(static::$_cache[$flow]);
    }
    else {
      static::$_cache = array();
    }
  }
  
  /**
   * Load all the CspWorkflow objects that apply to
   * the entity passed in as an argument.
   * 
   * @param object $entity
   *  The entity being worked on / to retrieve CspFlow for.
   * @param string $type
   *  Machine name of the entity type.
   * @return array
   *  An array of the applicable workflow objects keyed
   *  by their {csp_flows}.flow ID. Array is empty if no
   *  workflows apply to this entity.
   */
  static function EntityWorkflows($entity, $type) {      
    $info     = entity_get_info($type);
    $settings = static::GetEntityVariable($type);
 
    $bundle = empty($info['entity keys']['bundle']) ? $type : $entity->{$info['entity keys']['bundle']};
    return empty($settings[$bundle]) ? array() : static::LoadMultiple($settings[$bundle]);
  }
  
  /**
   * Load all the entity states that apply to the workflows pass in the
   * $workflows argument. If left blank, all workflows found by
   * CspController::EntityWorkflows() will be used.
   * 
   * @param object $entity
   *  Entity to have all the workflow information added to it.
   * @param string $type
   *  Machine name of the entity type of $entity.
   * @param array $workflows
   *  A list of workflows to load the state for. Must by keyed
   *  by the CspFlow {csp_flows}.flow ID. If unset, all workflows
   *  will be searched for.
   */
  static function LoadEntityStates($entity, $type, $workflows = NULL, $setDefault = TRUE) {
    if (!isset($workflows)) {
      $workflows = static::EntityWorkflows($entity, $type);
    }

    if (isset($entity->csp)) {
      $workflows = array_diff_key($workflows, $entity->csp);
    }
   
    // Only workflow states if they haven't already been loaded.
    if (!empty($workflows)) {
      list($id, $rev) = entity_extract_ids($type, $entity);
      
      $query = db_select('csp_entities', 'ent')
        ->fields('ent', array('flow', 'state'))
        ->condition('entity_type', $type)
        ->condition('entity_id', $id)
        ->condition('flow', array_keys($workflows));

      // Check the revisioning of the module.
      if (isset($rev)) {
        $query->condition('rev', $rev);
      }
      $entity->csp = $query->execute()->fetchAllKeyed(0, 1);

      if ($setDefault) {
        // For empty items, find and setup a default workflow state.
        foreach (array_diff_key($workflows, $entity->csp) as $id => $flow) {
          reset($flow->states);
          $entity->csp[$id] = key($flow->states);
        }
      }
    }
  }
}
