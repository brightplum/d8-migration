<?php
/**
 * @file
 * Contains the formatter settings form.
 */


/**
 * Form builder which generates the settings form.
 */
function iwformatter_admin_settings_form($form, &$form_state) {
  $form['iwformatter_override_field_ui'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use iwformatter containers for field management.'),
    '#default_value' => variable_get('iwformatter_override_field_ui', FALSE),
  );
  
  $form[IWFORMATTER_VAR_DOCK_ENTITY_ACTIONS] = array(
    '#type' => 'checkbox',
    '#title' => t('Dock entity form actions'),
    '#default_value' => variable_get(IWFORMATTER_VAR_DOCK_ENTITY_ACTIONS, TRUE),
  );
  
  return system_settings_form($form);
}
