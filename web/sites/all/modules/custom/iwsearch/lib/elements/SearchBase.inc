<?php
/**
 * @file search/iwSearchElement.inc
 * Contains the base class for items which can be used as search
 * elements in the generated search forms.
 */


class iwSearchValue {

  // Protected information that shouldn't get changed.
  protected $indexKey;
  protected $sortType;
  
  /**
   * Constructor for the search value type.
   * 
   * @param string $indexKey
   *  The index key used for SolrKey.
   * @param string $type
   *  The type of value being processed.
   */
  function __construct($indexKey, $type, $sort = NULL) {
    $this->indexKey  = $indexKey;
    $this->valueType = $type;
    $this->sortType  = isset($sort) ? $sort : $type;
  }

  /**
   * Report the type of value used
   * by this search value.
   * 
   * @return string
   *  Name of type of value which is used
   *  when building the search filters.   
   */
  function getValueType() {
    return $this->valueType;
  }
  
  /**
   * Report the type of value to use for
   * use with the sorting.
   * 
   * @return string
   *  The name of the value type which is
   *  used in constructing the search.
   */
  function getSortType() {
    return $this->sortType;
  }
  
  /**
   * Get the index key to use for this value and search type.
   * Most elements will just use the $indexKey value,
   * but there are special cases for changing dynamically.
   *
   * @return string
   *  The string value of the Solr Index key
   *  to use for the value of this search element.
   */
  function getIndexKey() {
    return $this->indexKey;
  }
}

/**
 * Base class for all search elements used by the iwapi search system.
 * 
 * Search is driven with Drupal entities and therefore all relate to
 * a single entity type and bundle combination. 
 */
abstract class iwSearchElement extends iwSearchValue implements iwIConfigurable, iwIPackage {

  // Defined constants for dictating how element labels are displayed.
  // Not all elements can / should support all options. It will be
  // up to the specific element how to handle this.
  const LABEL_HIDDEN = 0; // The label should be hidden.
  const LABEL_INSIDE = 1; // Display label inside of the field.
  const LABEL_INLINE = 2; // Display label next to element.
  const LABEL_ABOVE  = 3; // Default Drupal form label display (over the input).


  // ------------------------------
  // Instance properties
  // ------------------------------

  // Publically available information
  public $label        = '';
  public $labelDisplay = iwSearchElement::LABEL_ABOVE;

  public $multi      = FALSE;
  public $autoSubmit = FALSE;


  // ------------------------------
  // Instance methods
  // ------------------------------
  
  /**
   * Generate data for creating representing a search element.
   * 
   * @param array $data
   *  Object specific data for this element, which set the state
   *  and behavior of this instance.
   */
  function __construct(array $data = array()) {
    $vars = get_class_vars(get_called_class()); 
    foreach ($vars as $key => $default) {
      if (isset($data[$key])) {
        $this->{$key} = $data[$key];
      }
    }
  }
  
  /**
   * Reports what label display options are
   * supported by this
   * 
   * @return array
   *  The list of supported label display options
   *  for this particular search element.
   */
  function getLabelOptions() {
    return array(
      self::LABEL_HIDDEN => t('Hidden'),
      self::LABEL_ABOVE  => t('Above'),
      self::LABEL_INSIDE => t('Embedded'),
      self::LABEL_INLINE => t('Inline'),
    );
  }
  
  /**
   * Create the schema column definitions which will be used by
   * used by this field for storing information into an index
   * table. It is inserted directly into the $schema['fields'].
   *
   * @param array $schema
   *  Reference to the schema array. This function will
   *  insert additional field and indexes into this array.
   */
  abstract function setElementSchema(array &$schema);
  
  /**
   * This search element will is responsible for populating its
   * own data into the index tables. This should be data fields
   * specified by the iwSearchElement::getElementSchema();
   *
   * @param object $entity
   *  The entity of the appropriate type and bundle. This will be
   *  used as the data source.
   * @param array $data
   *  Reference to the current data set to be written. The function
   *  will translate the appropriate data from the $entity to this
   *  $data array.
   */
  abstract function indexElement($entity, array &$data);
  
  
  // ---------------------------------
  // Implement iwIPackage
  // ---------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwIPackage::formatSummary()
   */
  function formatSummary() {
    $label = $this->label;
    
    $props = array();
    switch ($this->labelDisplay) {
      case self::LABEL_INSIDE:
        $label .= ' (embedded)';
        break;

      case self::LABEL_HIDDEN:
        $label .= ' (hidden)';
        break;

      case self::LABEL_INLINE:
        $label .= ' (inline)';
        break;
    }
  
    return array(
      '#theme'      => 'package_summary',
      '#properties' => array(
        'Label' => $label,
        'Auto submit' => $this->autoSubmit ? 'Yes' : 'No',
      ),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    $vars = get_class_vars(get_called_class());
    $values = get_object_vars($this);
    
    // Return current state of element for class
    // defined properties.
    return array_intersect_key($values, $vars);
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
  
  
  // ================================
  // iwIConfigurable
  // ================================
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $element['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Display title'),
      '#size' => 32,
      '#weight' => -10,
      '#default_value' => $this->label,
    );
    $element['labelDisplay'] = array(
      '#type' => 'select',
      '#title' => t('Label display'),
      '#weight' => -9,
      '#options' => $this->getLabelOptions(),
      '#default_value' => $this->labelDisplay,
    );
    
    $element['autoSubmit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto submit on filter changes'),
      '#weight' => -8,
      '#default_value' => $this->autoSubmit,
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
    /* Default elements don't need additional validation */
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    // Set the native values for this element.  
    $this->label        = $values['label'];    
    $this->labelDisplay = $values['labelDisplay'];
    
    // Update the auto-submit flag.
    $this->autoSubmit = (bool)$values['autoSubmit'];
    
    // There are times where the original element passed
    // and the current working object are no longer referencing
    // the same heap object. Assign the form state so that
    // the caller can find the updated version of this object.
    $form_state['search_element'] = $this;
  }


