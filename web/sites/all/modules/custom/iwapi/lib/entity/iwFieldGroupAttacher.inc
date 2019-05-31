<?php

/**
 * Entity attacher responsible for managing groups
 * from the field_group module for import, export
 * and attaching to entities.
 */
class iwFieldGroupAttacher extends iwEntityAttacher {
  
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
  	return 'fieldgroup';
  }
  
  /**
   * (non-PHPdoc)
   * @see IEntityAttacher::attach()
   */
  function attach(array $template, $force = FALSE) {    
    if (!empty($template)) {
      $id_array = array('', $this->type, $this->bundle, '');

      foreach ($template as $view => $groups) {
        if (empty($this->view_modes[$view])) {
          continue;
        }
        
        $id_array[3] = $view;
        foreach ($groups as $group) {
          $id_array[0] = $group->group_name;
          $group->identifier  = implode('|', $id_array);
          $group->mode        = $view;
          $group->entity_type = $this->type;
          $group->bundle      = $this->bundle;
          
          // See if we are overwriting a previous value or not.
          if ($previous = field_group_load_field_group($group->group_name, $this->type, $this->bundle, $view)) {
            // We only want to overwrite previous settings if the $force parameter is set
            if (!$force) {
              return;
            }
            $group->id          = $previous->id;
            $group->export_type = $previous->export_type;
          }
          else {
            unset($group->id);
            unset($group->export_type);
          }
          field_group_group_save($group);
        }
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::allowedValues()
   */
  function allowedValues() {
    $options = array();
    
    // Run through each of the view modes and record the groups present.
    foreach ($this->view_modes as $mode => $info) {
      $groups = field_group_info_groups($this->type, $this->bundle, $mode);
      
      // Generate a key of options.
      if (!empty($groups)) {
        $group_names = array_keys($groups);
        $options[$mode] = array_combine($group_names, $group_names);
      }
    }
    return $options;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::export()
   */
  function export() {
    $retval = array();
    
    // Run through each of the view modes and record the groups present.
    foreach ($this->view_modes as $mode => $info) {
      
      // Get the group definitions for this view
      $groups = field_group_info_groups($this->type, $this->bundle, $mode);
      if (!empty($groups) && !empty($this->values[$mode])) {
        $retval[$mode] = array_intersect_key($groups, $this->values[$mode]);
     
        // Clean out identifiers that would tie this group to a particular instance.
        foreach ($retval[$mode] as &$def) {
          unset($def->id, $def->identifier);
          unset($def->entity_type, $def->bundle);
          unset($def->mode, $def->export_type);
        }
        unset($def);
      }
    }
    return $retval;
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
  }
}