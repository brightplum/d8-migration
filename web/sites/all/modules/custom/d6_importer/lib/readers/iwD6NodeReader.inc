<?php
/**
 * @file
 * Contains definition of the database reader
 * which
 */

class iwD6NodeReader extends iwDbReader {

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

    $this->type       = $data['type'];
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
      ->condition('type', $this->type);
    
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
    if (!isset($this->fields)) {
      $this->buildFields();
    }
    
    $keys = array_keys($this->fields + array_fill_keys(static::$NodeKeys, TRUE));
    array_unshift($keys, 'orig_nid');
    
    return $keys;
  }
  
    
  // ========================
  // Special query handling
  // ========================
  
  /**
   * Query the database and get the field properties from
   * the source database.
   */
  function buildFields() {
    if (!isset($this->fields)) {
      $this->fields = array('body' => new d6BodyFieldReader());
      
      // Determine which fields are used with the database.
      $query = $this->getConnection()
        ->select('content_node_field_instance', 'fi')
        ->condition('type_name', $this->type);

      $query->join('content_node_field', 'f', 'fi.field_name=f.field_name');
      $query->fields('fi', array('field_name', 'widget_settings'))
        ->fields('f', array('type', 'global_settings', 'db_storage', 'multiple'));

      foreach ($query->execute() as $field) {
        $field->settings = unserialize($field->global_settings);
        $field->instance = unserialize($field->widget_settings);
        unset($field->global_settings, $field->widget_settings);

        $this->fields[$field->field_name] = d6FieldReader::CreateFieldReader($this, $field, $field->instance);    
      }
      
      $taxonomy = $this->getConnection()
        ->select('vocabulary_node_types', 'vnt')
        ->fields('vnt', array('vid'))
        ->condition('type', $this->type)
        ->execute()->fetchCol();
      
      if (!empty($taxonomy)) {
        $tax_query = db_select('d6_importer_lookup', 'lookup')
          ->condition('old', $taxonomy)
          ->condition('data_type', 'vocabulary');
        
        $tax_query->addField('lookup', 'old', 'old_vid');
        
        $tax_query->innerJoin('taxonomy_vocabulary', 'vocab', 'lookup.id=vocab.vid');
        $tax_query->fields('vocab', array('vid', 'machine_name'));
        
        foreach ($tax_query->execute() as $vocab) {
          $this->fields['term_'. $vocab->machine_name] = new d6TermFieldReader($this, (array)$vocab);
        }
      }

      // Clear empty fields.
      $this->fields = array_filter($this->fields);
    }
  }
  
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
        ->fields('n', static::$NodeKeys)
        ->condition('type', $this->type)
        ->range(0, $bufferSize)
        ->orderby('n.nid');
      
      // Apply search conditions to filter nodes.
      $this->applyConditions($this->query);
      
      // Add revision information / data
      $this->query->innerJoin('node_revisions', 'rev', 'n.vid=rev.vid');
      $this->query->fields('rev', array('body', 'teaser'));
      
      $this->buildFields();
      $field_table = 'content_type_'. $this->type;
      if ($this->getConnection()->schema()->tableExists($field_table)) {
        $this->query->leftJoin($field_table, 'content_type', 'content_type.vid=n.vid');
        $this->query->fields('content_type');
      }
    }

    // Pull items from where we left off.
    $query = clone $this->query;
    $rs = $query->condition('n.nid', $this->id, '>')->execute();
    
    // Iterate through the items we've found and add them to the buffer.
    $keys = array_flip(static::$NodeKeys);
    while ($obj = $rs->fetchAssoc()) {
      $node = array_intersect_key($obj, $keys);
      $node['orig_nid'] = $obj['nid'];
      
      foreach ($this->fields as $name => $fieldReader) {
        $node[$name] = $fieldReader->getValues($obj);
      }
      
      unset($node['nid']);
      $this->buffer[$obj['nid']] = $node;
    }
  }
}
