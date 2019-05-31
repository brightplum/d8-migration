<?php
/**
 * @file
 * Contains the functions specific to the field display UI settings.
 */

require_once dirname(__DIR__) .'/includes/builder_form.inc';


/**
 * Form builder to create an alternative field management form.
 */
function iwformatter_entity_display_form($form, &$form_state, $entity_type, $bundle = NULL, $view_mode = "default") {
  $entity_info = entity_get_info($entity_type);
  $bundle_name = field_extract_bundle($entity_type, $bundle);
  
  // Ensure some helper functions are avaiable.
  form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');
  field_ui_inactive_message($entity_type, $bundle_name);

  // Ensure that the form state is properly initialized.
  if (!isset($form_state['content_fields'])) {
    $form_state['content_fields'] = $entity_info['fieldable']
      ? _iwformatter_get_entity_fields($entity_type, $bundle_name) : array();
  
    $form_state['content_wrappers'] = iwFieldGroupController::LoadContainers($entity_type, $bundle_name, $view_mode);
  }  
  $form_state['entity_type'] = $entity_type;
  $form_state['bundle_name'] = $bundle_name;
  $form_state['view_mode']   = $view_mode;
  
  // Generate the content builder form
  $callbacks = array(
    'field_callback' => '_iwformatter_builder_fields_display',
    'field_post_callback' => '_iwformatter_builder_display_extra_fields',
    'context' => array(
      'entity_type' => $entity_type,
      'bundle_name' => $bundle_name,
      'view_mode' => $view_mode,
      'base_uri' => _field_ui_bundle_admin_path($entity_type, $bundle_name),
      'form' => $form,
      'form_state' => &$form_state,
    ),
  );
  
  // Are we configuring any of the fields?
  $configuring = empty($form_state['configuring_item']) ? array(NULL) : $form_state['configuring_item'];
  $callbacks['context']['configuring_field'] = ($configuring[0] === 'fields') ? $configuring[1] : NULL;
  
  $form = _iwformatter_create_content_builder_form($form_state, $callbacks, TRUE);
  
  // -----------------------------
  // Final configuration options.
  // -----------------------------
  $form['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array(
      'id' => 'form-wide-actions',
      'class' => array('iwapi-dockable', 'dock-bottom-attach'),
    ),
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('_iwformatter_entity_display_submit'),
      '#valdiate' => array(),
      '#limit_validation_errors' => array(
        array('grid_system'),
        array('fields'),
        array('containers')
      ),
    ),
  );
  
  return $form;
}

/**
 * Form builder submit function to alter the ordering
 * and make changes to formatters permanent.
 */
function _iwformatter_entity_display_submit($form, &$form_state) {
  $entity_type = $form_state['entity_type'];
  $bundle      = $form_state['bundle_name'];
  $view_mode   = $form_state['view_mode'];
  
  $pool = $form_state['values']['fields'];
  $fields = $form_state['content_fields'];
  $wrappers = $form_state['content_wrappers'];
  
  // save the grid settings
  variable_set("iwformatter_{$entity_type}_{$bundle}_grid", intval($form_state['values']['grid_system']));
  
  // Update the field listing / folding.
  $bundle_settings = field_bundle_settings($entity_type, $bundle);
  $extra = &$bundle_settings['extra_fields']['display'];

  foreach ($form_state['values']['fields'] as $key => $data) {
    if (key_exists($key, $fields)) {
      if (empty($data['parent'])) {
        $fields[$key]['instance']['display'][$view_mode]['type'] = 'hidden';
        unset($fields[$key]['instance']['display'][$view_mode]['module']);
      }
      else {
        // Ensure the correct field formatter is set.
        $fields[$key]['instance']['display'][$view_mode]['type'] = $data['formatter'];
      }

      $fields[$key]['instance']['display'][$view_mode]['weight'] = $data['weight'];
      field_update_instance($fields[$key]['instance']);
    }
    else {
      $extra[$key][$view_mode]['visible'] = !empty($data['parent']);
      $extra[$key][$view_mode]['weight']  = $data['weight'];
    }
  }

  // Save changes to the extra fields all at once.
  field_bundle_settings($entity_type, $bundle, $bundle_settings);

  // Update the container wrapping
  if (!empty($form_state['values']['containers'])) {
    $pool += $form_state['values']['containers'];

    $containers = $form_state['values']['containers'];
    foreach ($wrappers as $name => $wrapper) {
      $wrapper->children = array(); // reset the wrapper data
      $wrapper->weight = $containers[$name]['weight'];
    }

    uasort($pool, 'drupal_sort_weight');
    foreach ($pool as $name => $data) {
      if (isset($wrappers[$data['parent']])) {
        $wrappers[$data['parent']]->children[] = $name;
      }
    }
  }  

  foreach ($wrappers as $name => $wrapper) {
    $wrapper->save();
  }

  drupal_set_message(t("Field display settings were saved."), 'info');
}

