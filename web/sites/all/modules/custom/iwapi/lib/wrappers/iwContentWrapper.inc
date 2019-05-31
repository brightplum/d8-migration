<?php
/**
 * @file
 * Contains the base HTML content wrapper.
 */


/**
 * Handles managing a single instance of field grouping data.
 */
class iwContentWrapper implements iwIConfigurable {

  // ----------------------------------
  // Instance properties
  // ----------------------------------

  public $controller; // class name of the controller class managing the cache and scope.

  public $scope;      // Scope identifier to which this content wrapper applies.
  public $name;       // Identifier of this wrapper within the scope.

  public $label;
  public $weight   = 0;
  public $children = array();
  public $settings = array(
    'display_title' => 'visible',
    'span' => -1,
    'equal_heights' => FALSE,
    'hide_empty' => TRUE,
    'attributes' => array('class' => array()),
  );


  // ----------------------------------
  // Instance methods
  // ----------------------------------

  /**
   * Populate this field grouper based on array values.
   *
   * @param string $controller
   *  Class name of the managing controller, which
   *  facilitates usage + cache of these content wrappers.
   * @param array $values
   *  An array of values which match properties
   *  of this class to populate.
  */
  function __construct($controller, array $values = array()) {
    $vars = get_class_vars(get_called_class());
    
    // Pass values from the $values that match any local properties.
    foreach ($vars as $key => $default) {
      if (isset($values[$key])) {
        $this->{$key} = $values[$key];
      }
    }

    // Apply defaults and settings that require extra handling.
    $this->settings  += $vars['settings'];
    $this->controller = $controller;
  }
  
  /**
   * Tell the system if this content wrapper type allows
   * for a variation of different widths.
   * 
   * @return boolean
   *  TRUE if this container type supports a variable width.
   */
  function hasWidth() {
    return TRUE;
  }

  /**
   * Persist the configurations that are stored for this
   * field group configuration.
   */
  function save() {
    if ($schema = drupal_get_schema('iwapi_containers')) {
      // Build the correct data format for saving to the database.
      $values = array_intersect_key((array)$this, $schema['fields']);
      $values['class_name'] = get_class($this);
      $values['data'] = array_diff_key((array)$this, $schema['fields']);
      $values['data'] = serialize($values['data']);

      $keys = array(
        'scope' => $values['scope'],
        'name' => $values['name'],
      );

      // Try to save or update the new content.
      db_merge('iwapi_containers')
        ->key($keys)
        ->fields($values)
        ->updateFields(array(
          'label' => $values['label'],
          'weight' => $values['weight'],
          'class_name' => $values['class_name'],
          'data' => $values['data'],
        ))->execute();

      // Update the cache value, so that other functions will get the persisted / current version.
      $ctrlClass = $this->controller;
      $ctrlClass::ResetCache($this->scope);
    }
    else {
      throw new PDOException(t(
        'Invalid table name: @table',
        array('@table' => 'iwapi_containers')
      ));
    }
  }

  /**
   * Delete this fieldgroup setup / settings from persisting.
   */
  function delete() {
    db_delete('iwapi_containers')
      ->condition('scope', $this->scope)
      ->condition('name', $this->name)
      ->execute();

    // Update the cache value, so that other functions will get the persisted / current version.
    $ctrlClass = $this->controller;
    $ctrlClass::ResetCache($this->scope, $this->name);
  }

  /**
   * Move all child elements from a source to a
   * destination. Essentially is the method which
   * moves child items to their final destination.
   * 
   * @param array $element
   *  Reference to the source array. Items will be removed
   *  from this source destination.
   * @param array $dst
   *  Reference to the array where to move all the elements
   */
  function transferItems(array &$element, array &$dst) {
    $has_content = FALSE;
    
    foreach ($this->children as $name) {
      if (isset($element[$name])) {
        $has_content = TRUE;
        $dst[$name] = $element[$name];
        unset($element[$name]);
      }
    }

    // Consider this element sorted.
    $dst['#sorted'] = TRUE;
    $dst['#access'] = $has_content || empty($this->settings['hide_empty']);
  } 
  
  /**
   * Modify the element to reflect the changes
   * required by this container wrapper class.
   *
   * @param array $element
   *  Reference to the element for which to apply
   *  the changes to.
   */
  function view() {
    $path = drupal_get_path('module', 'iwapi');

    $element = array(
      '#type' => 'container',
      '#weight' => $this->weight,

      '#attached' => array(
        'css' => array($path .'/theme/css/content-wrappers.css'),
      ),
    );

    // Move the elements over.
    $this->prepareView($element);
    return $element;
  }


  // ===================================
  // Additional formatting utilities
  // ===================================

  /**
   * Apply the title to the wrapper content
   * depending on the settings of the content wrapper.
   *
   * @param array $wrapper
   *  Reference to the wrapper array to apply the title to.
   */
  protected function applyTitle(array &$wrapper) {
    $ctrlClass = $this->controller;
    $attr = array('class' => array($ctrlClass::CssName .'-title'));

    switch ($this->settings['display_title']) {
      case 'inline':
        $attr['class'][] = 'inline';
      case 'visible':
        $wrapper['container_title'] = array(
          '#theme' => 'html_tag',
          '#tag' => 'h3',
          '#value' => check_plain($this->label),
          '#attributes' => $attr,
          '#weight' => -10,
        );
        break;
      case 'hidden':
      case 'invisible':
        break;
      default:
        drupal_set_message(t('@class: Unknown title display setting %display', array(
          '@class'   => get_class($this),
          '%display' => $this->settings['display_title'],
        )), 'warning');
    }
  }