  // ================================
  // Search form
  // ================================
  
  /**
   * Pass the translated value into the current
   * filter query string. This will be used
   * to represent the search criteria processed.
   * 
   * @param <mixed> $value
   *  The value used with the search.
   * @return string | NULL
   *  A string which represents the current search 
   *  criteria filter used with this field.
   */
  function filterString($value = NULL) {
    return NULL;
  }
 
  /**
   * Most of the common label settings apply for
   * all elements, and present a uniform way of
   * adding them to the search element.
   * 
   * @param array $element
   *  Reference to the element for which
   *  to assign the title attribute for.
   */
  function setElementTitle(array &$element) {
    if (!empty($this->label)) {
      
      switch ($this->labelDisplay) {
        case static::LABEL_HIDDEN:
          $element['#title_display'] = 'invisible';
          $element['#title'] = check_plain($this->label);
          break;
      
        case static::LABEL_INLINE:
          $element['#attributes']['class'][] = 'inline-label';

        case static::LABEL_ABOVE:
          $element['#title'] = check_plain($this->label);
          break;
    
        case static::LABEL_INSIDE:
          $element['#attributes']['placeholder'] = check_plain($this->label);
          break;
      }
    }
  }
  
  /**
   * Attach the search widget to a search form. This allows
   * the widget to define its own data and search params.
   * 
   * @param array $form_state
   *  Reference to the full $form_state values to use
   *  when generating the search form.
   * @param <mixed> $values
   *  Current search field values. This value is not
   *  contained in the object instance.
   */
  abstract function attachForm(array &$form_state, $default = NULL);
  
  /**
   * Validation callback for the search element. If there
   * is a validtion error, call form_error() and set the
   * element which has failed validate.
   * 
   * @param array $elements
   *  Array of search element definitions. This should be
   *  the parent array where the search element lives.
   * @param array $values
   *  Reference to the current set of form values.
   */
  function attachValidate(array $elements, array &$values) {
    // Most validation might be empty for common fields.
  }
  
  /**
   * Take the raw form values and format them into
   * a format which can be used by some standard
   * value_type query builders for the provider.
   *
   * @param <mixed>
   *  The raw value from the attach form input.
   * @return <mixed> | NULL
   *  Either the translated value to match the search
   *  element or NULL if the value is empty / shouldn't
   *  be used.
   */
  abstract function translateValue($value);
  
}


// #################################
// # Search fields
// #################################

/**
 * Abstract base class for implementing search fields.
 */
abstract class iwSearchFieldElement extends iwSearchElement {
  
  protected $valCol;
  protected $fieldName;

  /**
   * Static function to help with determine where the data
   * might live within related information.
   *
   * @param array $field
   *  An array that contains the current field information.
   * @return string
   *  The index name fo the column used to contain what we should
   *  treat as the value of the field. This is not the full database
   *  name of the column
   */
  static function MapFieldToColumn(array $field) {
    // TODO: Add a HOOK to allow the altering / addition to this mapping.
    // TODO: Cache the mapping look-up so this will be fast.
    switch ($field['type']) {
      case 'taxonomy_term_reference':
        return 'tid';
      case 'user_reference':
        return 'uid';
      case 'node_reference':
        return 'nid';
    }
 
    return 'value';
  }

  /**
   * Create a MyViceSearchWidget object to handle search fields
   * connected to a entity type, bundle and field combination.
   *
   * @param string $entity_type
   *  String name of the entity type that is used with this field.
   * @param string $bundle
   *  The bundle name to be used with this field type.
   * @param array $data
   *  Any additional data which can be used to determine how fields
   *  are displayed or the data should be indexed.
   */
  function __construct(array $data = array()) {
    if (isset($data['field_name']) && empty($data['fieldName'])) {
      $data['fieldName'] = $data['field_name'];
    }
    
    parent::__construct($data);
    
    // Field specific management values.
    if ($field = field_info_field($this->fieldName)) {
      $this->valCol = static::MapFieldToColumn($field);
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getElementSchema()
   */
  function setElementSchema(array &$schema) {
    if ($field_info = field_info_field($this->fieldName)) {
      $table  = _field_sql_storage_tablename($field_info);
      $column = _field_sql_storage_columnname($this->fieldName, $this->valCol);
   
      $field_schema = drupal_get_schema($table);
      $schema['fields'][$this->fieldName] = $field['fields'][$column];
      // TODO: Determine if there should be any indexes built with this field.   
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::indexElement()
   */
  function indexElement($entity, array &$data) {
    $items = field_get_items($this->entityType, $entity, $this->fieldName);
    if (!empty($items)) {
      $item  = reset($items);

      // Save what should be the field value into this data array.
      $data[$this->fieldName] = $item[$this->valCol];
    }
    else {
      $data[$this->fieldName] = NULL;
    }
  }
}
