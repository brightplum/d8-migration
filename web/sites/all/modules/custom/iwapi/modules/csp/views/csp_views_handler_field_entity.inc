<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

class csp_views_handler_field_entity extends views_handler_field {

  /**
   * Constructor; calls to base object constructor.
   */
  function construct() {
    parent::construct();
    
    $this->additional_fields['entity_type'] = 'entity_type';
  }
  
  /**
   * (non-PHPdoc)
   * @see views_handler_field::option_definition()
   */
  function option_definition() {
    $options = parent::option_definition();
    
    $default = $this->definition + array('link_to_entity default' => FALSE);
    $options['link_to_entity'] = array(
      'default' => $default['link_to_entity default'],
      'bool' => TRUE,
    );

    return $options;
  }
  
  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_entity'] = array(
      '#title' => t('Link this field to the original entity'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_entity']),
    );
  
    parent::options_form($form, $form_state);
  }
  
  /**
   * (non-PHPdoc)
   * @see views_handler_field::render()
   */
  function render($values) {
    $value = $this->get_value($values);
    $type  = $this->get_value($values, 'entity_type');
    
    $entities = entity_load($type, array($value));
    if ($entity = reset($entities)) {      
      if (!empty($this->options['link_to_entity'])) {
        $uri = entity_uri($type, $entity);
        
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = $uri['path'];
        
        $allowed = array('fragment' => TRUE, 'query' => TRUE);
        $this->options['alter'] += array_intersect_key($uri['options'], $allowed);
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
      
      // Return a sanitized label for this entity.
      return $this->sanitize_value(entity_label($type, $entity));
    }
    
    return t('Unknown');
  }
}
