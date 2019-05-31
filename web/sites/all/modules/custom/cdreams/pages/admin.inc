<?php
/**
 * @file
 * Create a form that contains settings form builders
 */

/**
 * Form builder which create a form for setting
 * the TTL for sorting meta data.
 */
function cdreams_content_settings_form($form, &$form_state) {
  $day = 24*3600;
    
  $form[CDREAMS_VAR_ORDERING_DATA_TTL] = array(
    '#type' => 'select',
    '#title' => t('Ordering Data TTL'),
    '#required' => TRUE,
    '#options' => array(
      3*$day => t('3 Days'),
      7*$day => t('1 Week'),
      14*$day => t('2 weeks'),
      21*$day => t('3 weeks'),
      30*$day => t('30 Days'),
    ),
    '#default_value' => _cdreams_get_ordering_ttl(),
    '#description' => t(
      'How far back should ordering data be kept in the database. '.
      'It can significantly speed up the database if it does not need '.
      'to maintain records that are too old.'
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
    )
  );
  
  return $form;
}

/**
 * Form submit callback to save and rebuild the ordering table.
 */
function cdreams_content_settings_form_submit($form, &$form_state) {
  $ttl = $form_state['values'][CDREAMS_VAR_ORDERING_DATA_TTL];
  variable_set(CDREAMS_VAR_ORDERING_DATA_TTL, $ttl);
}


// ============================
// Domain filter settings
// ============================

/**
 * Form builder callback which generates the
 * settings form for managing how the domain
 * filtering works for Common Dreams.
 */
function cdreams_domain_filter_settings_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'iwapi', 'loadable/ajax');

  // Make sure that our AJAX utilities are included.
  $form['#attached']['library'][] = array('iwapi', 'iwapi.ajax');
  
  if (!isset($form_state['settings'])) { 
    $form_state['settings'] = variable_get(CDREAMS_VAR_DOMAIN_FILTER, array()) + array(
      'domains' => array('admin.commondreams.org'),
      'alter_to' => CDREAMS_DOMAIN_CANONICAL, // by default, change domains to the canonical URL.
    );
  }
  $settings = $form_state['settings'];
  
  $form['domains'] = array(
    '#type' => 'table_elements',
    '#header' => array(t('Domain'), t('Actions')),
    '#empty' => t('There are currently no domains being to captured.'),
    '#description' => t('Enter domains to filter and convert.'),
  );
  
  foreach ($settings['domains'] as $index => $domain) {
    $form['domains'][$index] = array(
      'value' => array('#type' => 'value', '#value' => $domain),
      'text' => array('#markup' => 'http://' . check_plain($domain)),
      'ops' => array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "remove-domain-{$index}-button",
        '#submit' => array('cdreams_domain_filter_settings_remove_submit'),
        '#validate' => array(),
        '#limit_validation_errors' => array(),
        '#ajax' => array(
          'callback' => '_iwapi_ajax_single_command_callback',
          'ref_command' => 'iwapi_ajax_command_delete_tablerow',
        ),
      ),
    );
  }

  $form['add_domain'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add domain'),
    '#tree' => TRUE,
    
    'domain' => array(
      '#type' => 'textfield',
      '#title' => t('Domain'),
      '#field_prefix' => 'http://',
      '#attributes' => array('placeholder' => 'Enter domain'),
    ),
    
    'add_domain' => array(
      '#type' => 'submit',
      '#value' => t('Add Domain'),
    ),
  );

  // clean up.
  unset($form_state['input']['add_domain']);
  
  $form['alter_to'] = array(
    '#type' => 'radios',
    '#title' => t('Alter to'),
    '#options' => array(
      CDREAMS_DOMAIN_CANONICAL => t('Canonical domain'),
      CDREAMS_DOMAIN_CURRENT => t('Match domain to current'),
    ),
    '#default_value' => $settings['alter_to'],
    '#description' => t('Alter the captured domain to this version of the domain.'),
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save Changes'),
    ),
  );

  return $form;
}

function cdreams_domain_filter_settings_remove_submit($form, &$form_state) {
  $element = $form_state['triggering_element'];
  
  if (preg_match('/^remove-domain-(\d+)-button$/', $element['#name'], $matches)) {
    $index = $matches[1];
  
    $settings = variable_get(CDREAMS_VAR_DOMAIN_FILTER, array()) + array(
      'domains' => array('admin.commondreams.org'),
      'alter_to' => CDREAMS_DOMAIN_CANONICAL, // by default, change domains to the canonical URL.
    );
  
    // Remove the item
    unset($form_state['settings']['domains'][$index]);
    variable_set(CDREAMS_VAR_DOMAIN_FILTER, $settings);
  }

  $form_state['#rebuild'] = TRUE;
}

/**
 * Form submit callback to save settings for domain filtering.
 */
function cdreams_domain_filter_settings_form_submit($form, &$form_state) {
  $settings = variable_get(CDREAMS_VAR_DOMAIN_FILTER, array()) + array(
    'domains' => array('admin.commondreams.org'),
    'alter_to' => CDREAMS_DOMAIN_CANONICAL, // by default, change domains to the canonical URL.
  );
  
  switch ($form_state['values']['op']) {
    case t('Save Changes'):
      $settings['alter_to'] = $form_state['values']['alter_to'];
      $settings['domains']  = _iwapi_extract_field($form_state['values']['domains'], 'value');

      variable_set(CDREAMS_VAR_DOMAIN_FILTER, $settings);
      break;
  
    case t('Add Domain'):
      if (!empty($form_state['values']['add_domain']['domain'])) {
        $form_state['settings']['domains'][] = $form_state['values']['add_domain']['domain'];
      }

      $form_state['rebuild'] = TRUE;
      break;
  }  
}