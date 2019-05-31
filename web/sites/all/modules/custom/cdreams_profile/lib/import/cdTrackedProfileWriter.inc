<?php
/**
 * @file
 * Class used to import profiles.
 */


class cdTrackedProfileWriter extends iwEntityWriter {
  const ENTITY_TYPE = 'cdreams_profile';
  
  /**
   * Report back the customizable writer settings
   * which are available.
   * 
   * @return array
   *  Return the configurable package properties for the
   *  node writer.
   */
  static function getPackageProps() {
    return array(
      'bundle'  => array('label' => t('Entity bundle')),
      'status'  => array('label' => t('Import profile as published')),
    );
  }
  
  
  // ----------------------------------
  // Instance variables
  // ----------------------------------
  
  protected $status = IWAPI_ACTIVE;

  
  // ----------------------------------
  // Instance methods
  // ----------------------------------

  /**
   * Create a node writer that is compatible with a specific node type.
   *
   * @param $nodeType
   *  String representing the node type that we attempt to build
   *  a writer for.
   * @param $mode
   *  The writing mode used by this writer. Determines how existing
   *  entries are updated or inserted.
   *
   * @throws iwException
   */
  function __construct(array $values = array()) {
    parent::__construct($values);

    // only allow new nodes. Allowing overwrites / updates
    // can have adverse effects that I'll hold off on dealing with.
    $this->mode = static::ALLOW_INSERT;
    
    // Required fields
    $this->required['type'] = 'type';
    $this->required['name'] = 'name';
    
    // Allowed fields
    $this->allowed['name']     = 'name';
    $this->allowed['status']   = 'status';
    $this->allowed['created']  = 'created';
    $this->allowed['changed']  = 'changed';    
    $this->allowed['orig_nid'] = 'orig_nid';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    if (!empty($this->bundle)) {
      $data['type'] = $this->bundle;
    }

    if ($error = $this->validate($data)) {
      drupal_set_message($error, 'error');
      return FALSE;
    }
    
    // Save non-field items
    $pass_thru = array('uid', 'status', 'created', 'changed');
    foreach ($pass_thru as $extra) {
      if (!empty($this->{$extra})) {
        $data[$extra] = $this->{$extra};
      }
    }

    $profile = new cdProfileEntity($data);
    $profile->name = trim($profile->name);
    
    // If invalid user found, then re-assign to the administrator ID.
    if (empty($profile->uid) || !user_load($profile->uid)) {
      global $user;
      $profile->uid = $user->uid;
    }
    
    $this->applyFields($profile, $data);
    $profile->save();
    
    if (!empty($profile->profile_id) && !(empty($data['created']) && empty($data['changed']))) {
      $allowed = array_fill_keys(array('created', 'changed'), TRUE);
      
      db_update('cdreams_profiles')
        ->fields(array_intersect_key($data, $allowed))
        ->condition('profile_id', $profile->profile_id)
        ->execute();
    }

    if (module_exists('d6_importer')) {
      // Save a lookup between the old and new node id.
      db_merge('d6_importer_lookup')
        ->key(array('old' => $data['orig_nid'], 'data_type' => 'node'))
        ->fields(array(
          'entity_type' => 'cdreams_profile',
          'id' => $profile->profile_id,
        ))->execute();
    }
  }
}
