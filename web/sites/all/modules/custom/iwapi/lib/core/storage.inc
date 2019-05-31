<?php
/**
 * @file class/storage.inc
 * Contains the base storage classes for readers and writers (exceptions too).
 */


// =============================
// Data parsing / Readers
// =============================

/**
 * Interface for parsers that can be used in conjunction with datamaps
 * - Provide an iterator interface that fetches a single entry (keyed array)
 */
abstract class iwReader extends iwPackage implements Iterator {
  
  
  // --------------------------
  // Static methods
  // --------------------------

  /**
   * Reads in the source data, and tries to verify if the
   * source is valid input for this parser.
   *
   * @param $src
   *  An identifier or actually data source to be parsed.
   *
   * @return boolean
   *  Returns TRUE if resource can be determined as being
   *  the proper type (file, array, etc...).
   */
  static function checkFormat($src) {
    throw new BadMethodCallException(
      'You must override iwReader::checkFormat() to validate source '.
      'data and report if it is a valid data type for this parser.'
    );
  }
  
  
  // --------------------------
  // Instance methods
  // --------------------------
  
  /**
   * Set the data sources that will be used internally.
   * Data sources are validated, and usually stored
   * to be used later.
   * 
   * @param <mixed> $src
   *  The data which maintains and contains the content source.
   * @throws iwReaderException
   *  If $src is not of the appropriate type.
   */
  function setSource($src) {
    $className = get_class($this);
    if (!$className::checkFormat($src)) {
      throw new iwReaderException(iwReaderException::INVALID_SOURCE, 'array');
    }
 
    $this->src = $src;
  }

  /**
   * Get the data key names that are presented from the source data.
   *
   * @return array
   *  The values of the array are the index field names.
   */
  abstract function getKeys();

  /**
   * Request the current parsing progress in a way that can later
   * be restored with the resumeProgress() function call.
   *
   * @return array
   *  In array of information pertaining to the state of this parser.
   */
  abstract function getProgress();

  /**
   * Loads a progress array and
   *
   * @param array $progress
   *  Array that should have been returned by getProgress()
   */
  abstract function resumeProgress(array $progress);
}


// =============================
// Data writers
// =============================

/**
 * Defines how data is written into a destination.
 * - Manages specific data destinations, and how to write to them.
 * - Though mappers usually handle filtering content writers are
 * 	responsible for ensure which fields can be written to and how.
 */
abstract class iwWriter extends iwPackage {
  const ALLOW_UPDATE = 0x00000001;
  const ALLOW_INSERT = 0x00000002;
  const ALLOW_ANY    = 0x00000003;  // ( ALLOW_UPDATE | ALLOW_INSERT )

  
  // =============================
  // Instance properties
  // =============================
  
  protected $mode = self::ALLOW_ANY;  // Write mode to determine what is allowed (can update, insert?).

  protected $ids       = array();  // Which fields are used to identify if an entry exists already
  protected $required  = array();  // Check what fields are required for to form a full entry.
  protected $allowed   = array();  // What fields should be allowed.
  

  // =============================
  // Instance methods
  // =============================
  
  /**
   * Required are there fields that the writer requires
   * and should check before attempting to write.
   *
   * @return array
   *  List of required fields that the writer will expect
   */
  function getRequirements() {
    return $this->required;
  }

  /**
   * Get a list of current identifiers used in this writer.
   *
   * @return array
   *  Get a list of identifiers to determine if a field exists.
   */
  function getIdentifiers() {
    return $this->ids;
  }

  /**
   * List the fields that are allowed to be used with this
   * writer, all other fields will get filtered out.
   *
   * @return array
   *  Get a list of fields that are not filtered out before
   *  writing the data to the destinations.
   */
  function getAllowed() {
    return $this->allowed;
  }

  /**
   * Validate the data array, by checking that all required
   * fields are provided in the data set.
   *
   * @param $data
   *  The data to check for integrity of the fields
   * @return boolean
   *  TRUE - if this data set seems complete and valid
   *  FALSE - if data doesn't validate.
   */
  function validate(array $data) {
    $diff = array_diff_key($this->required, $data);
    if (!empty($diff)) {
      $missing  = implode(', ', array_keys($diff));
      return t('Entry missing fields @missing.', array('@missing' => $missing));
    }
    return FALSE;
  }

  abstract function exists(array $data);
  abstract function save(array &$data);
}


// ================================
// Entity writer
// ================================

/**
 * Abstract base definition for 
 */
abstract class iwEntityWriter extends iwWriter {
  const ENTITY_TYPE = '!error!';
  
  /**
   * (non-PHPdoc)
   * @see iwPackage::getPackageProps().
   */
  static function getPackageProps() {
    $props = array(
      'mode'   => array('label' => t('Write mode')),
      // -----------------------------------------
      // TODO: Create setting to allow selection of properties / fields get filtered.
      // 'props'  => array('label' => t('Entity properties')),
      // 'fields' => array('label' => t('Entity fields')),
    );
    
    // Determine if this entity type uses bundles.
    $entity_info = entity_get_info(static::ENTITY_TYPE);
    if (!empty($entity_info['entity keys']['bundle'])) {
      $props['bundle'] = array('label' => t('Entity bundle'));
    }
    return $props;
  }
  
  
  // ------------------------------
  // Instance variables
  // ------------------------------
  
