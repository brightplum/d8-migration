<?php

/**
 * Attaches / Exports the extra fields display
 * properties and settings. Extra fields are
 * provided by HOOK_extra_fields().
 */
class iwFieldExtraAttacher implements iwIEntityAttacher {

  protected $type;
  protected $bundle;
  
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
   * (non-PHPdoc)
   * @see IEntityAttacher::attach()
   */
  function attach(array $template, $force = FALSE) {
    $avail = field_bundle_settings($this->type, $this->bundle);
    
    $extra = &$avail['extra_fields'];
    $values = $template;
    
    if (isset($template['form']) && is_array($template['form'])) {
      $current       = field_info_extra_fields($this->type, $this->bundle, 'form');
      $extra['form'] = array_intersect_key($template['form'], $current);
    }
    
    if (isset($template['display']) && is_array($template['display'])) {
      $current          = field_info_extra_fields($this->type, $this->bundle, 'display');
      $extra['display'] = array_intersect_key($template['display'], $current);
      
      // Check for custom display modes, and turn on the ones included in the template.
      if ($force && $entity_info = entity_get_info($this->type)) {
        $display_settings = array();
        foreach ($template['display'] as $fieldname => $settings) {
          $display_settings += $settings;
        }
        
        $view_modes = $entity_info['view modes'];
        foreach (array_keys($display_settings) as $mode_name) {
          if (!empty($view_modes[$mode_name])) {
            $avail['view_modes'][$mode_name]['custom_settings'] = TRUE;
          }
        }
      }
    }
    field_bundle_settings($this->type, $this->bundle, $avail);
  }

  /**
   * (non-PHPdoc)
   * @see iwIEntityAttacher::export()
   */
  function export() {
    $extras = field_bundle_settings($this->type, $this->bundle);
    return is_array($extras) ? $extras['extra_fields'] : array();
  }
}
