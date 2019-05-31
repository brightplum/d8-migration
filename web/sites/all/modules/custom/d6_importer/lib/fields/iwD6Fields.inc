<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


/**
 * The base class for the Drupal 6 field readers. These will
 * map data, into read to consume chunks for the importer.
 */
abstract class d6FieldReader {
  
  /**
   * FieldReader factory, which will create a new
   * instance of the correct type to handle and arrange
   * the field information.
   * 
   * @param type $field
   *  Field information and data.
   * @param type $instance
   *  Field instance information.
   */
  static function CreateFieldReader(iwD6NodeReader $src, $field, $instance) {
    switch ($field->type) {
      case 'text':
        return new d6TextFieldReader($src, $field, $instance);
      case 'link':
        return new d6LinkFieldReader($src, $field, $instance);
      case 'nodereference':
        return new d6ReferenceFieldReader($src, $field, $instance);
      case 'filefield':
        return new d6FileFieldReader($src, $field, $instance);
      default:
        drupal_set_message("type not found: {$field->type}({$field->field_name})", 'error');
        return NULL;
    }
  }


  // ===============================
  // Instance variables / methods
  // ===============================

  protected $srcReader;

  protected $name;
  protected $type;
  protected $field;
  protected $instance;

  public $hasTable;
  public $multi;

  /**
   * Take the field settings and properties and
   * prepare them for later usage.
   * 
   * @param type $field
   * @param type $instance
   */
  protected function __construct(iwD6NodeReader $src, $field, $instance) {
    $this->srcReader = $src;
    
    // Field specific items.
    $this->name     = $field->field_name;
    $this->type     = $field->type;
    $this->field    = $field->settings;
    $this->instance = $instance;
    
    $this->multi    = (bool)$field->multiple;
    $this->hasTable = (!$field->db_storage) || $this->multi;
  }
  
  /**
   * Fetches information from the databases.
   * 
   * @param array $data
   *  Data pulled from the database for the node
   *  content thus far.
   */
  function getValues($data) {
    $cols = $this->columns();
    
    $values = array();
    if ($this->hasTable) {
      $rs = $this->srcReader->getConnection()->select("content_{$this->name}", 'f')
        ->fields('f', $cols)
        ->condition('vid', $data['vid'])
        ->execute();

      while ($value = $rs->fetchAssoc()) {
        $values[] = $this->trimKeyNames($value);
      }
    }
    else {
      $values[] = $this->trimKeyNames(array_intersect_key($data, array_flip($cols)));
    }
    
    // Convert the values into expected format for the complementary type.
    return $this->format($values);
  }
  
  /**
   * Trim the prefix from the value names.
   * 
   * @param array $items
   *  Array of for which to trim the key names for.
   */
  function trimKeyNames(array $items) {
    $values = array();
    
    foreach ($items as $key => $value) {
      $values[preg_replace("/^{$this->name}_/", '', $key)] = $value;
    }
    return $values;
  }
  
  abstract function columns();
  abstract function format(array $values);
}

/**
 * Handles the body field for Drupal 6. Here it
 * is treated differently from Drupal 7.
 */
class d6BodyFieldReader extends d6FieldReader {
  
  function __construct() {
    // Body field has no additional information needed to read it.
  }
  
  function getValues($data) {
    $cols = $this->columns();
    
    $values[] = array_intersect_key($data, array_flip($cols));
    return $this->format($values);
  }
  
  function columns() {
    return array('body', 'teaser');
  }
  
  function format(array $values) {
    $translated = array();
    foreach ($values as $items) {
      $translated[] = array(
        'value'   => $items['body'],
        'summary' => $items['teaser'],
      );
    }
    return $translated;
  }
}


/**
 * Handles the text types.
 */
class d6TextFieldReader extends d6FieldReader {
  
  function columns() {
    return array($this->name .'_value');
  }

  function format(array $values) {
    return $values;
  }
}

/**
 * Handles link fields, and remaps them.
 */
class d6LinkFieldReader extends d6FieldReader {
  
