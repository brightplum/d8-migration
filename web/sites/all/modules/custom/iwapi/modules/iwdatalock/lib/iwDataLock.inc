<?php
/**
 * @file lib/iwDataLock.inc
 * Content locking objects and features. Mirrors a lot of the mutex implementation.
 * 
 * Offers content locking implementations, which allow you to "checkout" some
 * content and inform other users who try to access this same data, that the
 * content is locked.
 * 
 * Though similar to mutexes, there are 3 major differences:
 * 1.) Author orientied as opposed to thread based access (re-entrant).
 * 2.) The requirement to report expirations and no wait based calls.
 * 3.) Allow refreshing or breaking of locks. 
 * 
 * @author Liem Khuu <liem@imagewks.com>
 */


/**
 * Base implementation of a content lock. Also is used
 * as the lock object factory
 */
abstract class iwDataLock {
  
  /**
   * Factory function to generate the content lock type,
   * which is specified in the locking configurations.
   * 
   * @param string $entityType
   *  The name of the entity type which is being locked.
   * @param int $entityId
   *  The Id of the entity from the base table.
   * @param int $ttl
   *  The amount of time in seconds before we expire this content lock.
   *  Defaults to a half a hour.
   */
  static function CreateLock($entityType, $entityId, $ttl = 1800) {
    global $user;
    
    // Locks will only be created for the current user - no one can
    // create a lock on another user's behalf.
    $_class = variable_get(IWDATALOCK_VAR_LOCK_TYPE, 'iwDbDataLock');
    if (class_exists($_class) && is_subclass_of($_class, __CLASS__)) {
      return new $_class($entityType, $entityId, $ttl, $user);
    }
    
    // Report that the system could not create this lock.
    throw new Exception(t(
      'Content lock type %class is unavailable or not a subclass of iwContentLock.',
      array('%class' => $_class)
    ));
  }
  
  // Information about the lock.
  protected $type;
  protected $id;
  protected $uid;
  protected $ttl;
  
  // Reports if the lock has been acquired previously.
  protected $locked = FALSE;

  /**
   * Protected constructor meant to be called by
   * iwContentLock::CreateLock(), to populate
   * instance properties.
   * 
   * @param string $entityType
   *  The type of entity that this lock is being constructed for.
   * @param int $entityId
   *  The unique ID used to identify the content to protect.
   * @param string $ttl
   *  The time in seconds that this lock should last for.
   * @param stdClass $account
   *  User account to for which to obtain this lock for.
   *  
   * @see iwContentLock::CreateLock().
   */
  protected function __construct($entityType, $entityId, $ttl, stdClass $account) {
    $this->type = $entityType;
    $this->id   = $entityId;
    $this->uid  = $account->uid;
    
    // Ensure that TTL is at least 1 minute. Anything less will
    // likely be meaningless and cause more issues than it'll help.
    $this->ttl  = $ttl > 60 ? $ttl : 60;
    
    if (empty($this->type) || empty($this->id)) {
      throw new InvalidArgumentException('Missing entity type or identifier.');
    }
  }
  
  /**
   * The heart of locking functionality. Acquire is the
   * combination of checking and capturing the lock in
   * an "atomic" operation.
   * 
   * This way race conditions don't exist with checking 
   * for a lock and capturing it.
   * 
   * Locks are re-entrant so requesting a lock you already
   * own results in success!
   */
  abstract function acquire();
  
  /**
   * In order to report the status of a lock we
   * need to know what time might be remaining and
   * the current owner of the lock, if relevant.
   */
  abstract function stat();
  
  /**
   * If the lock is acquired, attempt to refresh
   * the lock for another TTL interval. If you 
   * do not own the lock, this request should
   * fail.
   */
  abstract function refresh();
  
  /**
   * Release a lock that has already been captured.
   * Usually, you can only release a lock that
   * you own but, setting $force will cause
   */
  abstract function release($force = FALSE);

}

/**
 * Content locking implementation which has status
 * information in a database table.
 */
class iwDbDataLock extends iwDataLock {
  
  const Table = 'iwdatalock_locks';
  
