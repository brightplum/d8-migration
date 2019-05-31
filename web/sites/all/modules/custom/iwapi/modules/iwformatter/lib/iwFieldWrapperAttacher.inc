<?php
/**
 * @file
 * Contains the definition for the handler which facilitates 
 * attaching & exporting field wrappers with entities.
 */

class iwFieldWrapperAttacher extends iwEntityAttacher {
  
  protected $view_modes;

  /**
   * Constructor for this entity attacher. It will
   * create a list of view modes available for
   * the provided entity type.
   * 
   * @param $entity_type
   *  The type of entity we are going to handle with
   *  this object instance.
   * @param $bundle
   *  Name of the bundle we are using this attacher for.
   * 
   * @throws iwException
   */
  function __construct($entity_type, $bundle) {
    parent::__construct($entity_type, $bundle);
    
    $info = entity_get_info($this->type);
    if (!empty($info)) {
      $this->view_modes = array(
        'form' => array('label' => 'Form'),
        'default' => array('label' => 'Default'),
      ) + $info['view modes'];
    }
    else {
      $params = array('%type' => $this->type);
      throw new iwException(t('Unable to find the referenced entity type %type', $params));
    }
  }

  /**
   * (non-PHPdoc)
   * @see IEntityAttacher::getKey()
   */
  function getKey() {
    return 'fieldwrapper';
  }
  
  /**
   * (non-PHPdoc)
   * @see IEntityAttacher::attach()
   */
  function attach(array $template, $force = FALSE) {    
    foreach ($template as $view => $wrappers) {
      if (empty($this->view_modes[$view])) {
        continue; // skip if view mode doesn't match.
      }
      
      $scope = iwFieldGroupController::GetScope($this->type, $this->bundle, $view);
      $current = iwFieldGroupController::LoadContainers($this->type, $this->bundle, $view);

      foreach ($wrappers as $name => $wrapper) {
        $wrapperName = preg_replace('/^wrapper_/', '', $wrapper['name']);

        if ($force || !isset($current[$wrapperName])) {
          $wrapperClass = $wrapper['class_name'];          
          $wrapObject = new $wrapperClass(
            'iwFieldGroupController',
            array('scope' => $scope, 'name' => $wrapperName) + $wrapper
          );

          $wrapObject->save();
        }
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::export()
   */
  function export() {
    $retval = array();
    
    // Run through each of the view modes and record the groups present.
    foreach ($this->view_modes as $mode => $info) {
      $wrappers = iwFieldGroupController::LoadContainers($this->type, $this->bundle, $mode);
      
      if (!empty($wrappers) && !empty($this->values[$mode])) {
        foreach (array_intersect_key($wrappers, $this->values[$mode]) as $key => $wrapper) {
          $retval[$mode][$key] = (array)$wrapper;
          $retval[$mode][$key]['class_name'] = get_class($wrapper);
          
          unset($retval[$mode][$key]['controller']);
          unset($retval[$mode][$key]['scope']);
        }
      }
    }
    return $retval;
  }

  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::allowedValues()
   */
  function allowedValues() {
    $options = array();
    
    // Run through each of the view modes and record the groups present.
    foreach ($this->view_modes as $mode => $info) {
      if (isset($info['custom settings']) && empty($info['custom settings'])) {
        continue;
      }
      
      $wrappers = iwFieldGroupController::LoadContainers($this->type, $this->bundle, $mode);
      
      // Generate a key of options.
      if (!empty($wrappers)) {
        $options[$mode] = _iwapi_extract_field($wrappers);
      }
    }
    return $options;
  }
  
  
  /**
   * (non-PHPdoc)
   * @see iwEntityAttacher::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    foreach ($this->allowedValues() as $mode => $opts) {
      $element[$mode] = array(
        '#type' => 'checkboxes',
        '#title' => t($this->view_modes[$mode]['label']),
        '#options' => $opts,
        '#default_value' => isset($this->values[$mode]) ? $this->values[$mode] : array(),
      );
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntityAttacher::submitOptions()
   */
  function submitElements(array $element, array &$values, array &$form_state) {    
    // Use the options to update the state of these elements.
    $this->values = array_map('array_filter', $values);
    $this->values = array_filter($this->values);
  }
}