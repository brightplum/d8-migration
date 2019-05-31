<?php
/**
 * @file
 * Pull information form database rows.
 */

/**
 * Read data from a database.
 */
abstract class iwDbReader extends iwReader {
  
  // Node fields that should transfer directly into the new content.
  static protected $NodeKeys = array(
    'nid', 'vid', 'title', 'type', 'status',
    'created', 'changed', 'promote', 'sticky',
  );
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::checkFormat()
   */
  public static function checkFormat($src) {
    foreach (array('driver', 'database', 'username', 'password') as $key) {
      if (empty($src[$key])) {
        return FALSE;
      }
    }

    return TRUE;
  }
  
  
  // --------------------------
  // Instance fields
  // --------------------------
  
  public $name = 'default';
  
  protected $id;
  
  /**
   * (non-PHPDOC)
   * @see iwIReader::setSource()
   */
  public function setSource($src) {
    $this->id     = 0;
    $this->count  = 0;
    $this->buffer = array();
    
    $src += array('host' => 'localhost');
    Database::addConnectionInfo('iw_import', $this->name, $src);
    
    $this->total = $this->getTotal();
  }
  
  /**
   * Get the total number of expected items
   * found by this query.
   * 
   * @return int
   *  Number of total items to return.
   */
  abstract protected function getTotal();
  
  /**
   * (non-PHPDOC)
   * @see iwIReader::getProgress().
   */
  public function getProgress() {
    return array(
      'id'    => $this->id,
      'count' => $this->count,
    );
  }
  
  /**
   * Report the percentage that we've completed
   * reading this source file. Makes it a little
   * friendlier than showing a constant number. 
   * 
   * @return float
   *  Returns the percentage that this batch
   *  is completed.
   */
  function percentComplete() {
    return $this->count / $this->total; 
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::resumeProgress()
   */
  function resumeProgress(array $progress) {
    $this->id    = $progress['id'];
    $this->count = $progress['count'];
    
    $this->bufferData();
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::rewind()
   */
  function rewind() {
    $this->id     = 0;
    $this->count  = 0;
    
    $this->bufferData();
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::next()
   */
  function next() {
    array_shift($this->buffer);
    
    if (empty($this->buffer)) {
      $this->bufferData();
    }
    
    ++$this->count;
    $cur = reset($this->buffer);
    $this->id = !empty($cur) ? $cur[$this->idKey] : NULL; 
    return $cur;
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::key()
   */
  function key() {
    return $this->id;
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::current()
   */
  function current() {
    return reset($this->buffer);
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::valid()
   */
  function valid() {
    return (bool)reset($this->buffer);
  }
 
  
  // ========================
  // Special query handling
  // ========================
  
  /**
   * Make a query and buffer the data.
   * 
   * @param type $bufferSize
   *  Size of the data to buffer from the
   *  source database.
   */
  abstract function bufferData($bufferSize = 25);
  
  /**
   * Fetch the database connection being used by this
   * database reader. Will allow fields to get data
   * from the same source database.
   */
  function getConnection() {
    return Database::getConnection($this->name, 'iw_import');
  }
}
