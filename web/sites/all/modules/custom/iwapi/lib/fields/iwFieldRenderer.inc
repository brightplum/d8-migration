<?php
/**
 * @file 
 * Handles rendering a field in a uniform way as the iwEntityProperty class.
 */

class iwFieldRenderer implements iwIEntityProperty {

  public $fieldName;
  public $settings;
  
  public $title    = '';
  public $cardinal = 1;
  
  
  // ==============================
  // Instance functionality
  // ==============================
  
  /**
   * 
   * @param string $fieldName
   *  The name o
   * @param array $config
   *  The various display settings to use when rendering
   *  the field. Defaults will be applied when settings
   *  are missing.
   */
  function __construct($fieldName, array $config = array()) {
    $this->fieldName = $fieldName;
    
    // General display options.
    $this->title    = isset($config['title'])   ? $config['title'] : '';
    $this->settings = isset($config['display']) ? $config['display'] : array();
 
    // The number of items to display, defaults to only 1.
    if (!empty($config['cardinality'])) {
      $this->cardinal = $config['cardinality'];
    }
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIEntityProperty::getValue()
   */
  function getValue($entity) {
    return $entity->{$this->fieldname};
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIEntityProperty::getSearchInfo()
   */
  function getSearchInfo() {
    return FALSE; // we don't present search data from this interface.
  }

  /**
   * Render the field using the settings configured
   */
  function view($entity, $entity_type) {
    list($id, $rev, $bundle) = entity_extract_ids($entity_type, $entity);
    
    if (!isset($this->bundles[$bundle])) {
      $instance = field_info_instance($entity_type, $this->fieldName, $bundle);
    
      if (empty($instance)) {
        watchdog('iwsearch', 'Unable to render field %field for results of %name', array(
          '%field' => $this->fieldName,
          '%name' => $this->provider->getName(), 
        ));
        return array();
      }
      
      // Combine the default settings with display of this instance. 
      $this->bundles[$bundle] = $this->settings + $instance['display']['default'];
      if (!empty($this->settings['settings'])) {
        $this->bundles[$bundle]['settings'] += $instance['display']['default']['settings'];
      }
    }
    
    // Assign the default settings here and try to render the field.
    $renderable = field_view_field($entity_type, $entity, $this->fieldName, $this->bundles[$bundle]);
    
    if ($this->cardinal > 0) {
      $items = field_get_items($entity_type, $entity, $this->fieldName);
      
      // Only need to do this if the # of fields exceeds the requested cardinality.
      if (count($items) > $this->cardinal) {
        $keys = array_keys($items);
        array_splice($keys, 0, $this->cardinal);
        
        foreach ($keys as $key) {
          unset($renderable[$key]);
        }
      }
    }
    
    // Apply our custom title, if one was needed.
    if (!empty($this->title)) {
      $renderable['#title'] = check_plain($this->title);
    }
    
    return $renderable;
  }
}
