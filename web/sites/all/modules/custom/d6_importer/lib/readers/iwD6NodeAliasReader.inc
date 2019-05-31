<?php
/**
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


class iwD6NodeAliasReader extends iwDbReader {

  // Node fields that should transfer directly into the new content.
  static protected $NodeKeys = array(
    'nid', 'vid', 'title', 'type', 'status',
    'created', 'changed', 'promote', 'sticky',
  );
  
  /**
   * (non-PHPDOC)
   * @see iwIPackage::getPackageProps().
   */
  public static function getPackageProps() {
    return array(
      'type' => array('label' => t('Source Node type')),
      'conditions' => array('label' => t('Filter Criteria')),
    ) + parent::getPackageProps();
  }
  
  
  // --------------------------
  // Instance fields
  // --------------------------

  public $name = 'default';

  protected $id;
  protected $type;
  
  public $conditions = array();


  // --------------------------
  // Instance methods
  // --------------------------

  public function __construct(array $data = array()) {
    parent::__construct($data);

    $this->types      = $data['types'];
    $this->idKey      = 'orig_nid';
    $this->conditions = $data['conditions'];
  }

  /**
   * Internal function used to determine the total
   * number of nodes that will get imported.
   * 
   * @return int
   *  Number of items found in the database.
   */
  protected function getTotal() {
    // Get the count information for the data about to get loaded.
    $count_query = $this->getConnection()->select('node', 'n')
      ->condition('type', $this->types);
    
    // Alter the query to include the current node conditions.
    $this->applyConditions($count_query);
    
    $count_query->addExpression('COUNT(*)', 'count');
    return $count_query->execute()->fetchField();
  }
  
  /**
   * Apply conditions linked to / related to this
   * 
   * @param QueryConditionInterface $query
   *  Add conditions to a query in order to restrict
   *  the nodes that are being returned.
   */
  protected function applyConditions(QueryConditionInterface $query) {
    foreach ($this->conditions as $cond) {
      if ($cond instanceof DatabaseCondition) {
        $query->condition($cond);
      }
      elseif (is_array($cond)) {
        $cond += array('op' => '=');
        $query->condition("n.$cond[field]", $cond['value'], $cond['op']);
      }
    }
  }
 
  /**
   * (non-PHPdoc)
   * @see iwIReader::getKeys()
   */
  function getKeys() {
    return array(
      'orig_nid' => 'orig_nid',
      'alias' => 'alias',
    );
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
  function bufferData($bufferSize = 25) {
    $this->buffer = array();
    if (!isset($this->query)) {
      $this->query = $this->getConnection()->select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('n.type', $this->types)
        ->range(0, $bufferSize)
        ->orderby('n.nid');
      
      // Apply search conditions to filter nodes.
      $this->applyConditions($this->query);
      
      // Add revision information / data
      $this->query->innerJoin('url_alias', 'ua', 'CONCAT(:prefix, n.nid)=ua.src', array(':prefix' => 'node/'));
      $this->query->addField('ua', 'dst');
    }

    // Pull items from where we left off.
    $query = clone $this->query;
    $rs = $query->condition('n.nid', $this->id, '>')->execute();
    
    // Iterate through the items we've found and add them to the buffer.
    $keys = array_flip(static::$NodeKeys);
    while ($obj = $rs->fetchAssoc()) {
      $obj['orig_nid'] = $obj['nid'];
      $obj['nid'] = db_select('d6_importer_lookup', 'l')
        ->fields('l', array('id'))
        ->condition('data_type', 'node')
        ->condition('old', $obj['nid'])
        ->execute()->fetchField();
      
      $this->buffer[$obj['orig_nid']] = $obj;
    }
  }
}
