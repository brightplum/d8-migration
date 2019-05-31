<?php
/**
 * @file
 * Contains the administrative forms for configuring the content type.
 */


// ============================
// Overview page
// ============================

/**
 * Page callback to list and link to the various
 * profile types and administrative links.
 * 
 * @return array
 *  Renderable array which will display a table of
 *  profile types with management links.
 */
function cdreams_profile_types_overview_page() {
  $page = array(
    'types_table' => array(
      '#theme' => 'table',
      '#header' => array(t('Name'), t('Actions')),
      '#empty' => t(
        'There are currently no profile types available. You can create one !link.',
        array('!link' => l('here', 'admin/structure/cdreams-profiles/add'))
      ),
    ),
  );
  
  $rows = array();
  foreach (cdreams_profile_type_load() as $type) {
    $baseUri = 'admin/structure/cdreams-profiles/manage/'. strtr($type->type, '_', '-');

    $rows[] = array(
      'name' => check_plain($type->label) .' <small>('. $type->type .')</small>',
      'links' => theme('links', array(
        'attributes' => array('class' => array('inline')),
        'links' => array(
          'edit'    => array('title' => 'Edit', 'href' => "$baseUri/edit"),
          'fields'  => array('title' => 'Manage fields', 'href' => "$baseUri/fields"),
          'display' => array('title' => 'Manage display', 'href' => "$baseUri/display"),
          'delete'  => array('title' => 'Delete', 'href' => "$baseUri/delete"),
        ),
      )),
    );
  }

  $page['types_table']['#rows'] = $rows;
  return $page;
}


// ============================
// Edit profile types
// ============================

/**
 * Form builder for creating the profile type edit form.
 */