/**
 * Callback to generate the form option elements.
 */
function _iwformatter_builder_fields_display($field, $instance, $elements, $context) {
  $fieldname = $field['field_name'];
  $view_mode = $context['view_mode'];
  $display   = $instance['display'][$view_mode];
  $field_id  = strtr($fieldname, '_', '-');
  
  // Create the actual field configurations shell. 
  $form = array(
    'label' => array(
      '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
      '#markup' => '<div class="item-name"><span class="item-label">'. check_plain($instance['label']) .'</span>'
        ." <small>({$fieldname})</small></div>",
    ),
    'formatter' => array(
      '#type' => 'select',
      '#suffix' => '</div>',
      '#title' => t('Display: '),
      '#options' => field_ui_formatter_options($field['type']),
      '#default_value' => $display['type'],
      '#ajax' => array(
        'callback' => '_iwformatter_builder_update_field_display_ajax',
        'trigger_as' => array('name' => $field_id .'-update-btn'),
      ),
    ),
    'update' => array(
      '#type' => 'submit',
      '#name' => $field_id .'-update-btn',
      '#value' => t('Update Formatter'),
      '#attributes' => array('style' => 'display:none;'),
      '#submit' => array('_iwformatter_builder_update_display_formatter_submit'),
      '#limit_validation_errors' => array(array('fields', $fieldname, 'formatter')),
      '#ajax' => array(
        'callback' => '_iwformatter_builder_update_field_display_ajax',
      ),
    ),
    'configure' => array(
      '#type' => 'submit',
      '#name' => $field_id,
      '#value' => html_entity_decode('&#xf013;', 0, 'UTF-8'),
      '#attributes' => array('class' => array('fa', 'draggable-field-op')),
      '#submit' => array('_iwformatter_builder_set_configure_submit'),
      '#validate' => array(),
      '#limit_validation_errors' => array(),
      '#ajax' => array('callback' => '_iwformatter_builder_set_configure_ajax'),
    ),
    'summary' => array(
      '#type' => 'container',
      '#attributes' => array('class' => array('field-summary')),
    ),
    'settings' => array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'id' => $field_id .'-settings',
        'class' => array('content-wrapper-settings', 'clearfix'),
      ),
    ),
  ) + $elements;
  
  // ----------------------------------------------
  // Start building summary and configuration form.
  $form['#attributes']['id'] = $field_id .'-display';
  $form['weight']['#default_value'] = $display['weight'];
  
  if ($display['type'] == 'hidden') {
    $form['parent']['#default_value'] = ''; // set it to the unused region (or hidden).
    
    $field_type = field_info_field_types($field['type']);
    $formatter  = field_info_formatter_types($field_type['default_formatter']);
    
    $display['type']     = $field_type['default_formatter'];
    $display['module']   = $formatter['module'];
    $display['settings'] += isset($formatter['settings']) ? $formatter['settings'] : array();
  }

  $summary = module_invoke($display['module'], 'field_formatter_settings_summary', $field, $instance, $view_mode);
  $hook_context = array('field' => $field, 'instance' => $instance, 'view_mode' => $view_mode);

  foreach (module_implements('field_formatter_settings_summary_alter') as $module) {
    $func = $module .'_field_formatter_settings_summary_alter';
    $func($summary, $hook_context);
  }
  $form['summary']['summary']['#markup'] = $summary;

  // Is this being configured?
  if ($context['configuring_field'] === $fieldname) {
    $settings = array();  

    $function = $display['module'] . '_field_formatter_settings_form';
    if (function_exists($function)) {
      $settings = $function($field, $instance, $view_mode, $context['form'], $context['form_state']);
    }

    $settings['label_display'] = array(
      '#type' => 'select',
      '#title' => t('Label display'),
      '#weight' => -15,
      '#options' => array(
        'above' => t('Above'),
        'inline' => t('Inline'),
        'hidden' => t('<Hidden>'),
      ),
      '#default_value' => $display['label'],
    );

    foreach (module_implements('field_formatter_settings_form_alter') as $module) {
      $func = $module .'_field_formatter_settings_form_alter';
      $func($settings, $hook_context);
    }

    $form['settings'] += $settings;
    $form['settings']['actions'] = array(
      '#type' => 'actions',
       'settings_save' => array(
        '#type' => 'submit',
        '#name' => 'configuring-element-save',
        '#value' => t('Save Changes'),
        '#submit' => array(
          '_iwformatter_builder_configure_display_submit',
          '_iwformatter_builder_cancel_configure_submit',
        ),
        '#validate' => array('_iwformatter_builder_configure_wrapper_validate'),
        '#limit_validation_errors' => array(array('fields', $fieldname, 'settings')),
        '#ajax' => array('callback' => '_iwformatter_builder_update_field_display_ajax'),
      ),
      'settings_cancel' => array(
        '#type' => 'submit',
        '#name' => 'configuring-element-cancel',
        '#value' => t('Cancel'),
        '#attributes' => array('class' => array('cancel-action')),
        '#submit' => array('_iwformatter_builder_cancel_configure_submit'),
        '#validate' => array(),
        '#limit_validation_errors' => array(),
        '#ajax' => array('callback' => '_iwformatter_builder_set_configure_ajax'),
      ),
    );
  }
  
  return $form;
}

