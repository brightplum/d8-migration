<?php
/**
 * @file iwFieldHandlers.inc
 * Contains definitions for most of the basic field handlers types.
 */

/**
 * Base class for iwFieldHandler classes.
 * 
 * Also defines a static function for creating 
 * an object instance of the correct iwFieldHandler 
 * subtype that handles the field instances passed.
 */
abstract class iwFieldHandler {

  protected $field;
  protected $instance;

  /**
   * Static function to generate an object instance of
   * the correct field handlers type.  
   * 
   * @param $field
   *  Field definition of the field we want to build the handler for.
   * @param $instance
   *  Field bundle instance which we want to use the settings for
   *  to build our handler from.
   *  
   * @return iwFieldHandler | bool
   *  Returns a new iwFieldHandler instance which handles the field
   *  provided or FALSE if the correct handler could not be determined. 
   */
  static function CreateField($field, $instance) {
    static $lookup;

    // Build a look-up table for classes that extend the iwFileHandler class.
    // To match them to the field types that they handle.
    if (!isset($lookup)) {
      $lookup = array();
      foreach (iwapi_get_class_info('field_handler') as $className => $info) {
        $types  = $className::GetTypes();
        $lookup += array_fill_keys($types, $className);
      }
    }
    
    // Check the field type to ensure that we support it, and can handle it.
    $type = $field['type'];
    if (empty($lookup[$type])) {
      throw new InvalidArgumentException(t(
        'The field type "@type" is not supported',
        array('@type' => $type)
      ));
    }

    // Prep and create the new handler instance.
    $className = $lookup[$type];
    return new $className($field, $instance);
  }
  
  /**
   * Return an array listing all the field types that
   * that this handler is capable of supporting. This
   * function is mainly used to build a lookup table
   * to create the proper handler per field.
   * 
   * @return array
   *  Of field type string names, this handler supports.
   */
  static function GetTypes() {
    return array();
  }
  
  /**
   * Create the very basic field handler. All we need
   * is to store the instance and field information.
   * 
   * @param $field
   *  The global field definition for this field.
   * @param $instance
   *  The instance field settings for an entity.
   */
  protected function __construct($field, $instance) {
    $this->field    = $field;
    $this->instance = $instance;
  }
  
  /**
   * Assigns values to the entity after parsing them into the
   * correct format for the given field type.
   * 
   * @param $values
   *  The set of values to parse and assign to the entity.
   * @param $entity
   *  Reference to the entity we wish to attach values to.
   * @param $entity_type
   *  The type of entity we are currently working with.
   */
  function assign($values, &$entity) {
    /*
    $entity_type = $this->instance['entity_type'];

    // Ensure that the field information matches this entity and bundle.
    list($id, $rev_id, $bundle) = entity_extract_ids($entity_type, $entity);
    if ($this->instance['bundle'] !== $bundle) {
      throw new iwException('The entity type or bundle do not match the data from this field instance.');
    }
    */
    
    $fieldname = $this->field['field_name'];
    $values = is_array($values) ? $values : array($values);
    $lang   = entity_language($this->instance['entity_type'], $entity);
    $lang   = empty($lang) ? LANGUAGE_NONE : $lang;
    
    // finally write the field results to the entity.
    $values = $this->parseValues($values);
    
    $empty_func = "{$this->field['module']}_field_is_empty";
    if (function_exists($empty_func)) {
      $entity->{$fieldname}[$lang] = array();
      
      foreach ($values as $value) {
        if (!$empty_func($value, $this->field)) {
          $entity->{$fieldname}[$lang][] = $value;
        }
      }  
    }
    else {
      $entity->{$fieldname}[$lang] = array_filter($values);
    }
  }
  
  /**
   * Function responsible for transforming actual values
   * into valid field data for its respective type. The validation
   * should occur here as well, when there are field specific
   * conditions that should be met.
   * 
   * @param array $values
   *  An array of values that each handler will need to
   *  validate and parse into their respective value fields.
   * 
   * @return array
   *  A set of data that is ready to be used in it's
   *  respective field context.
   */
  abstract function parseValues(array $values);
}

/**
 * Field handler which handle assigning values for
 * date, datetime or datestamp field types.
 */