function cdreams_profile_edit_type_form($form, &$form_state, $type = NULL) {
  drupal_add_library('iwapi', 'iwapi.ajax');
    
  if (!empty($form_state['profile_type'])) {
    $profileType = $form_state['profile_type'];
  }
  else {
    if (!isset($type)) {
      // Fill in default values for new type definition.
      $profileType = new stdClass();
      $profileType->isNew         = TRUE;
      $profileType->type          = NULL;
      $profileType->label         = '';
      $profileType->title_display = CDREAMS_PROFILE_TITLE_NORMAL;
      $profileType->has_page      = TRUE;
      $profileType->overrides     = array();
    }
    else {
      $profileType = $type;
    }
    
    $form_state['profile_type'] = $profileType;
  }

  // Populatable type values.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#size' => 32,
    '#required' => TRUE,
    '#default_value' => $profileType->type,
    '#machine_name' => array(
      'source' => array('label'),
      'exists' => 'cdreams_profile_type_load',
    ),
  );
  
  $form['type'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#size' => 16,
    '#maxlength' => 32,
    '#disabled' => !empty($profileType->type),
    '#default_value' => $profileType->type,
    '#machine_name' => array(
      'source' => array('label'),
      'exists' => 'cdreams_profile_type_load',
    ),
  );
  
  $form['title_display'] = array(
    '#type' => 'select',
    '#title' => t('Title display'),
    '#options' => array(
      CDREAMS_PROFILE_TITLE_NORMAL => t('Use default page title'),
      CDREAMS_PROFILE_TITLE_EMBED  => t('Use field display settings'),
    ),
    '#default_value' => $profileType->title_display,
    '#description' => t('How is the title used in page views.'),
  );
  
  $form['has_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Profile has a page view'),
    '#default_value' => $profileType->has_page,
  );
  
  if (module_exists('views')) {
    $views = views_get_enabled_views();

    $form['view'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#options' => _iwapi_extract_field($views, 'human_name'),
      '#empty_option' => t('- Select a view -'),
      '#default_value' => $profileType->view,
      '#description' => t(
        'A view to be included in the display and should take '.
        'the profile id as an argument to filter content.'
      ),
    );
  }
  
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
    '#default_value' => isset($profileType->description) ? $profileType->description : NULL,
    '#description' => t(
      'Describe the usage of this profile, and other details '.
      'that can help with the selection of profile types.'
    ),
  );
  
  // Addition of overrides
  $form['overrides'] = array(
    '#type' => 'table_elements',
    '#title' => t('Field overrides'),
    '#header' => array(t('Profile field'), t('Parent field'), t('Actions')),
    '#empty' => t('Profile type must have embeddable fields to use overrides.'),
    '#description' => t(
      'Specify fields, that if they exist in the containing entity, will '.
      'be used in place of a field in this profile, when embedded.'
    ),
  );

  // Form submission options.
  $form['actions'] = array(
    '#type' => 'actions',

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => '-10',
      '#submit' => array('cdreams_profile_edit_type_form_submit'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => 'Cancel',
      '#href' => 'admin/structure/cdreams-profiles',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  
  if (empty($profileType->isNew)) {
    $instances = cdProfileEntity::EmbeddableFields($profileType->type);
    $remainder = array_diff_key($instances, $profileType->overrides);
    
    foreach ($profileType->overrides as $pField => $oField) {
      $form['overrides'][$pField] = array(
        'profile_field' => array(
          '#markup' => check_plain($instances[$pField]) .' <small>('. $pField .')</small>',
        ),
        'parent_field' => array('#markup' => $oField),
        'remove' => array(
          '#type' => 'submit',
          '#value' => t('Remove'),
          '#name' => 'override-'. strtr($pField, '_', '-') .'-remove',
          '#submit' => array('_cdreams_profile_type_remove_override_submit'),
          '#validate' => array(),
          '#limit_validation_errors' => array(),
          '#ajax' => array('callback' => '_cdreams_profile_type_remove_override_ajax'),
        ),
      );
    }
    
    if (!empty($remainder)) {
      $other_fields = array_keys(field_info_field_map());

      $form['overrides']['__add_new'] = array(
        'profile_field' => array(
          '#type' => 'select',
          '#options' => $remainder,
          '#empty_option' => t('-Select profile field-'),
        ),
        'parent_field' => array(
          '#type' => 'select',
          '#options' => array_combine($other_fields, $other_fields),
          '#empty_option' => t('-Select parent field-'),
        ),
        'add_override' => array(
          '#type' => 'submit',
          '#value' => t('Add Override'),
          '#submit' => array('_cdreams_profile_type_add_override_submit'),
          '#validate' => array('_cdreams_profile_type_add_override_validate'),
          '#limit_validation_errors' => array(array('overrides', '__add_new')),
        ),
      );
     
      array_unshift($form['actions']['save']['#submit'], '_cdreams_profile_type_add_override_submit');
    }

    $form['actions']['delete'] = array(
      '#type' => 'link',
      '#title' => t('Delete'),
      '#href' => 'admin/structure/cdreams-profiles/'. strtr($type->type, '_', '-') .'/delete',
      '#attributes' => array('class' => array('button')),
      '#weight' => -5,
    );
  }
  return $form;
}

/**
 * Form submit callback for editing profile types. Will save settings.
 */
function cdreams_profile_edit_type_form_submit($form, &$form_state) {
  try {
    $type   = $form_state['profile_type'];
    $schema = drupal_get_schema('cdreams_profile_types');    
    $values = array_intersect_key($form_state['values'], $schema['fields']);

    $values['overrides'] = serialize($type->overrides);
    
    db_merge('cdreams_profile_types')
      ->key(array('type' => $type->type))
      ->fields($values)
      ->execute();
    
    if (!empty($type->isNew)) {
      field_attach_create_bundle('cdreams_profile', $type->type);
    }

    // Clean caches
    _cdreams_profile_types(TRUE);
    cache_clear_all('field_info:bundle_extra:cdreams_profile:', 'field_cache', TRUE);
    
    // Set form states and prepare for form processing completion.
    $form_state['rebuild']  = FALSE;
    $form_state['redirect'] = 'admin/structure/cdreams-profiles';
    drupal_set_message(t('Profile type %label was saved successfully.', array('%label' => $type->label)));
  }
  catch (Exception $e) {
    watchdog_exception('cdreams_profile', $e);
    drupal_set_message(t('Unable to save profile type settings.'), 'error');
  }
}


// ============================
// Add a field override
// ============================

/**
 * Form validate callback to ensure field override requirements are set.
 */
function _cdreams_profile_type_add_override_validate($form, &$form_state) {
  $values = $form_state['values']['overrides']['__add_new'];
  
  if (empty($values['profile_field'])) {
    form_error(
      $form['overrides']['__add_new']['profile_field'],
      t('When adding an override, profile embed field must be set.')
    );
  }
  if (empty($values['parent_field'])) {
    form_error(
      $form['overrides']['__add_new']['parent_field'],
      t('When adding an override, parent field must be set.')
    );
  }
}

/**
 * Form submit callback which adds a field override.
 */
function _cdreams_profile_type_add_override_submit($form, &$form_state) {
  $values = $form_state['values']['overrides']['__add_new'];
  
  if (!empty($values['profile_field']) && !empty($values['parent_field'])) {
    $type = $form_state['profile_type'];
    $type->overrides[$values['profile_field']] = $values['parent_field'];
  }
  
  $form_state['rebuild'] = TRUE;
}

// ============================
// Remove a field override
// ============================

/**
 * Form submit callback which removes an override from a profile type.
 */
function _cdreams_profile_type_remove_override_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  
  if (preg_match('#^override-([\w\-]+)-remove$#', $trigger['#name'], $matches)) {
    unset($form_state['profile_type']->overrides[$matches[1]]);
  }
    
  $form_state['rebuild'] = TRUE;
}

