<?php
/**
 * @file tuckaway_header.admin.inc
 * Contains the form builders and callbacks for administrative tasks.
 */


/**
 * Form builder for generating the setting form to configure H1 tuckaways.
 */
function tuckaway_header_settings_form($form, &$form_state) {
  $theme = variable_get('theme_default', 'bartik');
  
  $form['theme'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display and Location'),
    '#tree' => FALSE,
    
    TUCKAWAY_HEADER_VAR_REGION => array(
      '#type' => 'select',
      '#title' => t('Display region'),
      '#options' => system_region_list($theme),
      '#default_value' => variable_get(TUCKAWAY_HEADER_VAR_REGION, 'page_top'),
      '#description' => t('Theme region to display the tuckaway header if it is available.'),
    ),
  );
  
  $types = array();
  foreach (_tuckaway_header_get_tuckaway_info() as $type_name => $info) {    
    if ($entity_info = entity_get_info($type_name)) {
      $types[$type_name] = check_plain($entity_info['label']);
    }
  }
  
  $current_types = variable_get(TUCKAWAY_HEADER_VAR_TYPES, array('node' => TRUE));
  $form['entity'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Settings'),
    '#tree' => FALSE,
    
    TUCKAWAY_HEADER_VAR_TYPES => array(
      '#type' => 'checkboxes',
      '#title' => 'Entity types',
      '#options' => $types,
      '#default_value' => array_keys(array_filter($current_types)),
      '#description' => t('Assign entity types that are allowed to have tuckaway headers.'),
    ),
  );
  
  return system_settings_form($form);
}
