<?php
/**
 * @file
 * 
 */

class iwConditionalFieldWrapper extends iwContentWrapper {
 
  public $settings = array(
    'display_title' => 'visible',
    'span' => -1,
    'equal_heights' => FALSE,
    'cond_field' => '',
    'invert_cond' => FALSE, 
    'attributes' => array('class' => array()),
  );
  
  /**
   * (non-PHPDOC)
   * @see iwContentWrapper::transferItems()
   */
  function transferItems(array &$element, array &$dst) {
    list($type, $bundle, $view) = explode('|', $this->scope);

    $cond = $this->settings['cond_field'];
    if ($view != 'form' && isset($element['#'. $type]) && !empty($cond)) {
      $entity = $element['#'. $type];
      $items = field_get_items($type, $entity, $cond);
      
      if (!(empty($items) && empty($this->settings['invert_cond']))) {
        unset($element['wrapper_'. $this->name]);
        
        foreach ($this->children as $child) {
          unset($element[$child]);
        }
        
        return;
      }
    }
    
    parent::transferItems($element, $dst);
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIConfigurable::appendElements().
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $controller = $this->controller;
    list($type, $bundle) = $controller::ParseScope($this->scope);
    $fields = field_info_instances($type, $bundle);
    
    if (!empty($fields)) {
      $element['cond_field'] = array(
        '#type' => 'select',
        '#title' => t('Conditional field'),
        '#options' => _iwapi_extract_field($fields),
        '#default_value' => $this->settings['cond_field'],
        '#description' => t('The contents of this wrapper are visible when specified field is empty.'),
      );
      
      $element['invert_cond'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show when conditional field not empty'),
        '#default_value' => $this->settings['invert_cond'],
      );
    }
  }
}