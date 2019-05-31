<?php
/**
 * @file iwWriters.inc
 *  Writers are used by the data mapping functionality to write data
 *  to their respective destinations. With a writer you specify what
 *  is allowed and required.
 */


/**
 * Writer class for creating filter formats.
 *  Essentially a wrapper for filter_format functions.
 */
class iwFormatWriter extends iwWriter {
  
  function __construct($mode = self::ALLOW_ANY) {
    $this->mode = $mode;
    
    $this->ids      = array('format' => 'format');
    $this->required = array('format' => 'format', 'name' => 'name');
    $this->allowed  = array('format', 'name', 'cache', 'status', 'weight', 'filters');

    // Combine the allowed fields.
    $this->allowed = array_combine($this->allowed, $this->allowed);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    return filter_format_exists($data['format']);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array $data) {
    if (!$this->validate($data)) {
      return FALSE;
    }
    
    $data = array_intersect_key($data, $this->allowed);
    $exists = $this->exists($data);
    if (($exists && ($this->mode & self::ALLOW_UPDATE) !== 0) || 
      (!$exists && ($this->mode & self::ALLOW_INSERT) !== 0)) {
      return filter_format_save((object)$data);
    }
    return FALSE;
  }
}

/**
 * Very similar to the database writer, this is needed
 *  due to the special triggers called by the user_role_save()
 */
class iwRoleWriter extends iwWriter {
  
  static function getSummaryProps() {
    return array();
  }
  
  /**
   *
   */
  static function getPackageProps() {
    // All the settings for this writer are canned
    return array();
  }
  
  
  function __construct($mode = self::ALLOW_ANY) {
    $this->mode = $mode;
  
    $this->ids = array('rid' => 'rid');
    $this->required = array('name' => 'name');
    $this->allowed = array('rid' => 'rid', 'name' => 'name', 'weight' => 'weight');
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    return !empty($data['rid']);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array $data) {
    if (!$this->validate($data)) {
      return FALSE;
    }
    return user_role_save((object)$data);
  }
}


/**
 * Write entries into the permissions tables.
 */
class iwPermissionWriter extends iwDatabaseWriter {

  /**
   *
   */
  static function getPackageProps() {
    // All the settings for this writer are canned
    return array();
  }

  function __construct() {
    $this->mode = self::ALLOW_INSERT;
    $this->table = 'role_permission';

    $this->ids      = array('rid' => 'rid', 'permission' => 'permission');
    $this->required = array('rid' => 'rid', 'permission' => 'permission', 'module' => 'module');
    $this->allowed  = $this->required;
  }

  /**
   * Ensure all required fields are present and that
   *  permission is not empty.
   *
   * @see iwDatabaseWriter::validate()
   */
  function validate($data) {
    return parent::validate($data) && !empty($data['permission']);
  }

  /**
   * (non-PHPdoc)
   * @see iwDatabaseWriter::save()
   */
  function save(array $data) {
    if (!$this->validate($data)) {
      return FALSE;
    }

    if (is_array($data['rid'])) {
      $ret = array();
      $rids = $data['rid'];
      foreach ($rids as $rid) {
        $data['rid'] = $rid;
        $ret[] = parent::save($data);
      }
      return $ret;
    }
    else {
      return parent::save($data);
    }
  }
}
