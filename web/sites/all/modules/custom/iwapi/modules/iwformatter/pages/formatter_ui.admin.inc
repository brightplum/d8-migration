<?php
/**
 * @file
 * Contains field_ui additional handlers.
 */


// ======================================
// Configure view modes
// ======================================

/**
 * Form builder which exposes the view mode settings for
 * an entity type and bundle combination.
 */
function iwformatter_entity_view_mode_form($form, &$form_state, $type, $bundle) {
  $bundle_name = is_string($bundle) ? $bundle : field_extract_bundle($type, $bundle);
  
  $form_state['entity_type'] = $type;
  $form_state['bundle_name'] = $bundle_name;
  
  $bundle_settings = field_bundle_settings($type, $bundle_name);
  $entity_info = entity_get_info($type);
  
  $values = _iwapi_extract_field($bundle_settings['view_modes'], 'custom_settings');
  $keys = array_keys(array_filter($values));
  
  $form['view_modes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('View modes'),
    '#options' => _iwapi_extract_field($entity_info['view modes']),
    '#default_value' => array_combine($keys, $keys),
    '#description' => t('Select the display modes that you wish to have customized configurations for.'),
  );  

  $form['actions'] = array(
    '#type' => 'actions',

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );

  return $form;
}

/**
 * Form builder submit callback to save the view mode setitngs.
 */
function iwformatter_entity_view_mode_form_submit($form, &$form_state) {
  $type   = $form_state['entity_type'];
  $bundle = $form_state['bundle_name'];
  
  $bundle_settings = field_bundle_settings($type, $bundle);
  foreach ($form_state['values']['view_modes'] as $mode => $value) {
    $bundle_settings['view_modes'][$mode]['custom_settings'] = !empty($value);
  }
  
  field_bundle_settings($type, $bundle, $bundle_settings);
  
  drupal_set_message(t('View mode has been updated.'));
  $form_state['redirect'] = _field_ui_bundle_admin_path($type, $bundle) .'/display';
}


// =======================================
// Deletion of a content wrapper element.
// =======================================

/**
 * Form builder which constructs the content wrapper
 * delete confirmation form.
 */
function iwformatter_content_wrapper_delete_confirm($form, &$form_state, $wrapper) {  
  list($type, $bundle, $view) = iwFieldGroupController::ParseScope($wrapper->scope);
  $entity_info = entity_get_info($type);
  $bundle_info = $entity_info['bundles'][$bundle];

  $uri = _field_ui_bundle_admin_path($type, $bundle);

  switch ($view) {
    case 'form':
      $view_label = t('Form');
      $uri .= '/fields';
      break;
    case 'default':
      $view_label = 'default display';
      $uri .= '/display';
      break;
    default:
      $view_label = $entity_info['view modes'][$view]['label'];
      $uri .= '/display/'. $view;
  }
  
  drupal_set_title(t(
    'Are you sure you wish to delete %label for %entity - %bundle in the %view view.',
    array(
      '%label' => $wrapper->label,
      '%entity' => $entity_info['label'],
      '%bundle' => $bundle_info['label'],
      '%view' => $view_label,
    )
  ), PASS_THROUGH);
  
  $form['warning'] = array(
    '#prefix' => '<div class="form-introduction">',
    '#suffix' => '</div>',
    '#markup' => 'This action cannot be undone!',
  );
  
  $form['summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary'),
    
    'label' => array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#weight' => -15,
      '#markup' => '<strong>Label: </strong>'. check_plain(t($wrapper->label)),
    ),
    'type' => array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#weight' => -10,
      '#markup' => '<strong>Type: </strong>'. get_class($wrapper),
    ),
  ) + $wrapper->formatSummary();
  
  $form['actions'] = array(
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Confirm Delete'),
      '#submit' => array('iwformatter_content_wrapper_delete_submit'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => $uri,
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  
  $form_state['return_uri'] = $uri;
  $form_state['wrapper'] = $wrapper;
  return $form;
}

/**
 * Form submit callback to handle the content wrapper
 * delete functionality.
 */
function iwformatter_content_wrapper_delete_submit($form, &$form_state) {
  try {
    $wrapper = $form_state['wrapper'];
    $wrapper->delete();
    
    drupal_set_message(t(
      '%label - Content wrapper was successfully deleted.',
      array('%label' => $wrapper->label)
    ));
    
    $form_state['redirect'] = $form_state['return_uri'];
  }
  catch (Exception $e) {
    drupal_set_error(t('Unable to delete this field wrapper due to an internal error.'), 'error');
  }
}
