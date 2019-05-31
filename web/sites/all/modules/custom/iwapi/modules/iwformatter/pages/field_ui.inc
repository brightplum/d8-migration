<?php
/**
 * @file
 * Contains alternative field-ui form builders.
 */

require_once dirname(__DIR__) .'/includes/builder_form.inc';


/**
 * Form builder to create an alternative field management form.
 */
function iwformatter_entity_field_form($form, &$form_state, $entity_type, $bundle = NULL) {
  $entity_info = entity_get_info($entity_type);
  $bundle_name = field_extract_bundle($entity_type, $bundle);
  
  // Cover some required messaging.
  form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');
  field_ui_inactive_message($entity_type, $bundle_name);

  // Ensure that the form state is properly initialized.
  if (!isset($form_state['content_fields'])) {
    $form_state['content_fields'] = $entity_info['fieldable']
      ? _iwformatter_get_entity_fields($entity_type, $bundle_name) : array();
  
    $form_state['content_wrappers'] = iwFieldGroupController::LoadContainers($entity_type, $bundle_name, 'form');
  }
  
  $form_state['entity_type'] = $entity_type;
  $form_state['bundle_name'] = $bundle_name;
  $form_state['view_mode']   = 'form';
  
  $field_info_types = _field_info_collate_types();  
  $field_types  = $field_info_types['field types'];
  $widget_types = $field_info_types['widget types'];
  
  // Generate the content builder form
  $callbacks = array(
    'field_callback' => '_iwformatter_builder_fields_settings',
    'field_post_callback' => '_iwformatter_builder_form_extra_fields',
    'tools_callback' => '_iwformatter_builder_fields_tools',
    'context' => array(
      'entity_type' => $entity_type,
      'bundle_name' => $bundle_name,
      'base_uri' => _field_ui_bundle_admin_path($entity_type, $bundle_name),
      'field_types' => $field_types,
      'widget_types' => $widget_types,
    ),
  );
  
  $form = _iwformatter_create_content_builder_form($form_state, $callbacks, false);
  
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
      '#submit' => array('_iwformatter_entity_field_submit'),
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
 * Form submit which saves the ordering and different
 * settings for the content wrappers.
 */
function _iwformatter_entity_field_submit($form, &$form_state) {
  $entity_type = $form_state['entity_type'];
  $bundle = $form_state['bundle_name'];
  
  $fields = $form_state['content_fields'];
  $wrappers = $form_state['content_wrappers'];
  
  // Save the grid system updated.
  variable_set("iwformatter_{$entity_type}_{$bundle}_grid", intval($form_state['values']['grid_system']));
  
  $pool = array();
  if (!empty($form_state['values']['fields'])) {
    $pool += $form_state['values']['fields'];
    
    // Update the field listing / folding.
    $bundle_settings = field_bundle_settings($entity_type, $bundle);

    foreach ($form_state['values']['fields'] as $key => $data) {
      if (key_exists($key, $fields)) {
        $fields[$key]['instance']['widget']['weight'] = $data['weight'];
        field_update_instance($fields[$key]['instance']);
      }
      else {
        $bundle_settings['extra_fields']['form'][$key]['weight'] = $data['weight'];
      }
    }
    // Save changes to the extra fields all at once.
    field_bundle_settings($entity_type, $bundle, $bundle_settings);
  }
  
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
  
  // Save the changes to each of the content wrappers.
  foreach ($wrappers as $name => $wrapper) {
    $wrapper->save();
  }

  drupal_set_message(t("Field overview settings were saved."), 'info');
}

/**
 * Callback function used generate the fields setting
 * form elements. These appear on the field settings page.
 * 
 * @param array $field
 *  Field info array, which contains all the data and
 *  settings about a field.
 * @param array $instance
 *  Instance settings and configurations for this field.
 * @param array $containers
 *  A keyed array with all the currently available object parents.
 * @param type $context
 *  An array of current parameters for use in the scope of these settings.
 * @return array
 *  Form array containing the field settings.
 */
function _iwformatter_builder_fields_settings($field, $instance, $elements, $context) {
  $field_uri   = "{$context['base_uri']}/fields/{$field['field_name']}";
  $field_type  = $context['field_types'][$field['type']];
  $widget_type = $context['widget_types'][$instance['widget']['type']];
  
  $form = array(
    'label' => array(
      '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
      '#suffix' => '</div>',
      '#markup' => check_plain($instance['label']) ." <small>({$field['field_name']})</small>",
    ),
    'op' => array(
      '#theme' => 'links',
      '#attributes' => array('class' => 'field-actions'),
      '#links' => array(
        'edit' => array('title' => 'edit', 'href' => $field_uri),
        'delete' => array('title' => 'delete', 'href' => $field_uri .'/delete'),
      ),
    ),
  ) + $elements;
  
  $form['label']['#markup'] .= '<div class="field-properties">';
  $form['label']['#markup'] .= '<div class="field-property"><span class="property-label">Type:</span> '. l($field_type['label'], "{$field_uri}/field-settings") .'</div>';
  $form['label']['#markup'] .= '<div class="field-property"><span class="property-label">Widget:</span> '. l($widget_type['label'], "{$field_uri}/widget-type") .'</div>';
  $form['label']['#markup'] .= '</div>';
  
      
  $form['weight']['#default_value'] = $instance['widget']['weight'];
  return $form;
}


function _iwformatter_builder_form_extra_fields(&$field_form, $cmn_elements, $context) {
  $extras = field_info_extra_fields($context['entity_type'], $context['bundle_name'], 'form');
  
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
    
    $field_form[$key]['weight']['#default_value'] = $data['weight'];
  }
}

