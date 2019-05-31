<?php
/**
 * @file includes/entity/iwEntity.inc
 * The base abstract class for use with creating entities.
 */

/**
 * Abstract class used to contain a base entity.
 */
abstract class iwEntity {

  /**
   * Static getter function to report the database
   * table containing base entity data.
   *
   * @return string
   *  Name of the table in the database without prefixes.
   */
  static function getTable() {
    return static::$Table;
  }

  /**
   * Static function use to retrieve the entity keys.
   *
   * @return array
   *  The key definitions for the entity. The names of the
   *  object's ID, bundle, status and revision properties.
   */
  static function getKeys() {
    return static::$Keys;
  }

  /**
   * Create a new role profile entity object.
   */
  function __construct(array $values = array()) {
    // Assign values based on passed in values
    foreach ($values as $name => $value) {
      $this->{$name} = $value;
    }
  }

  /**
   * Retrieve the unique identifier name for this item.
   * 
   * @return integer
   *  The unique ID for this entity. Generally the ID is array
   *  serial field in the database, but technically could be a
   *  regular string as well.
   */
  function getId() {
    $idKey = static::$Keys['id'];
    return isset($this->{$idKey}) ? $this->{$idKey} : NULL;
  }

  /**
   * Get the string machine name of this product's bundle.
   * 
   * @return string
   *  Name of the bundle for this entity. Defaults to the Entity
   *  type if a bundle or bundle key is not available.
   */
  function getBundle() {
    if (!empty(static::$Keys['bundle'])) {
      $bundleKey = static::$Keys['bundle'];
      return isset($this->{$bundleKey}) ? $this->{$bundleKey} : $this->entityType();
    }

    return $this->entityType();
  }

  /**
   * Get the current status of this entity. It will first check if
   * there is an entity key specified as the status key, otherwise,
   * it will use the default key 'status'.
   * 
   * @return int
   *  The status as indicated by a number. IWAPI defines 2 possible
   *  statuses which will be used by most entities. IWAPI_ACTIVE and
   *  IWAPI_INACTIVE to indicate active and inactive entities respectively.
   *  Undefined status will return IWAPI_INACTIVE.
   */
  function getStatus() {
    $statusKey = isset(static::$Keys['status']) ? static::$Keys['status'] : 'status';

    return isset($this->{$statusKey}) ? $this->{$statusKey} : IWAPI_INACTIVE;
  }

  /**
   * Try to determine if the current page is the display page
   * for this entity item.
   * 
   * @return bool
   *  TRUE iff the current page is the full page view of this
   *  entity object. False otherwise.
   * 
   * @see node_is_page().
   */
  function isPage() {
    if ($entity = menu_get_object('iwapi_entity_object', 1)) {
      $idKey = static::$Keys['id'];
    
      // check if entity type and ID match up.
      return $entity->entityType() === $this->entityType()
        && $entity->{$idKey} == $this->{$idKey};
    }
    return FALSE;
  }
  
  /**
   * Check if this entity should be considered active.
   * 
   * @return bool
   *  TRUE iff the entity should be considered published / active.
   *  FALSE otherwise (for pending states, or other inactive modes).
   */
  function isActive() {
    return $this->getStatus() === IWAPI_ACTIVE;
  }
 
  /**
   * Knows how to set the active / published state for
   * this entity type. By default sets the entity to active
   * but can be used to deactivate the entity status too.
   * 
   * @param boolean $active
   *  Whether to active or deactive the entity's status.
   */
  function setActive($active = TRUE) {
    $statusKey = isset(static::$Keys['status']) ? static::$Keys['status'] : 'status';
 
    $this->{$statusKey} = $active ? IWAPI_ACTIVE : IWAPI_INACTIVE;
  }
  
  /**
   * Function to get the entity type name for this
   * entity object. Generally static text, as these
   * classes tend to support only a single entity type.
   */
  abstract function entityType();
  
  /**
   * Get the recommended label name for this role profile.
   *
   * For role profiles this will be the friendly name
   * of the profile type.
   */
  abstract function getLabel();
 
  /**
   * Check an account should be granted permission to
   * perform a given operation on a given entity.
   * 
   * @param string $op
   *  The operation about to be performed on the entity.
   * @param stdClass $account
   *  User account being used to confirm access for.
   * @return bool
   *  TRUE if user should be granted access to this
   *  entity for the requested operation.
   */
  abstract function checkAccess($op, stdClass $account, $only_admin = FALSE);
  
  /**
   * Generate the URI for the full page view of this entity.
   * 
   * @return array
   *  The URI parameters with 'path' set to the profile page.
   */
  function getUri() {
    $base = strtr($this->entityType(), '_', '-') .'/';
    return array('path' => $base. $this->{static::$Keys['id']});
  }

  /**
   * Any special functionality this class wants to 
   * implement before saving the entity data.
   */
  function presave() {
    /* Allow subclasses to do any special handling before saving. */
  }
  
  /**
   * Called just before the completion of save or delete action
   * and only invoked if the action is sucessful. Gives the
   * entity a chance to handle any tidy-up actions.
   * 
   * @param string $op
   *  Name of the operation which is being finalized on this
   *  entity. Can be one of "insert", "update", or "delete".
   */
  function finalize($op) {
    /* Allow subclasses to do any special handling before saving or deleting. */
  }
  
