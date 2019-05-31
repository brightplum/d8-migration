<?php
/**
 * @file
 * Special field handling for terms. They were not fields in Drupal 6.
 */


/**
 * A taxonomy term reference.
 */
class d6TermFieldReader extends d6FieldReader {

  public $vid;
  public $name; 
  
  /**
   * Assign the proper vocabulary and machine names
   * for the taxonomy this field is meant to represent.
   */
  function __construct(iwD6NodeReader $src, array $vocab) {
    $this->srcReader = $src;
    
    $this->vid     = $vocab['vid'];
    $this->name    = $vocab['machine_name'];
    $this->old_vid = $vocab['old_vid'];
  }
  
  /**
   * Find the values
   */
  function getValues($data) {
    $cols = $this->columns();
    
    $query = $this->srcReader->getConnection()->select('term_node', 'tn')
      ->fields('tn', array('tid'))
      ->condition('tn.vid', $data['vid']);

    $query->innerJoin('term_data', 'td', 'td.tid=tn.tid');
    $query->condition('td.vid', $this->old_vid);
    $query->addField('td', 'name');
    
    $rs     = $query->execute();
    $values = array();
    while($value = $rs->fetchAssoc()) {
      $values[] = $value;
    }
    
    return $this->format($values);
  }
  
  function columns() {
    return array('tid', 'name');
  }
  
  function format(array $data) {
      
    if(!empty($data)) {
      $lookup = db_select('d6_importer_lookup', 'look')
        ->fields('look', array('old', 'id'))
        ->condition('data_type', 'term')
        ->condition('old', _iwapi_extract_field($data, 'tid'))
        ->execute()->fetchAllKeyed();
      
      // Use a lookup source, to find the map from old to new entity keys.
      foreach ($data as &$value) {
        if (isset($lookup[$value['tid']]) && !empty($lookup[$value['tid']])) {          
          $value['tid']  = $lookup[$value['tid']];
        }
        else {
          unset($value['tid']);
        }
      }
      unset($value);
    }
    return $data;
  }
}