/**
 * Creates the form elements which are used
 * to generate new fields / or container elements.
 * 
 * @param array $form_state
 *  Current state of the form, and values.
 * @param array $context
 *  Current context of information use to 
 *  build the toolbar.
 * @return array
 *  array of form elements that represent
 *  the tool bar for this builder. Add elements
 *  relevant to this builder.
 */
function _iwformatter_builder_fields_tools(&$form_state, $context) {
  $entity_type = $context['entity_type'];
  $bundle_name = $context['bundle_name'];
    
  $field_options = field_ui_field_type_options();
  $widget_options = field_ui_widget_type_options(NULL);
 
  // Expose the form values.
  $values = isset($form_state['values']) ? $form_state['values'] : array();
  
  $new_widgets = array('- Select a Widget -');
  if (!empty($values['new_field']['type'])) {
    $new_widgets += $widget_options[$values['new_field']['type']];
  }
  
  $tools['new_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new field'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    
    'label' => array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#size' => 15,
    ),
    'field_name' => array(
      '#type' => 'machine_name',
      '#field_prefix' => '<span dir="ltr">field_',
      '#field_suffix' => '</span>&lrm;',
      '#size' => 15,
      '#maxlength' => 26,
      '#machine_name' => array(
        'label' => t('Field name'),
        'exists' => '_field_ui_field_name_exists',
        'source' => array('tools', 'new_field', 'label'),
        'standalone' => TRUE,
      ),
    ),
    'type' => array(
      '#type' => 'select',
      '#title' => t('Field type'),
      '#options' => $field_options,
      '#ajax' => array(
        'wrapper' => 'new-field-widget',
        'callback' => '_iwapi_ajax_configurable_callback',
        'ref_element' => array(
          'type' => 'sibling',
          'value' => array('widget'),
        ),
      ),
    ),
    'widget' => array(
      '#prefix' => '<div id="new-field-widget">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#options' => $new_widgets,
    ),
    'actions' => array(
      '#type' => 'actions',
      
      'add_field' => array(
        '#type' => 'submit',
        '#value' => t('Add New Field'),
        '#submit' => array(
          '_iwformatter_entity_field_submit',
          '_iwformatter_ui_add_new_field_submit',
        ),
        '#validate' => array('_iwformatter_ui_add_new_field_validate'),
        '#limit_validation_errors' => array(
          array('new_field'),
          array('grid_system'),
          array('fields'),
          array('containers'),
        ),
      ),
    ),
  );
  
  $existing = field_ui_existing_field_options($entity_type, $bundle_name);
  if ($existing && $widget_options) {
    $existing_opts = array('- Select Existing Field -');
    $existing_widgets = array('- Select Widget -');
    
    foreach ($existing as $fieldname => $data) {
      $existing_opts[$data['type_label']][$fieldname] = "$fieldname ({$data['label']})";
    }
    
    if (!empty($values['existing_field']['field_name'])) {
      $existing_field = $values['existing_field']['field_name'];
      $existing_widgets += $widget_options[$existing[$existing_field]['type']];
    }
    
    $tools['existing_field'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add existing field'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      
      'label' => array(
        '#type' => 'textfield',
        '#title' => t('Label'),
        '#size' => 15,
      ),
      'field_name' => array(
        '#type' => 'select',
        '#options' => $existing_opts,
        '#ajax' => array(
          'wrapper' => 'existing-field-widget',
          'callback' => '_iwapi_ajax_configurable_callback',
          'ref_element' => array(
            'type' => 'sibling',
            'value' => array('widget'),
          ),
        ),
      ),
      'widget' => array(
        '#prefix' => '<div id="existing-field-widget">',
        '#suffix' => '</div>',
        '#type' => 'select',
        '#options' => $existing_widgets,
        '#disabled' => empty($existing_field),
      ),
      
      'actions' => array(
        '#type' => 'actions',
 
        'add_existing' => array(
          '#type' => 'submit',
          '#name' => 'iwformatter-add-existing-submit',
          '#value' => t('Add Existing Field'),
          '#limit_validation_errors' => array(
            array('existing_field'),
            array('grid_system'),
            array('fields'),
            array('containers'),
          ),
          '#validate' => array('_iwformatter_ui_add_existing_field_validate'),
          '#submit' => array(
            '_iwformatter_entity_field_submit',
            '_iwformatter_ui_add_existing_field_submit',
          ),
        ),
      ),
    );
  }
  
  return $tools;
}


