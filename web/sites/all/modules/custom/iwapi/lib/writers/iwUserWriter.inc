<?php
/**
 * @file
 * User entity writer, which create / alters a user account.
 */


class iwUserWriter extends iwEntityWriter {
  const ENTITY_TYPE = 'user';
  
  /**
   * In the case of users, we don't want to allow
   * the write mode to change, and only want to expose
   * the '$addRoles' property for setting roles to 
   * add to add users imported with this writer.
   * 
   * @return array
   *  Array with data about the exportable properties.
   */
  static function getPackageProps() {
    return array(
      'addRoles' => array(
        'label' => t('Additional roles'),
        'multi' => TRUE,
      ),
      'requireMail' => array('label' => t('Require an e-mail')),
      'status'      => array('label' => t('Imported user active')),
    );
  }
  
  
  // -----------------------------
  // Instance properties
  // -----------------------------
  
  protected $addRoles    = array();  // Additional roles that can be applied to all imported users.
  protected $status      = 0;        // the status of imported users.
  protected $requireMail = TRUE;     // Some modules make it so e-mail is not required, so allow for config.

  /**
   * Create a node writer that is compatible with a specific node type.
   *
   * @param $nodeType
   *  String representing the node type that we attempt to build
   *  a writer for.
   * @param $mode
   *  The writing mode used by this writer. Determines how existing
   *  entries are updated or inserted.
   */
  function __construct(array $def = array()) {   
    parent::__construct($def);
    
    // Do not allow user imports to do anything
    // other than create new accounts. Modifying
    // could cause unexpected ramifications.
    $this->mode = static::ALLOW_INSERT;
    
    if (!empty($this->addRoles)) {
      // Filter the values, and set them to the correct
      // and existing {users_roles}.rid values.
      $this->addRoles = array_intersect(user_roles(TRUE), $this->addRoles);
    }
    
    // Add references to required fields
    $addl = array('name', 'pass');
    if ($this->requireMail) {
      $addl[] = 'mail';
    }
    
    $addl = array_combine($addl, $addl);
    $this->required += $addl;
    $this->allowed  += $addl;
    
    // always allow mail!
    $this->allowed['mail'] = 'mail';
    
    // Add some safe user preferences that can get imported.
    $this->allowed['timezone'] = 'timezone';
    $this->allowed['language'] = 'language';
  }

  /**
   * (non-PHPdoc)
   * @see iwEntityWriter::exists()
   */
  function exists(array $data) {    
    // Build initial query. Username always unique!
    $query = db_select('users', 'u')
      ->condition('u.name', $data['name']);

    $query->addExpression('COUNT(1)');
    
    // Is mail is required and available?
    if (isset($data['mail'])) {
      $query->condition('u.mail', $data['mail']);
      
      $conds = &$query->conditions();
      $conds['#conjunction'] = 'OR';
    }
    
    return (bool) $query->execute()->fetchField();
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    $error = $this->validate($data);
    if (!empty($error)) {
      throw new InvalidArgumentException(t($error));
    }
    
    if ($this->exists($data)) {
      throw new Exception($data['name'] .' already exists.');
    }

    $account = new stdClass();
    $this->applyFields($account, $data);
    
    // Transfer the data into the an edit array. Users editing
    // behaves differently from any other entity saving.
    $edit = (array)$account;
    $edit = array_intersect_key($data, $this->allowed);
    
    if (!isset($edit['status'])) {
      $edit['status'] = $this->status;
    }
    
    // Lock the roles of the new user to the roles set in this writer.
    $edit['roles'] = $this->addRoles;
    
    if ($account = user_save($account, $edit)) {
      if (empty($data['uid'])) {
        $data['uid'] = $account->uid;
      }
    }
    else {
      throw new iwException('Unable to create the user %name', array('%name' => $data['name']));
    }
  }
}
