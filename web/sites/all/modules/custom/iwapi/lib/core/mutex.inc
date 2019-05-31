<?php
/**
 * @file lib/core/mutex.inc
 * Contains IWAPI re-entrant locking functionality
 * 
 * Drupal locking does not provide full locking functionality, since the
 * checking and acquiring is not an atomic operation. System V contains
 * OS level semaphores, but this is not always available. The iwMutex class
 * and sub-classes provide a uniform way of implementing locks depending
 * on different PHP extensions and libraries.
 * 
 * @author Liem Khuu <liem@imagewks.com>
 */

/*******************************************************************
 * RECOMMENDATIONS:
 *  - Single Computer server
 * iwFlockMutex - is most available and only valid for single computer setup.
 * iwAPCMutex   - is probably best mix of compatibility and performance.
 * 
 *  - Network / Server cluster
 * iwMemcacheMutex - If the cluster uses a single Memcache server for all locks
 *   a cluster can manage mutually exclusive tasks through Memcache.
 *   
 ******************************************************************/

/**
 * Base mutex class that also serves as the factory
 * for creating Mutex instances.
 * 
 * The mutexes derived from this class should be
 * re-entrant and properly release their lock.
 */
abstract class iwMutex {
  
  /**
   * @var string
   *  Name ID of the lock to this instance will use
   *  when acquiring and releasing the lock it owns.
   */
  protected $name;
  
  /**
   * @var <mixed>
   *  Any lock additional information iff a lock is currently held.
   *  Should be unset() or NULL if a lock is not currently active.
   */
  protected $lock;
  
  /**
   * Static call to get the current process id for this
   * PHP thread. The value should be consistent accross 
   * all mutex subclass variations, within a process.
   * 
   * @return string
   *  Return the process ID which should be unique for this
   *  thread. This only gets assigned once per PHP request.
   */
  protected function getPid() {
    static $pid;

    // Initialize the unique thread ID to use for all
    // mutex instances to aide with re-entrant locking.
    if (!isset($pid)) {
      $pid = uniqid(mt_rand(), TRUE);
    }
    return $pid;
  }
  
  /**
   * Factory function to generate mutexes. This call should be
   * used, so that the correct mutex strategy is used. The
   * strategy is determined by Drupal settings and extensions
   * installed on the server.
   * 
   * @param $name
   *  Name of the lock to associate with the mutex instance.
   */
  static public function CreateMutex($name) {    
    // The default class to use for locking is flock() since it
    // is available on just about any system. If you want to use
    // database method, use the standard Drupal locking system.
    $_class = variable_get(IWAPI_VAR_MUTEX_TYPE, 'iwFlockMutex');
    
    // Check to ensure that object type is a valid iwMutex subclass.
    if (class_exists($_class) && is_subclass_of($_class, __CLASS__)) {
      return new $_class($name);
    }

    throw new Exception(t('Invalid Mutex type: @class cannot be constructed.', array('@class' => $_class)));
  }
  
  /**
   * Initialize the Mutex name and lock status.
   * 
   * @param $name
   *  This is the named ID for the mutex designated
   *  to this instance. This identifies the name.
   */
  protected function __construct($name) {
    $this->name = $name;
    $this->lock = FALSE;
  }
  
  /**
   * Ensure that freeing this object will release
   * the associated lock if one has been captured.
   */
  function __destruct() {
    $this->release();
  }
  
  /**
   * Return the name of the lock ID for this instance.
   * 
   * @return string
   *  String id / name of the lock.
   */
  function getName() {
    return $this->name;
  }
  
  /**
   * Check if this instance of the Mutex has
   * access to the lock specified by $this->name.
   * 
   * @return boolean
   *  TRUE if this instance currently has
   *  control of this lock. FALSE otherwise.
   */
  function isAcquired() {
    return $this->lock;
  }
  
  /**
   * Attempt to acquire the lock if it is free or
   * spinlock until the lock is available.
   * 
   * @param float $waitSec
   *  Number of seconds to wait for the mutex to become 
   *  available. Anything zero or less equates an
   *  iwMutex::acquire() call with more overhead.
   * @return bool
   *  TRUE iff the lock was captured, FALSE if the wait
   *  timer expired before being able to capture the lock.
   */
  function waitAndAcquire($waitSec = 10.0 /* 10 seconds */) {
    $wait = (int)($waitSec * 1000000); // convert seconds to micro-seconds.
    $time = 0; $delay = 5000;          // initialize loop variables
    
    // Spinlock with each delay grows by a factor of 2 until
    // the maximum delay time is reach 0.75 seconds. After
    // which, we wait 0.75 seconds on successive iterations
    while ($wait > $time) {
      if($this->acquire()) {
        return TRUE;
      }
      
      usleep($delay);  // Sleep this thread
      $time += $delay; // adjust our current time tracking.
      $delay = min($delay << 1, 750000, $wait - $time);
    }
    return FALSE;
  }
  
