<?php
/**
 * @file includes/packaging.inc
 * Contains the utilities and functions used for handling iwIPackage family of classes.
 */


// ==========================
// Interfaces and classes
// ==========================

/**
 * iwIConfigurable - Interface meant to be the basis for
 *  classes represent objects which can be configured by
 *  through a Drupal form.
 *
 *  This provides a consistent way to create objects that
 *  can be configured.
 */
interface iwIConfigurable {

  /**
   * Add form elements to represent the configurable
   * options for this object.
   *
   * @param array $element
   *  Reference to the passed form element to populate.
   *  This is usually not the whole form, only the parts
   *  relevant to building this configuration.
   * @param array $state
   *  Current state of the form element, can be used to
   *  retrieve the default values.
   * @param array $form
   *  The whole form up to this point. Can be used
   *  to infer the submit handlers and other form properties.
   * @param array $form_state
   *  Reference to the current (entire) $form_state
   *
   * @see iwIConfigurable::validateElements()
   * @see iwIConfigurable::submitElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state);

  /**
   * Validate options that were added by appendElements().
   *
   * @param array $element
   *  Reference to the passed form element to populate.
   *  This is usually not the whole form, only the parts
   *  relevant to building this configuration.
   * @param array $state
   *  Current state of the form element, can be used to
   *  retrieve the current submitted values.
   * @param array $form_state
   *  Reference to the full $form_state array.
  */
  function validateElements(array $element, array &$values, array &$form_state);

  /**
   * Submit form callback was called. The convention should be that
   * this function updates the state fo the object with the form
   * changes only. It should be up to the caller / form submit
   * to call the save functionality. This will give greater
   * flexibility and control over when things are saved, and
   * allow for AJAX callbacks to update state changes only.
   *
   * Saving might not be desired in all contexts, so it should
   * be up to the calling context to persist.
   *
   * @param array $element
   *  The passed form element to populate.
   *  This is usually not the whole form, only the parts
   *  relevant to building this configuration.
   * @param array $values
   *  Reference to the a variable that might get modified
   *  with the updated values based on form_state / submission
   *  results.
   * @param array $form_state
   *  Reference to the full $form_state form variable. This
   *  will allow triggering of redirects or other special
   *  form handling.
  */
  function submitElements(array $element, array &$values, array &$form_state);
}


/**
 * Interface for classes that can have their state information
 * be converted into an array or rebuilt from an array.
 */
interface iwIPackage extends Serializable {

  /**
   * Get an array which contains elements that represent
   * the current state of this class. The output will
   * usually be themed by the package_summary template.
   *
   * @return array
   *  Return an array that contains summary elements
   *  for this classes state and values.
   *
   * @see template_preprocess_package_summary().
   */
  function formatSummary();
  
  /**
   * Convert the packaged data into an array.
   * 
   * @return array
   *  All the serializable data from this object
   *  into array form, with key value pairs.
   */
  function packData();
}

/**
 * Base class template implementation for an iwIPackage.
 * It primary defines how objects are packages and unpacked.
 *
 * Derived classes are expected to implement:
 * - getPackageProps(): properties that get packaged.
 */
abstract class iwPackage implements iwIPackage {

  /**
   * Describe the packable items in this package class.
   *
   * @return array
   *  An array keyed with the class properties that
   *  should get packaged, and contain an array of
   *  options for the values.
   */
  static function getPackageProps() {
    // Describe persistable object properties.
    return array(/*
      '<fieldname>' => array(
        'label' => <string>,     // Label that can be used for a form label
        'multi' => <bool>,       // Multiple values
        'type'  => <class type>, // Index of category of class from iwapi_get_class_info().
      ),*/
    );
  }
  
  /**
   * Create a new iwPackage object, and populate
   * automatic properties.
   * 
   * @param array $data
   *  Array of data to populate into the packaged item.
   *  Only the iwPackage::getPackageProps() item keys
   *  will get populated.
   */
  function __construct(array $data = array()) {
    // Only allow items that got serialized to populate our data.
    // Defaults however should not be filtered, in case they need to display.
    $props = static::getPackageProps();
    
    // Assign all the data and defaults that made it past the cut.
    foreach ($props as $key => $value) {
      if (isset($data[$key])) {
        $this->{$key} = $data[$key];
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwIPackage::formatSummary()
   */
  function formatSummary() {
    $values = array();
    
    // Iterate the properties and format them as a summary.
    foreach (static::getPackageProps() as $key => $info) {
      $values[$key] = $this->{$key};
    }
    
    return array(
      '#theme' => 'package_summary',
      '#properties' => $values,
    );
  }

  /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    return array_intersect_key((array)$this, static::getPackageProps());
  }
  
  /**
   * (non-PHPdoc)
   * @see Serializable::serialize()
   */
  function serialize() {    
    // Serialize the array to later restore the values.
    return serialize($this->packData());
  }

  /**
   * (non-PHPdoc)
   * @see Serializable::unserialize()
   */
  function unserialize($data) {
    $values = unserialize($data);
    if (is_array($values)) {
      $this->__construct($values);
    }
    else {
      // Fail-safe if the serialized data failed
      $this->__construct();
    }
  }
}


// ==============================
// Abstract class templates
// ==============================

/**
 * A packaged utility is an object that can be packaged
 * and later get run on a set of data. Such as an importer
 * or exporter of data.
 */
interface iwIPackagedUtility extends iwIPackage, iwIConfigurable {

  /**
   * Process the contents of a resource based.
   *
   * @param mixed $resource
   *  The data to process using this utility. As to what this
   *  data is, is determined by the utility and how it processes
   *  the resource in question. It could for instance by an array,
   *  or URI to a file.
   */
  function execute($resource);

  /**
   * Process the data to this utility as a batch process. The primary
   * difference from execute, is that this function should modify the
   * context to track positioning or progress so processing can resume
   * on successive calls.
   *
   * @param array $context
   *  Information about the resource to be processed by this utility.
   *  The resource format should generally be the same as with execute,
   *  and in some way partitionable / or progress tracked.
   * @param number $size
   *  The number of items to run in a single batch operation.
   *
   * @see iwIPackagedUtility::execute().
  */
  function batch(array &$context, array &$results, $size = 100);
}


/**
 * Base abstract class for the package utilities.
 */
abstract class iwPackagedUtility extends iwPackage implements iwIPackagedUtility {

  /**
   * Try to find configurable data and report
   * it as something that can be 
   * 
   * @return array
   *  Array which describes some properties and how they
   *  would be used with iwPackage subclasses.  
   */
  static function getPackageProps() {
    $vars = get_class_vars(get_called_class());
    
    $retVal = array();
    foreach (array_keys($vars) as $key) {
      // Try infer information about these values.
      $retVal[$key] = array(
        'label' => check_plain(ucfirst($key)),
        'multi' => is_array($vars[$key]),
      );
    }
    
    return $retVal;
  }
}


/**
 * An exception to signal failure related to packaging.
 */
class iwPackageException extends iwException {
  static protected $friendlyName = 'IWAPI Package';
}
