<?php
/**
 * @file
 * Contains the controlling class to managing CSP flows.
 */

class CspFlow {

  const FLAG_ADMIN       = 0x01;
  const FLAG_PUBLISH     = 0x08;
  const RESTRICTED_STATE = 0x09; // FLAG_ADMIN | CSP_PUBLISH_FLAG 


  // ---------------------------------
  // Instance properties and handling.
  // ---------------------------------
  
  public $flow;
  public $name;
  public $label = '';
  
  public $states = array();
  
  protected $publishes;  // reports if this workflow has anything to do with publishing.
  protected $admins;
  
  
  /**
   * Create a new CspFlow object with the array values.
   * 
   * @param array $values
   *  Values to populate in the current class.
   */
  function __construct(array $values = array()) {
    $vars = get_class_vars(get_class($this));  
    
    // Move values that are available into the object.
    foreach ($vars as $key => $default) {
      if (!empty($values[$key])) {
        $this->{$key} = $values[$key];
      }
    }
  }
  
  /**
   * Reports if this instance has knowledge
   * that it has been saved to the database.
   * 
   * @return boolean
   *  TRUE iff this instance has not been
   *  saved to the database, FALSE otherwise.
   */
  function isNew() {
    return empty($this->flow);
  }

  /**
   * Allows CspFlow to control the loading of the
   * list of administrators. CspFlow uses lazy load
   * since this is not used for most operations.
   * 
   * @return &array
   *  Reference to array of user objects representing
   *  each user considered an administrator of this workflow.
   */
  function &getAdmins() {
    if (!isset($this->admins)) {
      $uids = db_select('csp_admins', 'admins')
        ->fields('admins', array('uid'))
        ->condition('flow', $this->flow)
        ->execute()->fetchCol();
      
      $this->admins = entity_load('user', $uids);
    }
    
    return $this->admins;
  }
  
  /**
   * Return the states which are available to
   * the owner of the given account.
   * 
   * @param stdClass $account
   *  Account to check permissions to see
   *  which states this user can access.
   * @return array
   *  Array of states keyed by state name
   *  and has values which are the labels.
   */
  function allowedStates($entity, $type, stdClass $account) {
    if (user_access(CSP_PERM_ADMIN, $account) || user_access("use admin CSP $this->flow states", $account)) {
      $states = $this->states;
      array_walk($states, '_iwapi_extract_array_field', 'label');
      return $states;
    }

    // Does user own this entity.
    $owned = $entity->uid == $account->uid;

    // Regular users need to check what is available to them.
    if (user_access("use any CSP $this->flow states", $account) 
      || ($owned && user_access("use own CSP $this->flow states", $account))) {
  
      if (!isset($this->unrestricted)) {
        // Find all states that don't have access restrictions on them.
        foreach ($this->states as $name => $sInfo) {
          if (($sInfo['flags'] & static::RESTRICTED_STATE) === 0) {
            $this->unrestricted[$name] = $sInfo['label'];
          }
        }
      }
      
      return $this->unrestricted;
    }
 
    return array();
  }
  
  /**
   * Determine if user has access to this workflow state for
   * this state and entity combination.
   * 
   * @param object $entity
   *  The entity for which the state is checked in conjunction with.
   * @param string $state
   *  The state to check permissions against.
   * @param stdClass $account
   *  User account to check permissions for.
   */
  function checkAccess($entity, $type, $op, $state, stdClass $account) {
    // administrators have free reign! including missing / deleted states.
    if (user_access(CSP_PERM_ADMIN, $account) || user_access("use admin CSP $this->flow states", $account)) {
      return TRUE;
    }

    // What workflow states does this user have access to?
    $allowed = $this->allowedStates($entity, $type, $account);
    if (empty($allowed)) {
      return FALSE; // no allowed states, means user can't do anything here.
    }
    
    // Is the requested state in the allowed list?
    if (key_exists($state, $allowed)) {
      return TRUE;
    }
    elseif (!in_array($op, array('delete', 'remove')) && user_access("alter published CSP $this->flow entity", $account)) {
      $published = ($type == 'node') && ($entity->status == NODE_PUBLISHED);
      $published = $published || ($entity instanceof iwEntity && $entity->isActive());
      
      // last opportunity for some users, edit published entity
      return $published && ($this->states[$state]['flags'] & static::FLAG_ADMIN) == 0;
    }
 
    return FALSE;
  }
  
  /**
   * Save the workflow state of this entity.
   * 
   * @param object $entity
   *  The entity to save the
   * @param string $type
   *  Entity type for which this state is being
   *  saved for.
   * @param string $state
   *  The state that should get saved for this
   *  entity information.
   */
  function saveState($entity, $type, $state) {
    if (empty($this->states[$state])) {
      throw new InvalidArgumentException(
        "Cannot set state to \"$state\" for the $this->label workflow"
      );
    }
    
    // Build the values and items.
    list($id, $rev) = entity_extract_ids($type, $entity);
    
    $keys = array(
      'entity_type' => $type,
      'entity_id' => $id,
      'flow' => $this->flow,
    );
    $values = array(
      'state' => $state,
      'changed' => REQUEST_TIME,
    );
    
    // ---------------------------------------
    // If the entity is revisioned then count the revision as part of the PK
    if (isset($rev)) {
      $keys['rev'] = $rev;
    }
    else {
      $values['rev'] = $id;
    }

    // Run the query 
    db_merge('csp_entities')->key($keys)
      ->fields($values)
      ->execute();
  }
  
