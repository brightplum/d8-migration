<?php
/**
 * @file lib/entity/iwEntityAttacher.inc
 * Base definitions for entity attacher objects.
 */


/**
 * Base interface for the entity attacher classes.
 *
 * Entity attachers are used to add fields, groups
 *  or other elements that belong to entities.
 */
interface iwIEntityAttacher {

  /**
   * Attach the relevant items from the $template variable
   * to the entity indicated by the instance values of
   * $this->type and $this->bundle.
   *
   * @param array $template
   *  Array containing the entity template information.
   *  This information is used
   * @param bool $force
   *  TRUE if the attacher should try to attach items
   *  even if they already exist.
   */
  function attach(array $template, $force = FALSE);

  /**
   * Get definitions of items that can be later
   * used to attach to other entities.
   *
   * @param array $options
   *  Array of selected options that match the keys
   *  returned by the options() function.
   *
   * @return array
   *  Array of values to add to the template.
  */
  function export();
}


/**
 * The base template for configurable entity attachers.
 */
abstract class iwEntityAttacher implements iwIConfigurable {

  protected $type;
  protected $bundle;
  
  public $values = array();

  /**
   * Generate an instance of the iwEntityAttacher, initializing
   * the entity type and bundle information.
   *
   * @param array $entity_info
   *  Array of information to signify which entity type and bundle
   *  this attacher is meant to work with.
   *
   * @throws iwException
   *  When there is not enough information to qualify an entity type.
   */
  function __construct($entity_type, $bundle) {
    if (empty($entity_type) || empty($bundle)) {
      $params = array();

      if (empty($entity_type)) {
        $params['%entity_type'] = 'entity type';
      }
      if (empty($bundle)) {
        $params['%bundle'] = 'bundle';
      }

      $msg = 'Entity attacher is missing required elements: ' . implode(', ', array_keys($params));
      throw new iwException($msg, $params);
    }

    // Record the type and bundle information.
    $this->type   = $entity_type;
    $this->bundle = $bundle;
  }

  /**
   * Create a list of possible options for this field
   * attacher. These options will be used to create checkboxes.
   */
  abstract function allowedValues();

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
  */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $element['attach_options'] = array(
      '#type' => 'checkboxes',
      '#options' => $this->allowedValues(),
      '#default_value' => $this->values,
    );
  }

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
    /* Most elements have empty validation */
  }

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    $this->values = array_filter($values['attach_options']);
  }
}