  // Entity properties
  protected $bundle;     // the name of the bundle being used.
  protected $keys;       // important entity keys
  protected $table;      // the entity base table to use.

  // Contains a built list of field handlers.
  protected $fieldHandlers = array();
  
  
  // ------------------------------
  // Instance methods
  // ------------------------------
  
  /**
   * Construct the core data use with entity writers.
   * 
   * @param array $values
   *  Array of values that correlate to the package
   *  options. These should match up with 
   *  iwEntityWriter::getPackageProps().
   * @throws InvalidArgumentException
   *  If the bundle or entity type information is invalid
   *  throw this error.
   */
  function __construct(array $values) {
    parent::__construct($values);
    
    // Check the entity type, and ensure it is all correctly set.
    if (!($entity_info = entity_get_info(static::ENTITY_TYPE))) {
      throw new InvalidArgumentException(t(
        'A valid entity type must be provided. Type: %type is not valid',
        array('%type' => static::ENTITY_TYPE)
      ));
    }
    
    $this->table = $entity_info['base table'];
    $this->keys  = $entity_info['entity keys'];
    if (empty($this->keys['bundle'])) {
      $this->bundle = static::ENTITY_TYPE;
    }
    else {
      $this->bundle = $values['bundle'];
      $this->required[$this->keys['bundle']] = $this->keys['bundle'];

      // Check if the bundle is valid.
      if (empty($entity_info['bundles'][$this->bundle])) {
        throw new InvalidArgumentException(t(
          'Invalid bundle %bundle for %type',
          array('%bundle' => $this->bundle, '%type' => static::ENTITY_TYPE)
        ));
      }
    }
    
    // Create additional instance variables.
    $instances = field_info_instances(static::ENTITY_TYPE, $this->bundle);
    foreach ($instances as $fieldname => $inst) {
      $field = field_info_field_by_id($inst['field_id']);
      if ($field && $handler = iwFieldHandler::CreateField($field, $inst)) {
        $this->allowed[$fieldname]       = $fieldname;
        $this->fieldHandlers[$fieldname] = $handler;
      }
      
      // See if this instance is required, if so mark it as such.
      if (!empty($info['required'])) {
        $this->required[$fieldname] = $fieldname;
      }
    }
    
    // Entities only have 1 unique ID field as the primary key.
    $this->ids = array($this->keys['id']);
  }
  

  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    $idKey = $this->keys['id'];
    
    if (isset($data[$idKey])) {
      $query = "SELECT count(1) FROM {{$this->table}} base WHERE base.{$idKey}=:entity_id";
      return (bool)db_query($query, array(':entity_id' => $data[$idKey]))->fetchField();
    }
    return FALSE;
  }
  
  /**
   * Apply the data which corresponds to the entity fields.
   * 
   * @param object $entity
   *  Will be the entity object that is being built. Since
   *  it is an object type, it will be treated like a reference.
   * @param array $data
   *  Data to populate if available.
   */
  protected function applyFields($entity, array $data) {    
    // Iterate and attempt to assign all the fields with their special handlers.
    foreach ($this->fieldHandlers as $fieldname => $handler) {
      if (!empty($data[$fieldname])) {
        $handler->assign($data[$fieldname], $entity);
      }
    }
  }
}


// ==================================
// Exceptions
// ==================================

/**
 * Exception for iwIParser objects
 */
class iwReaderException extends iwException {
  static protected $friendlyName = 'IW Reader';

  const INVALID_SOURCE = 0;
  const READ_ERROR     = 1;
  const INVALID_DATA   = 2;
  const RESUME_FAIL    = 3;

  function __construct($code, $src = '', $place = '', Exception $prev = NULL) {
    $args = array(
      '%src' => $src,
      '%place' => $place,
      '%line' => $this->getLine(),
      '%file' => $this->getFile(),
    );
    $message = static::getCodeMessage($code);

    // Initialize the exception with the information we managed to parse here.
    parent::__construct($message, $args, 'error', $prev);
  }

  /**
   * Translate an error code into a human readable message.
   *
   * @param $code
   *  Integer code, which are defined by constants in this exception class.
   *
   * @return string
   *  Human readable error message.
   */
  static function getCodeMessage($code) {
    switch ($code) {
      case self::INVALID_SOURCE:  return 'Source is of invalid type %src';
      case self::READ_ERROR:      return 'Encountered source reading error for %src in %place.';
      case self::INVALID_DATA:    return 'Invalid data for %src (missing keys?).';
      case self::RESUME_FAIL:     return 'Tried to resume %src with %place.';
      default:
        return 'Unable to determine parsing error %file : %line';
    }
  }
}