  /**
   * (non-PHPdoc)
   * @see iwContentLock::acquire()
   */
  function acquire() {
    if ($this->locked) { // already captured.
      return TRUE;
    }
    
    // Row level mutex, should be separate from DB implementation.
    $mutex_id = "DataLock={$this->type}:{$this->id}";
    $mutex = iwMutex::CreateMutex($mutex_id);
    
    // Ensure that no one else is trying to obtain or write this lock.
    if ($mutex->waitAndAcquire()) {
      try {
        $query = 'SELECT uid, expire FROM {'. static::Table .'} ';
        $query .= 'WHERE entity_type=:type AND entity_id=:id';
      
        // Check if a lock previously existed, and what it's current state is.
        $lock = db_query($query, array(':type' => $this->type, ':id' => $this->id))->fetchObject();
        
        // These are the conditions that grant us access to the lock.
        if (!$lock || $lock->expire < time() + 5 /* Add 5 sec buffer */) {
          if ($lock) {
            // update only, make sure owner and expiration are correct.
            db_update(static::Table)
              ->fields(array('uid' => $this->uid, 'expire' => time() + $this->ttl))
              ->condition('entity_type', $this->type)
              ->condition('entity_id', $this->id)
              ->execute();
          }
          else {
            $data = array(
              'entity_type' => $this->type,
              'entity_id'   => $this->id,
              'uid'         => $this->uid,
              'expire'      => time() + $this->ttl,
            );
            
            // Insert a new lock entry.
            db_insert(static::Table)
              ->fields($data)
              ->execute();
          }
          $this->locked = TRUE;    // Update that we've capture this lock.
        }
        elseif ($lock->uid == $this->uid) {
          $this->locked = TRUE;  // Re-entrant, we have access, but shouldn't recreate.
        }

        $mutex->release();    // Free the mutex.
        return $this->locked; // Return whether or not we have this lock.
      }
      catch (Exception $e) {
        // Make sure that any exception will not miss the call to release the mutex.
        // PHP version 5.5 implements 'finally' keyword... too bad it's too late.
        $mutex->release();
        throw $e;
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwContentLock::stat()
   */
  function stat() {
    // ignore mutex locking for this since outdated reads will
    // not break or harm anything. Any other method from this
    // class can call it, but only within the context of a mutex;
    $lock = db_select(static::Table, 'datalock')
      ->fields('datalock')
      ->condition('entity_type', $this->type)
      ->condition('entity_id', $this->id)
      ->execute()->fetchObject();
    
    // Expiration is the only thing we validate when reporting the status.
    if (!$lock || $lock->expire <= time() + 5) {
      return FALSE;
    }
    
    $lock->remaining = $lock->expire - time();
    return $lock;  // Data still valid, return it.
  }
  
  /**
   * (non-PHPdoc)
   * @see iwContentLock::refresh()
   */
  function refresh() {
    // Row level mutex, should be separate from DB implementation.
    $mutex_id = "DataLock={$this->type}:{$this->id}";
    $mutex = iwMutex::CreateMutex($mutex_id);
    
    // Ensure that no one else is trying to obtain or write this lock.
    if ($mutex->waitAndAcquire()) {
      try {
        // update only, make sure owner and expiration are correct.
        $count = db_update(static::Table)
          ->fields(array('expire' => time() + $this->ttl))
          ->condition('entity_type', $this->type)
          ->condition('entity_id', $this->id)
          ->condition('uid', $this->uid)  // Important, can't update if we don't own this lock!
          ->execute();
        
        // Ensure that the mutex is correctly released.
        $mutex->release();
        
        // --------------------------------------------
        // If nothing was updated, try to obtain. It either didn't
        // exist already, or owned by someone else. This means we need
        // to create a lock with the new expiration.
        //
        // This can only happen if, no lock exists, or it has expired.
        // Both situations are correctly handled by $this->acquire().
        if ($count > 0) {
          $mutex->release();
          return TRUE;
        }
        else {
          $retval = $this->acquire();
          $mutex->release();
          return $retval;
        }
      }
      catch (Exception $e) {
        $mutex->release();
        throw $e;
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwContentLock::release()
   */
  function release($force = FALSE) {
    // Row level mutex, should be separate from DB implementation.
    $mutex_id = "DataLock={$this->type}:{$this->id}";
    $mutex = iwMutex::CreateMutex($mutex_id);
    
    // Ensure that no one else is trying to obtain or write this lock.
    if ($mutex->waitAndAcquire()) {
      try {
        if (!$this->locked) {
          $lock = $this->stat();
          $this->locked = ($lock && $lock->uid == $this->uid);
        }
        
        // attempt to release this lock
        if ($this->locked || $force) {
          db_delete(static::Table)
            ->condition('entity_type', $this->type)
            ->condition('entity_id', $this->id)
            ->execute();
            
          // We no longer have this lock.
          $this->locked = FALSE;
        }
    
      }
      catch (Exception $e) {
        $mutex->release();
        throw $e;
      }
      
      // Ensure that the mutex is correctly released.
      $mutex->release();
    }
  }
}

// TODO: Implement these additional lock methods.
/**
 *
 *
 */
/*
class iwApcContentLock extends iwContentLock {
  
}
//*/

/**
 * 
 *
 */
/*
class iwMemcacheContentLock extends iwContentLock {
  
}
//*/