class iwFieldHandlerDates extends iwFieldHandler {

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('datetime', 'date', 'datestamp');
  }
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $process = 'parse'. ucfirst($this->field['type']);
    
    $dates = array();
    foreach ($values as $value) {
      if ($date = $this->{$process}($value['value'])) {
        $dates[] = $date;
      }
    }
    
    return $dates;
  }
  
  /**
   * Ensures that the date information is formatted into the
   * datetime ISO format for storage.
   * 
   * @param $value
   *  A value that is to be converted into a datetime field.
   *  Can either be a date string, or timestamp
   *  
   * @return array
   *  Return an array with enough information to create
   *  a datetime field item.
   */
  function parseDatetime($value) {
    if (is_numeric($value)) {
      $value = date('Y-m-dTH:i:s', $value);
    }
    elseif (preg_match('#^(\d{4}-\d{2}-\d{2})\s*T?(\d{2}(:?\d{2}){2})?$#', $value, $matches)) {
      $matches[2] = empty($matches[2]) ? '00:00:00' : $matches[2]; // add a default time setting.
      $value = $matches[1] .'T'. $matches[2];
    }
    else {
      return;
    }
    
    return array('value' => $value, 'date_type' => 'datetime');
  }
  
  /**
   * Parse the value into an ISO date without any time components.
   * 
   * @param $value
   *  Value to parse into an ISO date string. Can be a timestamp
   *  or time data in string format.
   *   
   * @return NULL | array
   *  Return an array with enough information to create
   *  a date field item.
   */
  function parseDate($value) {
    if (is_numeric($value)) {
      $value = date('Y-m-d', $value);
    }
    elseif (preg_match('#^(\d{4}-\d{2}-\d{2})#', $value, $matches)) {
      $value = $matches[1] .'T00:00:00';
    }
    else {
      return NULL;
    }
  
    return array(
      'value' => $value,
      'date_type' => 'date',
    );
  }

  /**
   * Create a timestamp value for a field.
   * 
   * @param $value
   *  Value to read in. Can be a timestamp
   *  or time data in string format.
   *   
   * @return NULL | array
   *  Return an array with enough information to create
   *  a timestamp field item.
   */
  function parseDatestamp($value) {
    if (is_numeric($value) || ($value = strtotime($value)) !== FALSE) {
      return array(
        'value' => $value,
        'date_type' => 'datestamp',
      );
    }

    return NULL;  
  }
}

/**
 * Field handler used to parse values for numeric types.
 */
class iwFieldHandlerNumbers extends iwFieldHandler {
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('number_decimal', 'number_float', 'number_integer');
  }

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    // Determine the appropriate value for this field.
    $processed = array();
    foreach ($values as $value) {
      if (is_numeric($value)) {
        $processed[]['value'] = ($this->field['type'] === 'number_integer') ? intval($value) : $value;
      }
    }
    return $processed;
  }
}

/**
 * Field handler that deals with text related types.
 */
class iwFieldHandlerText extends iwFieldHandler {
  protected $allowed;
  protected $format;
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('text_long', 'text_with_summary', 'text');
  }

  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    
    $this->allowed = array('value' => TRUE);
    
    // Does this field have a summary field?
    if ($field['type'] == 'text_with_summary') {
      $this->allowed['summary'] = TRUE;
    }
    
    // Does this field have text processing enabled?
    if (!empty($this->instance['settings']['text_processing'])) {
      $this->allowed['format'] = TRUE;
      $this->format = filter_default_format();
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $processed = array();

    // It is possible that it was only a single value passed, check for that first.
    if (array_key_exists('value', $values)) {
      $values['format'] = $this->format;
      $processed[] = array_intersect_key($values, $this->allowed);
    }
    else {
      foreach ($values as $value) {
        if (is_array($value) && !empty($value['value'])) {
          $value      += array('format' => $this->format);
          $processed[] = array_intersect_key($value, $this->allowed);
        }
        elseif (is_string($value)) {
          $processed[] = array('value' => $value, 'format' => $this->format);
        }
      }
    }
    return $processed;
  }
}

/**
 * Field handler to address fields that come from selectable list.
 * Handle them specially here because their possible value set is
 * limited, and needs to be vetted as an allowed value.
 */