  function columns() {
    $cols = array('url', 'title', 'attributes');
    array_walk($cols, '_iwapi_prefix_value', $this->name .'_');
    
    return $cols;
  }

  function format(array $values) {
    foreach ($values as &$value) {
      if (!empty($value['attributes'])) {
        $value['attributes'] = unserialize($value['attributes']);
        
        if (!empty($value['attributes']['class']) && is_string($value['attributes']['class'])) {
          $value['attributes']['class'] = preg_split('/\s+/', $value['attributes']['class']);
        }
      }
    }
    unset($value);
    
    return $values;
  }
}

/**
 * Read references and try to map them back out.
 */
class d6ReferenceFieldReader extends d6FieldReader {
  
  public $idKey;
  public $type;
  
  function __construct(iwD6NodeReader $src, $field, $instance) {
    parent::__construct($src, $field, $instance);
    
    switch ($field->type) {
      case 'nodereference':
        $this->idKey = 'nid';
        $this->type  = 'node';
        break;
      case 'userreference':
        $this->idKey = 'uid';
        $this->type  = 'user';
        break;
    }
  }

  function columns() {
    return array("{$this->name}_{$this->idKey}");
  }
  
  function format(array $data) {
    if(!empty($data)) {
      $lookup = db_select('d6_importer_lookup', 'look')
        ->fields('look', array('old', 'id', 'entity_type'))
        ->condition('data_type', $this->type)
        ->condition('old', _iwapi_extract_field($data, $this->idKey))
        ->execute()->fetchAllAssoc('old');
      
      // Use a lookup source, to find the map from old to new entity keys.
      foreach ($data as &$value) {
        if (isset($lookup[$value[$this->idKey]]) && !empty($lookup[$value[$this->idKey]]->id)) {
          $old = $lookup[$value[$this->idKey]];
          
          $entity_info = entity_get_info($old->entity_type);
          $keys = $entity_info['entity keys'];
          
          $value['entity_type'] = $old->entity_type;
          $value[$keys['id']]   = $old->id;
        }
        else {
          $value[$this->idKey] = 0;
        }
      }
    }

    return $data;
  }
}

/**
 * Read data from file fields (includes docs and images).
 */
class d6FileFieldReader extends d6FieldReader {

  protected $fileDir = NULL;
  
  function columns() {
    return array($this->name .'_fid', $this->name .'_data');
  }
  
  function format(array $values) {      
    if (!isset($this->fileDir)) {
      $this->fileDir = $this->srcReader->getConnection()->select('variable', 'v')
        ->fields('v', array('value'))
        ->condition('name', 'file_directory_path')
        ->execute()->fetchField();
      
      $this->fileDir = !empty($this->fileDir) ? unserialize($this->fileDir) : 'sites/default/files';
    }
    
    // Find out where these files really belong.
    $fids = _iwapi_extract_field($values, 'fid');
    if (!empty($fids)) {
      $lookup = db_select('d6_importer_lookup', 'look')
        ->fields('look', array('old', 'id'))
        ->condition('data_type', 'file')
        ->condition('old', $fids)
        ->execute()->fetchAllKeyed();
      
      // Load full file information for these items as well.
      $rs = $this->srcReader->getConnection()->select('files', 'f')
        ->fields('f', array('fid', 'filename', 'filepath', 'filemime', 'filesize', 'status'))
        ->condition('fid', $fids)
        ->execute();
      
      $files = array();
      while ($file = $rs->fetchAssoc()) {
        $files[$file['fid']] = $file;
      }
      
      // Finally put the data where it belongs
      foreach ($values as &$value) {
        if (isset($value['fid']) && !empty($files[$value['fid']])) {
          $value += $files[$value['fid']];
          
          // Adjust the fid if it has been translated previously.
          if (!empty($lookup[$value['fid']])) {
            $value['fid'] = $lookup[$value['fid']];
          }
          else {
            unset($value['fid']); // This file ID is unusable.
          }

          // In D7 these are moved to the top level.
          if (!empty($value['data']) && is_array($imageData = unserialize($value['data']))) {
            $value += $imageData;
          }
        }
      }
      unset($value);
    }
    
    return $values;
  }
}