  /**
   * Persist the entity data and fields.
   *
   * @throws ErrorException
   * @throws Exception
   */
  function save() {
    $type  = $this->entityType();
    $keys  = static::$Keys + array('status' => 'status');
    
    $trans = db_transaction('iwEntity_save');
    try {
      // if not set, add a default status.
      if (!isset($this->{$keys['status']})) {
        $this->{$keys['status']} = IWAPI_ACTIVE;
      }
  
      // Setup some handlers and variables if review is new.
      if (empty($this->{$keys['id']})) {
        $op     = 'insert';
        $db_key = array();
        $this->created = REQUEST_TIME;
      }
      else {
        $op     = 'update';
        $db_key = array($keys['id']);
        $this->original = entity_load_unchanged($type, $this->{$keys['id']});
      }
      $this->changed = REQUEST_TIME;
  
      $this->presave();
      field_attach_presave($type, $this);
      module_invoke_all('entity_presave', $this, $type);
  
      // Actually save the review, and attach the fields.
      if (drupal_write_record($this->getTable(), $this, $db_key) === FALSE) {
        throw new ErrorException('Unable to save entity.');
      }

      $field_attach_callback = 'field_attach_'. $op;
      $field_attach_callback($type, $this);
      
      // Clear the static loading cache.
      $this->finalize($op);
      module_invoke_all('entity_'. $op, $this, $type);
      entity_get_controller($type)->resetCache(array($this->{$keys['id']}));
  
      // Ignore slave server temporarily to give time for the
      // saved entity to be propagated to the slave.
      db_ignore_slave();
      unset($this->original);
    }
    catch (Exception $e) {
      $trans->rollback();
      watchdog_exception($type, $e);
        
      throw $e;
    }
  }
  
  /**
   * Delete this entity. The function will return
   * normally if successful, otherwise an exception
   * with the error will be provided.
   *
   * @throws Exception
   *  Throws a database PDO exception if
   */
  function delete() {
    $type  = $this->entityType();
    $idKey = static::$Keys['id'];
    $trans = db_transaction('iwEntity_delete');
  
    try {
      field_attach_delete($type, $this);
      
      // Delete after calling hooks so that they can query node tables as needed.
      db_delete($this->getTable())
        ->condition($idKey, $this->{$idKey})
        ->execute();

      $this->finalize('delete');
      module_invoke_all('entity_delete', $this, $type);
         
      // Clear the page and block and node_load_multiple caches.
      entity_get_controller($type)->resetCache(array($this->{$idKey}));
    }
    catch (Exception $e) {
      $trans->rollback();
      watchdog_exception($type, $e);
  
      throw $e;
    }
  }
  
  /**
   * Get content ready for viewing by attaching formatted
   * fields and other specialize items for this entity.
   * 
   * @param string $view_mode
   *  The string name of the view mode to use when for
   *  building the viewable content for this entity.
   */
  function buildContent($view_mode, $langcode = NULL) {
    $type  = $this->entityType();
    $idKey = static::$Keys['id'];

    // Determine the language we should be using
    $entities = array($this->{$idKey} => $this);
    
    // Build fields content.
    field_attach_prepare_view($type, $entities, $view_mode, $langcode);
    entity_prepare_view($type, $entities, $langcode);
    
    return field_attach_view($type, $this, $view_mode);
  }
  
  /**
   * Format the entity fields and data into a format
   * that will be renderable by the Drupal system.
   *
   * @param string $view_mode
   *  Indicates which view mode we want to use when
   *  rendering the display of the resulting entity.
   * @return array
   *  Returns a renderable array of the built profile
   *  contents ready for the theming layer.
   */
  function view($view_mode) {
    $type  = $this->entityType();
    $langcode = entity_language($type, $this);
    
    $this->content = $this->buildContent($view_mode, $langcode);
    $this->content += array(
      '#theme'  => 'iwentity',
      '#entity' => $this,
      '#page'   => $this->isPage(),
      '#view_mode' => $view_mode,
    );
    
    // Allow other modules to change entity views.
    module_invoke_all('entity_view', $this, $type, $view_mode, $langcode);
    
    $display = $this->content;
    unset($this->content);
    
    // Make sure that other modules can make changes to the display.
    drupal_alter('entity_view', $display, $type);
    return $display;
  }
  
  /**
   * Preprocess callback to include specific entity elements
   * into the theming variable. This will make them available
   * for theming functions and templates.
   *
   * @param array $element
   *  The display element to render.
   * @param array $vars
   *  Reference to the variables object available to the
   *  theme preprocess call, and will be used to bring
   *  values into the template.
   */
  function preprocess($element, &$vars) {
    $type  = $this->entityType();
    $idKey = static::$Keys['id'];

    // Pass content information along to the template.
    $vars['title']      = check_plain($this->getLabel());
    $vars['page']       = $element['#page'];
    $vars['entity_uri'] = entity_uri($type, $this);

    $vars['content'] = array();
    foreach (element_children($element) as $key) {
      $vars['content'][$key] = $element[$key];
    }

    // Process the field content, and prepare for rendering.
    field_attach_preprocess($type, $this, $vars['content'], $vars);
    
    $type_attr = strtr($type, '_', '-');
    $vars['classes_array'] = array($type_attr, $element['#view_mode']);
    $vars['attributes_array']['id'] = $type_attr .'-'. $this->{$idKey};
  }
}


/**
 * Entity type which will check for pathauto aliasing when
 * saving and deleting content.
 * 
 * Note: This functionality is not useful without the pathauto
 * module and without implementing HOOK_pathauto() for the entity.
 */
abstract class iwAliasedEntity extends iwEntity {

  /**
   * (non-PHPdoc)
   * @see iwEntity::finalize()
   */
  function finalize($op) {
    if (!module_exists('pathauto')) {
      return;
    }
    
    // Manage entity path aliasing
    $type = $this->entityType();
    switch ($op) {
      case 'delete':
        pathauto_entity_path_delete_all($type, $this);
        break;
      case 'insert':
      case 'update':
        $uri = $this->getUri();
        module_load_include('inc', 'pathauto');
        pathauto_create_alias($type, $op, $uri['path'], array($type => $this), $this->getBundle());
        break;
    }
  }
}
