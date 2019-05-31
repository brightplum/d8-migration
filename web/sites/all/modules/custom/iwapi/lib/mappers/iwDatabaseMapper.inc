<?php
/**
 * @file iwDBMapper.inc
 * Define the database mapper type which handles data from a database.
 */

/**
 * Generalized database mapper, will find matching input items
 *  and match them to whatever the results field was. Will check
 *  constraints and run them through the list of processes.
 */
class iwDatabaseMapper extends iwMapper {

  protected $values = array();  // Maintain already mapped data, in case of referencing.
  
  protected $table;             // the table source
  protected $dbCol;             // Field to pull from the database. 
  
  function __construct(array $def) {
    parent::__construct($def);
 
    $this->table   = $def['table'];
    
    // If empty default to using the same field name or column as
    // the result field. Otherwise, set it to what came from the definition.
    $this->dbCol = empty($def['dbColumn']) ? $this->result : $def['dbColumn']; 
  }
  
  /**
   * Translate raw inputs into the resulting data type.
   * 
   * @see iwMapper::translate()
   */
  function translate(array $raw) {    
    $alias = 'base';
    $query = db_select($this->table, $alias);
    $query->addField($alias, $this->dbCol);
    
    $val_str = '';
    $op  = NULL;
    foreach ($this->inputs as $src => $handler) {
      if ($handler instanceof iwMapper) {
        $val = $handler->translate($raw);
        $key = $handler->getResultKey();
      }
      else {
        $val = $raw[$src];
        if (is_array($handler)) {
          $key = $handler['handler'];
          $op = empty($handler['op']) ? NULL : $handler['op'];
        }
        else {
          $key = $handler;
        }
      }
      
      $query->condition("$alias.$key", $val, $op);
      $val_str .= '&'. $val;
    }
    
    $val_str = strlen($val_str) > 32 ? md5($val_str) : $val_str;
    if (!isset($this->values[$val_str])) {
      $this->values[$val_str] = $query->execute()->fetchField();
    }
    
    $this->values[$val_str] = $this->values[$val_str] === FALSE ? NULL : $this->values[$val_str];
    return $this->values[$val_str];
  }
  
  /**
   * (non-PHPdoc)
   * @see iwMapper::packData()
   */
  function packData() {
    $data = parent::packData();
    $data['table'] = $this->table;
    
    // If using a different database column
    // from the results fiel, record that too.
    if ($this->dbCol !== $this->result) {
      $data['dbColumn'] = $this->dbCol;
    }
    
    return $data;
  }
}
