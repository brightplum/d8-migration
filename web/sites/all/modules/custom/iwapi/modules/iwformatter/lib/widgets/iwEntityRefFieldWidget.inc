<?php
/**
 * @file
 * Contains the iwEntityReferenceWidget definition
 */


/**
 * 
 */
class iwEntityRefFieldWidget implements iwIFieldWidget {  
  
  // ------------------------
  // Instance variables
  // ------------------------

  protected $field;
  protected $instance;
  
  
  // ------------------------
  // Instance method
  // ------------------------
  
  function __construct(array $field, array $instance) {
    $this->field    = $field;
    $this->instance = $instance;
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIFieldWidget::getSettingsForm().
   */
  public function getSettingsForm() {
    $widget = $this->instance['widget'];
    $settings = $widget['settings'] + field_info_widget_settings($widget['type']);

    $form = array();
    $form['match_operator'] = array(
      '#type' => 'select',
      '#title' => t('Autocomplete matching'),
      '#default_value' => $settings['match_operator'],
      '#options' => array(
        'STARTS_WITH' => t('Starts with'),
        'CONTAINS' => t('Contains'),
      ),
      '#description' => t('Select the method used to collect autocomplete suggestions. Note that <em>Contains</em> can cause performance issues on sites with thousands of nodes.'),
    );
    
    $form['size'] = array(
      '#type' => 'textfield',
      '#title' => t('Size of textfield'),
      '#default_value' => $settings['size'],
      '#element_validate' => array('_element_validate_integer_positive'),
      '#required' => TRUE,
    );
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIFieldWidget::getWidgetForm().
   */
  public function getWidgetForm(array &$form, array &$form_state, $langcode, array $items, $delta, $element) {
    $field = $this->field;
    $instance = $this->instance;

    // Ensure that the entity target type exists before displaying the widget.
    $entity_info = entity_get_info($field['settings']['target_type']);
    if (empty($entity_info)){
      return;
    }

    $entity_type = $instance['entity_type'];
    $entity = isset($element['#entity']) ? $element['#entity'] : NULL;
    $handler = entityreference_get_selection_handler($field, $instance, $entity_type, $entity);

    // We let the Field API handles multiple values for us, only take
    // care of the one matching our delta.
    $items = isset($items[$delta]) ? array($items[$delta]) : array();

    $entity_ids = array();
    $entity_labels = array();

    // Build an array of entities ID.
    foreach ($items as $item) {
      $entity_ids[] = $item['target_id'];
    }

    // Load those entities and loop through them to extract their labels.
    $entities = entity_load($field['settings']['target_type'], $entity_ids);

    foreach ($entities as $entity_id => $entity_item) {
      $label = $handler->getLabel($entity_item);
      $key = "$label ($entity_id)";
      // Labels containing commas or quotes must be wrapped in quotes.
      if (strpos($key, ',') !== FALSE || strpos($key, '"') !== FALSE) {
        $key = '"' . str_replace('"', '""', $key) . '"';
      }
      $entity_labels[] = $key;
    }

    // Prepare the autocomplete path.
    $autocomplete_path = 'entityreference/autocomplete/single';
    $autocomplete_path .= '/' . $field['field_name'] . '/' . $instance['entity_type'] . '/' . $instance['bundle'] . '/';
    
    // Use <NULL> as a placeholder in the URL when we don't have an entity.
    // Most webservers collapse two consecutive slashes.
    $id = 'NULL';
    if ($entity) {
      list($eid) = entity_extract_ids($entity_type, $entity);
      if ($eid) {
        $id = $eid;
      }
    }
    $autocomplete_path .= $id;
    
    $element += array(
      '#type' => 'textfield',
      '#maxlength' => 1024,
      '#default_value' => implode(', ', $entity_labels),
      '#autocomplete_path' => $autocomplete_path,
      '#size' => $instance['widget']['settings']['size'],
      '#element_validate' => array('_entityreference_autocomplete_validate'),
    );
    
    $uris = iwapi_get_entity_ajax_uri();
    $target_type = $field['settings']['target_type'];

    if (key_exists($target_type, $uris)) {
      static $sent = array();
        
      $js_settings['iwformatter']['entityRefWidgets'] = array(
        $field['field_name'] => $uris[$target_type] + array(
          'bundles' => $field['settings']['handler_settings']['target_bundles'],
        ),
      );

      if (!isset($sent[$target_type]) && isset($uris['bundle_info_callback']) && function_exists($uris['bundle_info_callback'])) {
        $js_settings['iwformatter']['entities'][$target_type] = $uris['bundle_info_callback']();
        $sent[$target_type];
      }

      drupal_add_js($js_settings, 'setting');
    }
    
    $mod_dir = drupal_get_path('module', 'iwformatter');
    
    // Include the JS functionality to create popups and be more interactive.
    $element['#attached']['library'][] = array('iwapi', 'iwapi.ajax');
    $element['#attached']['js'][] = $mod_dir .'/js/entity-ref-widget.js';
    $element['#attached']['css'][] = $mod_dir .'/theme/css/ref-widget.css';
    
    return array('target_id' => $element);
  }
}
