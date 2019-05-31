<?php
/**
 * @file
 * Views handler to rendering entities as a field in views. Will default to rendering the title.
 */


class iwapi_views_handler_field_entity extends views_handler_field {
    
  protected $entityType;
  protected $entityKeys;
  
  /**
   * (non-PHPdoc)
   * @see views_handler_field::init()
   */
  function init(&$view, &$options) {
    parent::init($view, $options);

    // Initialize the entity-type used.
    $table_data = views_fetch_data($this->view->base_table);
    $this->entityType = $table_data['table']['entity type'];
    
    $info = entity_get_info($this->entityType);
    $this->entityKeys = $info['entity keys'];
    
    // If linking to 
    if (!empty($this->options['link_to_entity'])) {
      $idKey = $this->entityKeys['id'];
      
      $this->additional_fields[$idKey] = array(
        'table' => $info['base table'],
        'field' => $idKey,
      );
    }
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
    $data = $this->sanitize_value($this->get_value($values));
 
    $idKey = $this->entityKeys['id'];
    if (!empty($this->options['link_to_entity']) && !empty($this->additional_fields[$idKey])) {
      if (!empty($data)) {
        $entities = entity_load($this->entityType, array($this->get_value($values, $idKey)));
        $entity = reset($entities);
        $uri = entity_uri($this->entityType, $entity);
        
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = $uri['path'];
        
        foreach (array('fragment', 'query') as $opt) {
          if (isset($uri['options'][$opt])) {
            $this->options['alter'][$opt] = $uri['options'][$opt];
          }
        }
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
    }
    return $data;
  }
}