class iwFieldHandlerList extends iwFieldHandler {
  // We'll allow for 2 chances to match values
  protected $allowedValues;   // 1.) Match exact values found in the field, not case sensitive
  protected $valueMap;        // 2.) Use a prebuilt lookup table attempt case insensitve match to values or labels.
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('list_boolean', 'list_text', 'list_integer', 'list_float');
  }
  
  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    if (isset($field['settings']['allowed_values'])) {
      $this->allowedValues = $field['settings']['allowed_values'];
      foreach ($this->allowedValues as $key => $value) {
        // Value map contains lowercase versions of the key and value that point to the original key.
        $this->valueMap[strtolower($key)]   = $key; 
        $this->valueMap[strtolower($value)] = $key; 
      }
    }
    else {
      $this->allowedValues = $this->valueMap = array();
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $processed = array();
    foreach ($values as $value) {
      $value = trim($value);
      if (empty($value)) {
        continue;
      }
      
      if (isset($this->allowedValues[$value])) {
        // Exact matches have priority
        $processed[] = array('value' => $value);
      }
      elseif (isset($this->valueMap[strtolower($value)])) {
        // We matched the text from either the key or label.
        $processed[] = array('value' => $this->valueMap[strtolower($value)]);
      }
      else {
        $params = array('%value' => $value, '%fieldname' => $this->field['field_name']); 
        drupal_set_message(t('Unable to match "%value" in field:%fieldname.', $params), 'warning');
      }
    }
    return $processed;
  }
}

/**
 * Field handler that parses files and image fields.
 * It will try to locate entries from the {managed_files}
 * database table. Files need to have already existed.
 */
class iwFieldHandlerFile extends iwFieldHandler {
  protected $basePath;
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('file', 'image');
  }

  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    
    // setup some basic path information to use.
    $settings = $this->instance['settings'];
    $scheme = empty($field['settings']['uri_scheme']) ? 
      file_default_scheme() : $field['settings']['uri_scheme'];
    
    // Setup file building and filter data.
    $this->basePath = $scheme .'://'. $settings['file_directory'];
    $this->extensions = '/\.('. preg_replace('/ +/', '|', preg_quote($settings['file_extensions'])) .')$/i';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $allowed = array('title' => TRUE, 'alt' => TRUE);

    $processed = array();
    foreach ($values as $value) {
      $properties = array();
      $value = array_filter($value, 'drupal_strlen');
      
      if (is_array($value)) {
        if (!empty($value['data'])) {
          $data = unserialize($value['data']);
          if (!empty($data) && is_array($data)) {
            $value += array_intersect_key($data, $allowed);
          }
        }

        $properties += array_intersect_key($value, $allowed);
      }
      
      $properties['fid'] = $this->findFileID($value);
      if (isset($properties['fid'])) {
        $processed[] = $properties;
      }
    }

    return $processed;
  }

  /**
   * Attempt to transform the given value into a {managed_file}
   * entry that can be returned to be used as the value of the field.
   * 
   * @param $value
   *  Name of file to be located.
   */
  protected function findFileID($value) {
    if (is_array($value)) {
      $fid  = $value['fid'];
      $name = $value['filename'];
    }
    else {
      $fid = $value;
    }
    
    // Let's see if we can find this first.
    $ent_query = new EntityFieldQuery();
    $ent_query->entityCondition('entity_type', 'file');
 
    // Build the appropriate query to test for the file's existence
    if (!empty($fid)) {
      $ent_query->entityCondition('entity_id', $fid);
    }
    elseif (!empty($name)) {
      // Check to see if the file extension matches what this field accepts.
      if (preg_match($this->extensions, $name) <= 0 ) {
        drupal_set_message(t('Invalid file extension or type @filename', array('@filename' => $value)));
        return NULL;
      }

      // Last confirmation is that a file entity with the URI exists
      preg_match('#^(?:[a-z]{2,5}:/)?/*(.*)$#i', $value, $matches);
      $uri = $this->basePath .'/'. $matches[1];
      $ent_query->propertyCondition('uri', $uri);
    }
    else {
      return NULL;
    }
    
    $rs = $ent_query->execute();
    if (isset($rs['file']) && reset($rs['file']) !== FALSE) {
      return key($rs['file']);
    }

    drupal_set_message(t('Unable to link "@value" to a file.', array('@value' => $value)));
    return NULL;
  }
}

/**
 * Field handler that handles taxonomy reference fields.
 */
class iwFieldHandlerTaxonomy extends iwFieldHandler {
  