/**
 * Form AJAX callback which removes the table row from overrides.
 */
function _cdreams_profile_type_remove_override_ajax($form, $form_state) {
  iwapi_include('ajax');
  
  return array(
    '#type' => 'ajax',
    '#commands' => array(iwapi_ajax_command_delete_tablerow()),
  );
}


// ============================
// Delete type confirmation
// ============================

/**
 * Allow users a chance to confirm the deleting of a
 * profile type before deleting it and all content.
 */
function cdreams_profile_delete_type_confirm($form, &$form_state, $type) {
  $form_state['profile_type'] = $type;
  
  drupal_set_title(t('Delete %label and related profiles?', array('%label' => $type->label)), PASS_THROUGH);
   
  $form['question']['#markup'] = '<p>'. t(
    'Are you sure that you wish to delete this profile type? <br/>'.
    'Deleting it will remove all profiles that use this type.'  
  ) .'</p>';
  
  $count = db_query(
    'SELECT COUNT(1) FROM {cdreams_profiles} WHERE type=:type',
    array(':type' => $type->type)
  )->fetchField();
  
  $form['count']['#markup'] = '<p>'. format_plural($count, 
    'There is currently %count profile of this type that will be deleted.',
    'There are currently %count profiles of this type that will be deleted.',
    array('%count' => $count)
  ) .'</p>';
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'confirm' => array(
      '#type' => 'submit',
      '#value' => t('Confirm Delete'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/structure/cdreams-profiles',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  return $form;
    
  return $form;
}

/**
 * User has confirmed the deletion of profile type. Delete it
 * and all profiles using this type.
 */
function cdreams_profile_delete_type_confirm_submit($form, &$form_state) {
  $type = $form_state['profile_type'];
  
  try {
    db_delete('cdreams_profile_types')
      ->condition('type', $type->type)
      ->execute();

    // Tell Drupal that this bundle is gone and reset cache.
    field_attach_delete_bundle('cdreams_profile', $type->type);    
    _cdreams_profile_types(TRUE);

    // Create a batch operation to clean out all the work products of this bundle
    $batch = array(
      'title' => t('Deleting profile content'),
      'operations' => array(
        array('_iwapi_batch_clear_entity_bundle', array('cdreams_profile', $type->type)),
      ),
      'finished' => '_iwapi_batch_clear_entity_bundle_finished',
      'file' => drupal_get_path('module', 'iwapi') .'/includes/entity.batch.inc',
    );
    batch_set($batch);

    $t_params = array('%title' => $type->label);
    drupal_set_message(t('Successfully deleted profile type %title.', $t_params));
    $form_state['redirect'] = array('path' => 'admin/structure/cdreams-profiles');
  }
  catch (Exception $e) {
    watchdog_exception('cdreams_profile', $e);
    drupal_set_message(t('Unable to delete profile type.'), 'error');
  }
}