  /**
   * Attempt to capture the lock with the
   * name ID of this instance.
   * 
   * @return boolean
   *  TRUE if the lock was succesfully captured.
   */
  abstract function acquire();
  
  /**
   * If this instance is holding a lock, check if
   * it should be released. Since multiple mutex object
   * can hold the same lock
   */
  abstract function release();
}


/**
 * Use flock() to maintain file locks. Though this function
 * is more available and ships with most PHP package, the
 * drawback is that flock() can be inconsistent on some OS.
 * 
 * Windows does not support non-blocking lock checking. It is
 * also the slowest of all the methods. Good for use on develop
 * instances, and some testing (or file writes).
 */
class iwFlockMutex extends iwMutex {
  
  const FLOCK_PREFIX = '/flock-mutex-';
  
  /**
   * @var array
   *  Tracks all the currently acquired locks, keyed
   *  by the file name, used to obtain the file pointer.
   */
  static protected $Files = array();
  
  /**
   * (non-PHPdoc)
   * @see iwMutex::acquire()
   */
  function acquire() {
    $name = file_directory_temp() . self::FLOCK_PREFIX . $this->name;
    
    // If we already have this lock, just return it.
    if ($this->lock) {
      return TRUE;
    }
 
    // Create a tracking entry for this file. With this 
    // method we need to store a file pointer and use counter.
    if (!isset(self::$Files[$name])) {
      self::$Files[$name] = array();
    }
    $entry = &self::$Files[$name];
    
    // Determine the current status of the file lock, and update
    // the status to reflect if we can obtain the lock or not.
    if (!empty($entry['fp'])) {
      $this->lock = TRUE;
      ++$entry['count'];
    }
    elseif (($entry['fp'] = fopen($name, 'c')) !== FALSE) {
      // It is not until here that we try to capture!
      $this->lock = flock($entry['fp'], LOCK_EX | LOCK_NB);
      
      // IFF success, update the mutex status and increment the
      // count tracking on failure, release the file pointer.
      if ($this->lock) {
        $this->lock = TRUE;
        $entry['count'] = 1;
      }
      else {
        fclose($entry['fp']);
        unset($entry['fp']);
      }
    }
    return $this->lock;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwMutex::release()
   */
  function release() {
    $name = file_directory_temp() . self::FLOCK_PREFIX . $this->name;

    $entry = array();
    if (isset(self::$Files[$name])) {
      $entry = &self::$Files[$name];
    }

    // Check if we truly have this lock, only release if
    // all instances have closed this lock.
    if ($this->lock && isset($entry['fp']) && $entry['fp'] !== FALSE) {
      $this->lock = FALSE;
      
      if (--$entry['count'] <= 0) {
        flock($entry['fp'], LOCK_UN);
        fclose($entry['fp']);
        unset(self::$Files[$name], $entry);
      }
    }
  }
}


/**
 * Mutex which uses the APC cache as its locking
 * strategy. Most APC builds use pthreads to lock
 * writing to the cache, meaning it is likely fast
 * and handles locking correctly.
 * 
 * What makes it reliable is that the apc_add()
 * function will only set the value if APC is not
 * already caching the value. Thus theoretically
 * the check and the lock happen atomicly.
 */
class iwApcMutex extends iwMutex {
  
  /**
   * @var array
   *  Maintains the number of instances that have this
   *  lock a name ID held. This is mainly to count
   *  re-entrant locks so we release only when the last
   *  instances has released its hold.
   */
  private static $Counter = array();
  
  /**
   * (non-PHPdoc)
   * @see iwMutex::acquire()
   */
  function acquire() {
    $pid  = $this->getPid();
    $ttl  = ini_get('max_execution_time'); // Fallback to expire when the lock does.

    // Prefix the name to avoid collisions with
    // other normal APC cached variables.
    $cacheName = 'iwMutex:' . $this->name;
    
    if (!$this->lock) {
      // apc_add() will only set the APC value iff name is not currently used. We 
      // use the process ID as the value in APC. If assigning fails, we check if
      // this lock has the same id, which means we can re-capture this lock.
      $this->lock = apc_add($cacheName, $pid, $ttl) || apc_fetch($cacheName) === $pid;
      
      // If newly obtained, adjust the counts of the iwApcMutex instances
      // that hold this lock. This is possible because locks are re-entrant.
      if ($this->lock) {
        if (!isset(self::$Counter[$this->name])) {
          self::$Counter[$this->name] = 0;
        }
        ++self::$Counter[$this->name];
      }
    }
    return $this->lock; // Return whether this instance is holding a lock.
  }
  