/**
 * Form submit callback to update the instance formatter type.
 */
function _iwformatter_builder_update_display_formatter_submit($form, &$form_state) {
  $view_mode = $form_state['view_mode']; 
  $trigger   = $form_state['triggering_element'];
  
  $parents = $trigger['#parents'];
  array_pop($parents);
  
  $fieldname = array_pop($parents);
  $formatter = $form_state['values']['fields'][$fieldname]['formatter'];
  $instance  = &$form_state['content_fields'][$fieldname]['instance'];
  
  $formatter_info = field_info_formatter_types($formatter);
  $instance['display'][$view_mode]['type']     = $formatter;
  $instance['display'][$view_mode]['module']   = $formatter_info['module'];
  $instance['display'][$view_mode]['settings'] += $formatter_info['settings'];
  
  // Rebuild the form changes and ensure we update settings.
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit callback to update the display configurations 
 */
function _iwformatter_builder_configure_display_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  
  $configuring = $form_state['configuring_item'];
  if ($configuring[0] === 'fields') {
    $fieldname = $configuring[1];
    $view_mode = $form_state['view_mode'];

    $instance = &$form_state['content_fields'][$fieldname]['instance'];
    $values = $form_state['values']['fields'][$fieldname]['settings'];
    
    $instance['display'][$view_mode]['label'] = $values['label_display'];
    unset($values['actions'], $values['label_display']);
    
    $instance['display'][$view_mode]['settings'] = $values;
  }
}


/**
 * Callback to add all the extra fields after the regular
 * instance fields have already been included.
 * 
 * @param array $field_form
 *  Reference to the current $form['fields'] section of the form.
 * @param array $cmn_elements
 *  Array definition of common elements used by fields in the builder
 *  (parent and weight field information).
 * @param array $context
 *  Contextual information about the form being built.
 */
function _iwformatter_builder_display_extra_fields(&$field_form, $cmn_elements, $context) {
  $extras = field_info_extra_fields($context['entity_type'], $context['bundle_name'], 'display');
  $mode   = $context['view_mode'];
  
  foreach ($extras as $key => $data) {
    $field_form[$key] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('draggable-field', 'field-extra')),
      
      'item-handle' => array(
        '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
        '#suffix' => '</div>',
        '#markup' => check_plain($data['label']),
      ),
    ) + $cmn_elements;
    
    if (empty($data['display'][$mode]['visible'])) {
      $field_form[$key]['parent']['#default_value'] = '';
    }
    $field_form[$key]['weight']['#default_value'] = $data['display'][$mode]['weight'];
  }
}

/**
 * AJAX callback to update the field display changes
 * from the display UI form.
 */
function _iwformatter_builder_update_field_display_ajax($form, $form_state) {
  $retval = _iwformatter_builder_set_configure_ajax($form, $form_state);
  
  $parents = $form_state['triggering_element']['#array_parents'];
  list($type, $fieldname) = array_splice($parents, 0, 2);
  
  if (isset($form[$type][$fieldname])) {
    $instance = $form_state['content_fields'][$fieldname]['instance'];
    $summary  = $form[$type][$fieldname]['summary'];
      
    $retval['#commands'][] = array(
      'command' => 'updateDraggableItem',
      'selector' => '#'. strtr($fieldname, '_', '-') .'-display',
      'label' => check_plain($instance['label']),
      'span' => -1,
      'summary' => render($summary),
    );
  }
  
  return $retval;
}