<?php
/**
 * @file
 * Base controller object for managing items in the build stash.
 */


abstract class iwStashController {

  /**
   * Reset either the cache of an individual item
   * or the entire cache of this controller.
   * 
   * @param object|string $obj
   *  Either the object to remove from the cache
   *  or the ID value of the object.
   */
  static function resetCache($id = NULL) {
    if (isset($id)) {
      unset(static::$cache[$id]);
    }
    else {
      static::$cache = array();
    }
  }
  
  /**
   * Function used to return the ID field of the object.
   * 
   * @return string
   *  Should be the unique ID for this object type.
   */
  abstract function extractId($obj);
  
  /**
   * Get the name of the object types
   * stored and used by this object.
   * 
   * @return string
   *  A string indicating the realm / type of object.
   */
  abstract function getType();

  /**
   * Load an object from the database or cache.
   * 
   * @param string $id
   *  The identifier to use in retrieving the
   *  load object.
   */
  function load($obj) {
    $id = is_string($obj) ? $obj : $this->extractId($obj);
    
    if (!isset(static::$cache[$id])) {  
      $obj = db_select('iwapi_build_stash', 'obj')
        ->fields('obj', array('changed', 'uid', 'data'))
        ->condition('type', $this->getType())
        ->condition('id', $id)
        ->execute()->fetchObject();
       
      static::$cache[$id] = $obj;
      if ($obj) {
        $obj->data = unserialize($obj->data);
      }
    }
    
    return static::$cache[$id];
    
  }
  
  /**
   * Fetches the most the version of the object that should
   * be used. The stash will prefer the stashed versions
   * if available. If needed, use the $isStashed to tell
   * if the object was loaded.
   * 
   * @param object $obj
   *  The stashed object, to check for a stashed version.
   * @param bool $isStashed
   *  Reference to a boolean of the status. Will be set
   *  by this function depending on if the item was loaded.
   * @return object
   *  The stashed object will be returned.
   */
  function fetch($obj, &$isStashed = NULL) {
    $stashed = $this->load($obj);
    $isStashed = (bool)$stashed;
 
    // If stashed, return the stashed item, otherwise
    // return the original item as the most current.
    return $isStashed ? $stashed->data : $obj;
  }
  
  /**
   * Store the build object to the database table. 
   * 
   * @param object $obj
   *  Serializable object used to store the data.
   */
  function save($obj) {
    global $user;
    
    $keys = array(
      'type' => $this->getType(),
      'id' => $this->extractId($obj),
    );
    
    $data = array(
      'uid' => $user->uid,
      'changed' => REQUEST_TIME,
      'data' => serialize($obj),
    );
    
    // Stash the object for later usage.
    db_merge('iwapi_build_stash')
      ->key($keys)
      ->fields($keys + $data)
      ->execute();
    
    $id = $this->extractId($obj);
    static::resetCache($id);
  }
  
  /**
   * Taking either the ID or the object,
   * delete the item indicated.
   * 
   * @param object|string $obj
   *  Object or the id to indicate what
   *  stash to delete.
   */
  function delete($obj) {
    $id = is_string($obj) ? $obj : $this->extractId($obj);

    db_delete('iwapi_build_stash')
      ->condition('type', $this->getType())
      ->condition('id', $id)
      ->execute();
    
    $id = $this->extractId($obj);
    static::resetCache($id);
  }
  
  /**
   * Clear the entire stash for this type of object.
   */
  function clear() {
    db_delete('iwapi_build_stash')
      ->condition('type', $this->getType())
      ->execute();
    
    static::resetCache();
  }
}