  /**
   * IFF this instance was holding a lock, update the number of 
   * instances holding that lock. If this is the last object of
   * this <em>type</em> which holds the lock, release the underlying lock.
   */
  function release() {
    $cacheName = 'iwMutex:' . $this->name;
    if ($this->lock && apc_fetch($cacheName) === $this->getPid()) {
      $this->lock = FALSE;
      
      // Only when all instances have dropped this lock do we really release.
      if (--self::$Counter[$this->name] <= 0) {
        apc_delete($cacheName);
      }
    }
  }
}

/**
 * Use memcache to manage locks. Slower than APC, but should
 * be the recommended method when using clusters, if locking
 * is delegated to a single Memcache server. APC or flock only
 * have locks that scope within a single machine.
 */
class iwMemcacheMutex extends iwMutex {

  /**
   * @var array
   *  Maintains the number of instances that have this
   *  lock a name ID held. This is mainly to count
   *  re-entrant locks so we release only when the last
   *  instances has released its hold.
   */
  private static $Counter = array();
  
  /**
   * @var array
   *  Connection information used to connect to the
   *  memcache server which manages this lock.
   */
  private static $ConnInfo;
  
  /**
   * @var Memcache
   * Memcache object to be shared by all instances of the
   * class. Since all mutex calls need to shared the same
   * server, and connection, one instance (connection) is
   * valid for all mutex calls using Memcache.
   */
  static protected $Cache;
  
  /**
   * If the Memcache server is not local and on the default
   * port, that information needs to be set
   * 
   * @param array $info
   *  
   */
  static function SetConnInfo(array $info) {
    if (isset(self::$Cache)) {
      foreach (self::$Counter as $count) {
        if ($count > 0) {
          throw new Exception('Server cannot be changed while iwMemcacheMutex instances has open locks.');
        }
      }
      
      // No open locks, it is safe to change server settings
      self::$Counter = array();
      self::$Cache->close();
      unset($self::$Cache);
    }
    
    $info += array('host' => 'localhost', 'port' => '11211');

    // Verify that the server settings are valid.
    if (preg_match('#^([a-z]{2,5}://)?[\w\-_.]+$#i', $info['host']) && is_numeric($info['port'])) {
      self::$ConnInfo = array('host' => $info['host'], 'port' => $info['port']);
      
      if (isset(self::$Cache)) {
        self::$Cache->close();
        unset(self::$Cache);

        self::$Cache = new Memcache();
        self::$Cache->connect(self::$ConnInfo['host'], self::$ConnInfo['port']);
      }
    }
  }
  
  /**
   * Initialize a Memcache instance if one is required.
   * 
   * @param string $name
   *  Name ID of the lock this Mutex will try to hold.
   */
  function __construct($name) {
    // call the parent constructor.
    parent::__construct($name);
    
    if (!isset(static::$Cache)) {
      self::$Cache  = new Memcache();
      $hostMemcache = variable_get(IWAPI_VAR_MEMCACHE_LOCATION, array());
      self::SetConnInfo($hostMemcache);
      self::$Cache->connect(self::$ConnInfo['host'], self::$ConnInfo['port']);
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwMutex::acquire()
   */
  function acquire() {
    $pid        = $this->getPid();
    $cacheName  = 'iwMutex:' . $this->name;
    $ttl        = ini_get('max_execution_time'); // Fallback to expire when the lock does.
    
    if (!$this->lock) {
      // apc_add() will only set the APC value iff name is not currently used. We 
      // use the process ID as the value in APC. If assigning fails, we check if
      // this lock has the same id, which means we can re-capture this lock.
      $this->lock = self::$Cache->add($cacheName, $pid, 0, $ttl) || self::$Cache->get($cacheName) === $pid;
      
      // If newly obtained, adjust the counts of the iwApcMutex instances
      // that hold this lock. This is possible because locks are re-entrant.
      if ($this->lock) {
        if (!isset(self::$Counter[$this->name])) {
          self::$Counter[$this->name] = 0;
        }
        ++self::$Counter[$this->name];
      }
    }
    return $this->lock; // Return whether this instance is holding a lock.
  }
  
  /**
   * IFF this instance was holding a lock, update the number of 
   * instances holding that lock. If this is the last object of
   * this <em>type</em> which holds the lock, release the underlying lock.
   */
  function release() {
    $cacheName = 'iwMutex:' . $this->name;
    if ($this->lock && self::$Cache->get($cacheName) === $this->getPid()) {
      $this->lock = FALSE;
      
      // Only when all instances have dropped this lock do we really release.
      if (--self::$Counter[$this->name] <= 0) {
        self::$Cache->delete($cacheName);
      }
    }
  }
}

/* ---------------------------------------------
 * TODO: Eventually create a mutex which uses the System V
 * Semaphore functionality if it exists on a server. I believe
 * that APC performance is possibly similar here, but I have
 * yet to implement / profile the calls.
 * 
 * If performance with locking is similar, this implementation
 * is not likely to be needed.
 * 
 * My concern here is that I cannot rely on this with many
 * hosting providers. Doesn't seem commonly supported.
 * 
class iwSemaphoreMutex extends iwMutex {
  
}
*/