  /**
   * Change the workflow state of an entity, and update
   * publishing / handling if necessary.
   * 
   * @param object $entity
   *  Entity for which to change the workflow state for.
   * @param string $type
   *  The type of entity being worked with.
   * @param string $state
   *  The name of the state to change to.
   */
  function alterState($entity, $type, $state) {
    static $processed;

    // Manage the publishing state if this workflow contributes to that.
    if ($this->publishes) {
      // Objects being created might not have id/vid yet.
      $info = entity_get_info($type);
      $id   = isset($entity->{$info['entity keys']['id']}) ? $entity->{$info['entity keys']['id']} : 'new';
   
      // ++++++
      // Workflows that can publish are OR'd together to determine
      // if the entity should be published, so if any one of them
      // are in a published state, publish the entity.
      //
      // Change to AND by setting the default state to TRUE, and 
      // use use '&' instead of '|' when applying the current 
      // state publishing flag / status.
      
      $publish = FALSE;
      if ($type == 'node') {
        if (isset($processed[$type][$id])) {
          $publish = ($entity->status == NODE_PUBLISHED); // more than one workflow contributes to publishing.
        }
        else {
          $entity->status = NODE_NOT_PUBLISHED;
        }
      }
      elseif ($entity instanceof iwEntity) {
        if (isset($processed[$type][$id])) {
          $publish = $entity->isActive();
        }
        else {
          $entity->setActive(FALSE);
        }
      }
      
      // React to the publishing state of the entity.
      $publish |= (bool)($this->states[$state]['flags'] & static::FLAG_PUBLISH);
      if ($type == 'node') {
        $entity->status = $publish ? NODE_PUBLISHED : NODE_NOT_PUBLISHED;
      }
      elseif ($entity instanceof iwEntity) {
        $entity->setActive((bool)$publish);
      }
      
      $processed[$type][$id] = TRUE;
    }
  }
  
  
  // --------------------------
  // CRUD / Maintence
  // --------------------------  
  
  /**
   * Either insert or update the the workflow structure settings.
   */
  function save() {
    $trans = db_transaction('csp_flow_'. $this->flow);
    try {
      $vars   = get_class_vars(get_class($this));
      $values = array_intersect_key((array)$this, $vars);
      
      $schema = drupal_get_schema('csp_flows');
      $values['data'] = serialize(array_diff_key($values, $schema['fields']));
      $values = array_intersect_key($values, $schema['fields']);
      
      // Set the publishes flag based on if there are any states that contribute
      // to publishing. This is helpful so we don't have to scan the all the 
      // workflow states to know if we should account for publishing functionality.
      $values['publishes'] = FALSE;
      foreach ($this->states as $sInfo) {
        $values['publishes'] |= (bool)($sInfo['flags'] & static::FLAG_PUBLISH);
      }
      
      // Inserting is simplier than updating if states change.
      if ($this->isNew()) {
        $values['handler'] = get_class($this);
        unset($values['flow']);
        
        $this->flow = db_insert('csp_flows', array('return' => Database::RETURN_INSERT_ID))
          ->fields($values)
          ->execute();
      }
      else {
        // 1.) Load the original flow contents
        $rs = db_select('csp_flows', 'fl')
          ->fields('fl', array('data'))
          ->condition('fl.flow', $this->flow)
          ->execute();
        
        $data = unserialize($rs->fetchField());
        $rs->closeCursor();
        
        // 2.) Alter entities with states that no longer exist to the default state / status.
        $diff_states = isset($data['states']) && is_array($data['states'])
          ? array_diff_key($data['states'], $this->states) : NULL;
        
        if (!empty($diff_states)) {
          reset($this->states);

          db_update('csp_entities')
            ->fields(array('state' => key($this->states)))
            ->condition('flow', $this->flow)
            ->condition('state', array_keys($diff_states))
            ->execute();
        }

        // TODO: Evaluate changes to publishing state?
        // Update all entities in states relating to publishing.
        // 3.) ???
        
        // 4.) finally update changes to the state.
        db_update('csp_flows')->fields($values)
          ->condition('flow', $this->flow)
          ->execute();
      }
      
      // Update list of administrators if needed.
      if (isset($this->admins)) {
        db_delete('csp_admins')
          ->condition('flow', $this->flow)
          ->execute();
        
        // Insert updated list of administrators.
        $admin_ins = db_insert('csp_admins')->fields(array('flow', 'uid'));
        foreach ($this->admins as $uid => $account) {
          $admin_ins->values(array($this->flow, $uid));
        }
        
        $admin_ins->execute();
      }
      
      // Flush the cache of this instance.
      CspController::ResetCache($this->flow);
    }
    catch (Exception $e) {
      watchdog_exception('CSP Flow', $e);
      $trans->rollback();
      throw $e;
    }
  }
  
  /**
   * Delete the workflow instance from the database.
   */
  function delete() {
    $trans = db_transaction('csp_flow_'. $this->flow);
    
    try {
      foreach (array('csp_entities', 'csp_admins', 'csp_flows') as $table) {
        db_delete($table)
          ->condition('flow', $this->flow)
          ->execute();
      }
      
      // Flush the cache of this instance.
      CspController::ResetCache($this->flow);
    }
    catch (Exception $e) {
      watchdog_exception('CSP Flow', $e);
      $trans->rollback();
      throw $e;
    }
  }
}