// ==========================
// Handle add new field
// ==========================

/**
 * Form validation callback to create a new field and field instance.
 */
function _iwformatter_ui_add_new_field_validate($form, &$form_state) {
  $values  = $form_state['values']['new_field'];
  $element = $form['tools']['new_field'];
  
  foreach (array('label', 'field_name', 'type', 'widget') as $key) {
    if (empty($values[$key])) {
      form_error($element[$key], t(
        '%key cannot be blank. Please choose a proper %key to use.',
        array('%key' => strtr($key, '_', ' '))
      ));
    }
  }
}

/**
 * Form submit callback which creates a new field
 * and field instance for this entity and bundle.
 */
function _iwformatter_ui_add_new_field_submit($form, &$form_state) {
  $values = $form_state['values']['new_field'];
  $field_name = 'field_'. $values['field_name'];
  
  // Field setup and creation
  $field = array(
    'field_name' => $field_name,
    'type' => $values['type'],
  );
  field_create_field($field);
  
  // Instance setup and creation.
  $instance = array(
    'entity_type' => $form_state['entity_type'],
    'bundle' => $form_state['bundle_name'],
    'field_name' => $field_name,
  ) + $values;
  
  $instance['widget'] = array('type' => $instance['widget']);
  unset($instance['actions'], $instance['type']);
  
  field_create_instance($instance);
  
  // Redirect to the edit forms (field settings and instance).
  $dest = drupal_get_destination();
  $base_uri = _field_ui_bundle_admin_path($instance['entity_type'], $instance['bundle']);
  $instance_uri = "$base_uri/fields/$field_name?destination={$dest['destination']}";
  
  $form_state['redirect'] = array(
    'path' => "$base_uri/fields/$field_name/field-settings",
    'options' => array('query' => array('destination' => $instance_uri)),
  );
}


// ==========================
// Handle add existing field
// ==========================

/**
 * Form validation callback to check before adding an instance.
 */
function _iwformatter_ui_add_existing_field_validate($form, &$form_state) {
  $values  = $form_state['values']['existing_field'];
  $element = $form['tools']['existing_field'];
  
  foreach (array('label', 'field_name', 'widget') as $key) {
    if (empty($values[$key])) {
      form_error($element[$key], t(
        '%key cannot be blank. Please choose a proper %key to use.',
        array('%key' => strtr($key, '_', ' '))
      ));
    }
  }
}

/**
 * Form submit callback to use to add an existing field.
 */
function _iwformatter_ui_add_existing_field_submit($form, &$form_state) {
  $instance = array(
    'entity_type' => $form_state['entity_type'],
    'bundle' => $form_state['bundle_name']
  ) + $form_state['values']['existing_field'];
  
  $instance['widget'] = array('type' => $instance['widget']);
  unset($instance['actions']);
  
  field_create_instance($instance);
  
  $base_uri = _field_ui_bundle_admin_path($instance['entity_type'], $instance['bundle']);
  $form_state['redirect'] = array(
    'path' => $base_uri ."/fields/{$instance['field_name']}/edit",
    'options' => array('query' => drupal_get_destination()),
  );
}