  protected $vid;

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('taxonomy_term_reference');
  }
  
  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    
    // Determine the correct vocabulary to filter on.
    $allowed = reset($field['settings']['allowed_values']);
    if ($vocabulary = taxonomy_vocabulary_machine_name_load($allowed['vocabulary'])) {
      $this->vid = $vocabulary->vid;
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    // Determine the appropriate value for this field.
    $processed = array();

    foreach ($values as $value) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'taxonomy_term');
      
      if (isset($this->vid)) {
        $query->propertyCondition('vid', $this->vid);
      }

      if (!empty($value['tid'])) {
        $query->entityCondition('entity_id', $value['tid']);
      }
      else {
        $query->propertyCondition('name', $value['name'], 'LIKE');
      }

      $rs = $query->execute();
      if (isset($rs['taxonomy_term']) && reset($rs['taxonomy_term']) !== FALSE) {
        $processed[]['tid'] = key($rs['taxonomy_term']);
      }
    }

    return $processed;
  }
}

/**
 * Create the appropriate referencing mapping to
 * format data so that it can be used as a proper
 * entity / file reference.
 */
class iwFieldHandlerReference extends iwFieldHandler {
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('entityreference', 'node_reference', 'user_reference');
  }
  
  
  public $idKey;
  public $entityType;
  
  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    
    switch ($field['type']) {
      case 'user_reference':
        $this->entityType = 'user';
        $this->dstId = 'uid';
        break;
      case 'node_reference':
        $this->entityType = 'node';
        $this->dstId = 'nid';
        break;
      case 'entityreference':
        $this->entityType = $field['settings']['target_type'];
        $this->dstId = 'target_id';
        break;
    }
    
    $entInfo = entity_get_info($this->entityType);
    $this->idKey = $entInfo['entity keys']['id'];
  }

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    // Determine the appropriate value for this field.
    $processed = array();

    foreach ($values as $value) {        
      if (!empty($value[$this->idKey]) && (empty($value['entity_type']) || $value['entity_type'] == $this->entityType)) {
        $processed[] = array($this->dstId => $value[$this->idKey]);
      }
    }
    
    return $processed;
  }
}

/**
 * Field handler which helps with importing links.
 */
class iwFieldHandlerLink extends iwFieldHandler {

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('link_field');
  }
  
  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
  }

  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $settings = $this->instance['settings'];
    
    // Determine the appropriate value for this field.
    $processed = array();
    
    $filters = array('url' => TRUE, 'title' => TRUE, 'attributes' => TRUE);    
    foreach ($values as $value) {
      if (empty($value['attributes']) || !is_array($values['attributes'])) {
        $value['attributes'] = array();
      }
      $attr = $value['attributes'];
      
      // Set a default target if one was not set.
      if ($settings['attributes']['target'] == 'user') {
         $attr += array('target' => '_blank');
      }
      
      // Build the class information.
      if (!empty($attr['class']) && is_string($attr['class'])) {
        $attr['class'] = preg_split('#/s+#', $attr['class']);
      }
      $value['attributes'] = $attr;
      
      if (empty($value['url']) && !empty($value['value'])) {
        $value['url'] = $value['value'];
      }

      if (!empty($value['url']) || !empty($settings['url'])) {
        $processed[] = array_intersect_key($value, $filters);
      }
    }

    return $processed;
  }
}


/**
 * Field handler that deals with text related types.
 */
class iwFieldHandlerFivestar extends iwFieldHandler {
  protected $allowed;
  protected $format;
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::getTypes()
   */
  static function GetTypes() {
    return array('fivestar');
  }

  protected function __construct($field, $instance) {
    parent::__construct($field, $instance);
    
    $this->allowed = array('rating' => TRUE);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwFieldHandler::parseValues()
   */
  function parseValues(array $values) {
    $settings  = $this->instance['settings'];
    $processed = array();
    
    

    // It is possible that it was only a single value passed, check for that first.
    if (array_key_exists('rating', $values)) {
      $processed[] = array_intersect_key($values, $this->allowed);
    }
    else {
      foreach ($values as $value) {
        if (is_array($value) && !empty($value['rating'])) {
          $processed[] = array_intersect_key($value, $this->allowed);
        }
        elseif (is_string($value)) {
          $processed[] = array('rating' => $value);
        }
      }
    }
    
    foreach ($processed as &$val) {
      $val['rating'] = intval($val['rating']) * 100 / $settings['stars'];
    }
    unset($val);

    return $processed;
  }
}