  /**
   * Apply settings and attributes to the wrapper
   *
   * @param array $wrapper
   *  Reference to the currently building wrapper
   *  array. This is a renderable array to process.
   */
  protected function applyAttributes(array &$wrapper) {
    $ctrlClass = $this->controller;
    $attrs = $this->settings['attributes'];

    // Tag an ID attribute if one is not already specified.
    if (!isset($attrs['id'])) {
      $attrs['id'] = $ctrlClass::CssName . '-';
      $attrs['id'] .=  preg_replace(array('/\W+/', '/_/'), array('', '-'), $this->name);
    }

    // Add the content wrapper class.
    array_unshift($attrs['class'], $ctrlClass::CssName);
 
    $wrapper['#attributes'] = $attrs;
  }
 
  /**
   * Prepares the display element array to get displayed.
   * Sets up the title, attributes and column widths.
   * 
   * @param array $item
   *  Reference to the display array to get
   *  modified before actual rendering happens.
   */
  protected function prepareView(array &$item) {
    if (!empty($this->settings['display_title'])) {
      $this->applyTitle($item);
    }
    $this->applyAttributes($item);
    
    // Is there a width applied to this item.
    if ($this->hasWidth() && intval($this->settings['span']) > 0) { 
      if (!empty($this->settings['equal_heights'])) {
        $path = drupal_get_path('module', 'iwapi');
        $item['#attributes']['class'][] = 'equal-heights';
        
        $item['#attached']['js'][$path .'/js/equal-heights.js'] = array(
          'type' => 'file',
          'group' => JS_THEME,
          'weight' => 99,
        );
      }
      
      $item['#attributes']['class'][] = 'content-column';
      $item['#attributes']['class'][] = 'span-'. $this->settings['span'];
        
      $item['#prefix'] = '<div'. drupal_attributes($item['#attributes']).'>';
      $item['#suffix'] = '</div>';
      $item['#attributes'] = array('class' => array('inner-column', 'clearfix'));
    }
    else {
      $item['#attributes']['class'][] = 'content-container';
      $item['#attributes']['class'][] = 'clearfix';
    }
  }
  
  // ----------------------------------
  // Form configurations
  // ----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $element['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#size' => 32,
      '#weight' => -10,
      '#default_value' => $this->label,
    );
    
    $element['display_title'] = array(
      '#type' => 'select',
      '#title' => t('Title display'),
      '#weight' => -5,
      '#options' => array(
        'visible' => t('Above'),
        'inline'  => t('Inline'),
        'hidden'  => t('Hidden'),
      ),
      '#default_value' => $this->settings['display_title'],
    );
    
    $element['hide_empty'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide wrapper if empty'),
      '#weight' => -4,
      '#default_value' => $this->settings['hide_empty'],
    );
    
    if ($this->hasWidth()) {
      $range = range(1, 24);
      $element += array(
        'span' => array(
          '#type' => 'select',
          '#title' => t('Grid columns'),
          '#empty_value' => '-1',
          '#empty_option' => t('Full width'),
          '#options' => array_combine($range, $range),
          '#default_value' => $this->settings['span'],
          '#description' => t('Number of grid columns to span (column width in grid units).'),
        ),
        'equal_heights' => array(
          '#type' => 'checkbox',
          '#title' => t('Use equal heights?'),
          '#default_value' => !empty($this->settings['equal_heights']),
        ),
      );
    }
    
    // CSS classes to apply.
    $element['attributes'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#weight' => 10,

      'class' => array(
        '#type' => 'textfield',
        '#title' => t('CSS classes'),
        '#size' => 32,
        '#element_validate' => array('iwapi_element_validate_css_classes'),
        '#default_value' => implode(' ', $this->settings['attributes']['class']),
      ),
    );
  }

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    $this->label = $values['label'];
    unset($values['label']);
    
    foreach ($element as $name => $item) {
      if (isset($values[$name])) {
        $this->settings[$name] = $values[$name];
      }
    }
  }
  
  /**
   * Build an renderable array of the current object
   * state items that are configurable through form
   * items added by iwIConfigurable::appendElements().
   */
  function formatSummary() {
    $summary = array('#type' => 'container');
    
    // Get the title display.
    switch ($this->settings['display_title']) {
      case 'visible':
      case 'inline':
        $summary['display_title'] = array(
          '#markup' => '<strong>Title:</strong> '. $this->settings['display_title'],
          '#prefix' => '<div>',
          '#suffix' => '</div>',
          '#weight' => -5,
        );
        break;
    }
    
    // Add the status
    if (!empty($this->settings['equal_heights'])) {
      $summary['equal_heights'] = array(
        '#markup' => '<strong>Equal heights:</strong> Yes',
        '#prefix' => '<div>',
        '#suffix' => '</div>',
        '#weight' => 99,
      );
    }
    
    if ($this->settings['span'] > 0) {
      $summary['span'] = array(
        '#markup' => "<strong>Width: </strong>{$this->settings['span']}",
      );
    }

    // Display additional classes
    if (!empty($this->settings['attributes']['class'])) {
      $summary['class'] = array(
        '#markup' => '<strong>Classes:</strong> '. implode(' ', $this->settings['attributes']['class']),
        '#prefix' => '<div>',
        '#suffix' => '</div>',
        '#weight' => 20,
      );
    }
    return $summary;
  }
}
