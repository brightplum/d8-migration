<?php
/**
 * @file
 * Class for reading and translating file information
 * from the database.
 */


class iwD6FileReader extends iwDbReader {

  // Node fields that should transfer directly into the new content.
  static protected $FileKeys = array(
    'fid', 'filename', 'filepath', 'filemime',
    'filesize', 'status', 'timestamp',
  );

  
  // --------------------------
  // Instance fields
  // --------------------------
  
  public $name = 'default';
  
  protected $id;
  protected $type;

  
  // --------------------------
  // Instance methods
  // --------------------------
  
  public function __construct(array $data = array()) {
    parent::__construct($data);
    
    $this->idKey = 'orig_fid';
  }
  
  /**
   * Get the total number of active files available.
   * 
   * @return int
   *  The number of items that we should import.
   */
  protected function getTotal() { 
    // Get the count information for the data about to get loaded.
    $count_query = $this->getConnection()->select('files', 'f')
      ->condition('status', 0, '<>');
    
    $count_query->addExpression('COUNT(*)', 'count');
    return $count_query->execute()->fetchField();
  }
  
 
  /**
   * (non-PHPdoc)
   * @see iwIReader::getKeys()
   */
  function getKeys() {
    $keys = static::$FileKeysKeys;
    $keys[] = 'orig_fid'; // Include reference to the old fid.
    
    return $keys;
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
  function bufferData($bufferSize = 50) {
    $this->buffer = array();
    
    if (!isset($this->query)) {        
      $this->query = $this->getConnection()->select('files', 'f')
        ->fields('f', static::$FileKeys)
        ->condition('status', 1)
        ->range(0, $bufferSize)
        ->orderby('f.fid');
    }

    // Pull items from where we left off.
    $query = clone $this->query;
    $rs = $query->condition('f.fid', $this->id, '>')->execute();

    // Iterate through the items we've found and add them to the buffer.
    while ($obj = $rs->fetchAssoc()) {
      $obj['orig_fid'] = $obj['fid'];
      
      unset($obj['fid']);
      $this->buffer[$obj['orig_fid']] = $obj;
    }
  }
}
