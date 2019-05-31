<?php
/**
 * @file
 * Contains pages and forms for performing CRUD operations on profiles.
 */


// ============================
// Overview page callback
// ============================

/**
 * Page callback to generate profile page
 */
function _cdreams_profile_content_overview_page() {
  global $user;
  
  $page['#attached']['library'] = array(
    array('system', 'jquery.form'),
    array('system', 'drupal.form'),
    array('system', 'drupal.ajax'),
    array('iwapi', 'iwapi.ajax'),
  );
  
  $page['profiles'] = array(
    '#theme' => 'table',
    '#header' => array(
      array('data' => t('Name'), 'field' => 'cp.name'),
      array('data' => t('Type'), 'field' => 'ct.type'),
      array('data' => t('Author'), 'field' => 'u.name'),
      array('data' => t('Status'), 'field' => 'cp.status'),
      t('Has page'),
      array('data' => t('Updated'), 'field' => 'cp.changed', 'sort' => 'desc'),
      t('Actions'),
    ),
    '#empty' => t('There are currently no profiles entered matching your search.'),
  );

  // Create the query to find all the profiles.
  $query = db_select('cdreams_profiles', 'cp')
    ->addTag('cdreams_profile_access')
    ->addMetaData('account', $user)
    ->addMetaData('op', 'view')
    ->fields('cp')
    ->extend('PagerDefault')->limit(50)
    ->extend('TableSort')->orderByHeader($page['profiles']['#header']);

  // Added for sorting purposes + display
  $query->innerJoin('cdreams_profile_types', 'ct', 'ct.type=cp.type');
  $query->addField('ct', 'label', 'type_name');
  $query->addField('ct', 'has_page');

  $query->leftJoin('users', 'u', 'u.uid=cp.uid');  
  $query->addField('u', 'name', 'author');

  // Fetch $profiles and create a table entry for each.
  foreach ($query->execute() as $profile) {
    $baseUri = "profile/nojs/$profile->profile_id";
    
    $page['profiles']['#rows'][] = array(
      l($profile->name, $baseUri),
      check_plain($profile->type_name),
      l($profile->author, "user/$profile->uid"),
      t((bool)$profile->status ? 'published' : 'not published'),
      t((bool)$profile->has_page ? 'yes' : 'no'),
      format_date($profile->changed, 'small'),
      theme('links', array(
        'links' => array(
          'edit'   => array(
            'title' => 'Edit',
            'href' => "$baseUri/edit",
            'attributes' => array('class' => array('use-ajax')),
          ),
          'delete' => array(
            'title' => 'Delete',
            'href' => "$baseUri/delete",
            'attributes' => array('class' => array('use-ajax')),
          ),
        ),
        'attributes' => array('class' => array('inline')),
      )),
    );
  }
  
  $page['pager'] = array('#theme' => 'pager');
  return $page;
}


// ============================
// Overview page callback
// ============================

/**
 * Page menu callback to render the product add pages for
 * the product lines and items.
 */
function _cdreams_profile_create_types_page($bundles = NULL, $ajax = FALSE) {
  drupal_set_title(t('Add new profile of type'));
  
  $types = cdreams_profile_type_load();
  if (empty($types)) {
    $page = array(
      '#markup' => t(
        'There are no profile types available. You will need to create or '.
        'enable a profile type for this context. You can do so !link.',
        array('!link' => l('here', 'admin/structure/cdreams-profiles/add'))
      ),
    );
  }
  else {
    if (isset($bundles)) {
      $types = array_intersect_key($types, array_flip($bundles));
    }
    
    // Only one? Create new content of that type. 
    if (count($types) === 1) {
      $type = reset($types);
      drupal_goto('profile/'. ($ajax ? 'ajax' : 'nojs') .'/add/'. strtr($type->type, '_', '-'));
    }
    
    $page = array(
      '#theme' => 'cdreams_profile_add_list',
      '#items' => array(),
      '#attributes' => array('class' => array('admin-list')),
    );

    foreach ($types as $bundle => $info) {
      $page['#items'][$bundle] = array(
        'title' => $info->label,
        'href' => 'profile/nojs/add/'. strtr($bundle, '_', '-'),
        'attributes' => array('class' => array('use-ajax')),
        'description' => empty($info->description) ? NULL : $info->description,
      );
    }
  }

  return $page;
}


// ==============================
// Add or Edit page / form.
// ==============================

/**
 * Generate a menu page callback 
 * 
 * @param <cdProfileEntity | stdClass> $profile
 *  Either the loaded profile entity to edit, or the
 *  entity type definition to inform the system of
 *  the profile type being requested to create.
 * @return array
 *  Renderable page contents. $ajax will determine if
 *  it will be delivered as a normal page or AJAX command.
 */
function _cdreams_profile_edit_page($profile, $ajax = FALSE) {
  if (empty($profile->type)) {
    return MENU_NOT_FOUND;
  }
  
  $form_id = $profile->type .'_cdreams_profile_form';
    
  // Determine if we're building a new product or editing an existing one.
  if ($profile instanceof cdProfileEntity) {
    $title    = '<em>Edit</em> @type: %title';
    $type     = cdreams_profile_type_load($profile->type);
    $t_params = array('@type' => $type->label, '%title' => $profile->name);
  }
  else {
    global $user;
      
    $title    = '<em>Add</em> @type';
    $t_params = array('@type' => $profile->label);
    
    // Create a default profile shell for data
    $profile = new cdProfileEntity(array(
      'type'    => $profile->type,
      'uid'     => $user->uid,
      'created' => REQUEST_TIME,
    ));
  }

  // Determine if AJAX creation is being requested.
  if ($ajax) {
    iwapi_include('ajax');

    $state    = array();
    $commands = array();
    $form  = _iwapi_ajax_get_form($form_id, $state, $profile);
      
    // Check the submissions status of the form, and build appropriate command.
    if (!$command = iwapi_ajax_form_complete($state, NULL, TRUE)) {
      $commands[] = iwapi_ajax_command_dialog(
        strip_tags(t($title, $t_params)),
        array(
          'messages' => array('#theme' => 'status_messages'), 
          'form' => $form,
        ),
        array('width' => 800)
      );
    }    
    else {
      $commands[] = $command;
      
      // We know this will always be 
      $entity = $state['cdreams_profile'];
      $commands[] = array(
        'command'    => 'entity_saved',
        'entityType' => 'cdreams_profile',
        'entityId'   => $entity->profile_id,
        'isNew'      => $state['is_new'],
        'label'      => check_plain($entity->name),
      );
    }

    return array('#type' => 'ajax', '#commands' => $commands);
  }
  else {
    drupal_set_title(t($title, $t_params), PASS_THROUGH);
    return drupal_get_form($form_id, $profile);
  }
}

/**
 * Form build callback which allows
 */
function cdreams_profile_form($form, &$form_state, cdProfileEntity $profile) {
  // Retrieve and store the most updated version.
  if (isset($form_state['cdreams_profile'])) {
    $profile = $form_state['cdreams_profile'];
  }
  else {
    $form_state['cdreams_profile'] = $profile;
  }
  
  $form['#entity_type'] = 'cdreams_profile';
  $form['#entity'] = $profile;
  
  // Setup the form elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile name'),
    '#required' => TRUE,
    '#maxlength' => 255,
    '#default_value' => $profile->name,
    '#weight' => -10,
  );
  
  // Record all values that will need to get passed onward
  foreach (array('profile_id', 'uid', 'type', 'created', 'status') as $key) {
    if (isset($profile->{$key})) {
      $form[$key] = array(
        '#type' => 'value',
        '#value' => $profile->{$key},
      );
    }
  }
  
  // Allow addition of vertical tabs for a later time.
  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 98,
  );
  
  $form['status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status'),
    '#group' => 'additional_settings',
    '#tree' => FALSE,
    
    'status' => array(
      '#type' => 'checkbox',
      '#title' => t('Published'),
      '#default_value' => !empty($profile->status),
    ),
  );

  // Add Field API fields to this instance
  field_attach_form('cdreams_profile', $profile, $form, $form_state);
  
  // Form actions for saving or URL to the deleting of the product.
  $form['actions'] = array(
    '#type' => 'actions',
    
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#validate' => array('cdreams_profile_form_validate'),
      '#submit' => array('cdreams_profile_form_submit'),
    ),
  );
  
  if (!empty($profile->profile_id)) {
    $form['actions']['delete'] = array(
      '#type' => 'link',
      '#title' => 'Delete',
      '#href' => "profile/nojs/{$profile->profile_id}/delete",
      '#options' => array(
        'html' => FALSE,
        'attributes' => array('class' => array('button')),
      ),
    );
  }

  return $form;
}

/**
 * Form validation callback to validate the profile fields.
 */
function cdreams_profile_form_validate($form, &$form_state) {
  $profile = new cdProfileEntity($form_state['values']);
  
  // Remove house keeping items.
  unset($profile->form_id, $profile->form_token);
  unset($profile->form_build_id, $profile->op);
  
  // Validate the field submissions.
  field_attach_form_validate('cdreams_profile', $profile, $form, $form_state);
}

/**
 * Form submit callback to save a profile object.
 */
function cdreams_profile_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  
  try {
    $profile = new cdProfileEntity($form_state['values']);
    field_attach_submit('cdreams_profile', $profile, $form, $form_state);

    // Record if changed were made.
    $form_state['is_new'] = empty($profile->profile_id);
    
    $profile->save();
    $form_state['cdreams_profile'] = $profile; // record for later submit or handling.
    
    $uri = entity_uri('cdreams_profile', $profile);
    $form_state['redirect'] = array($uri['path'], $uri['options']);
    
    // Inform the user of success!
    drupal_set_message(t(
      'Commond Dreams profile %name was saved successfully!',
      array('%name' => $profile->name)
    ));
  }
  catch (Exception $e) {
    /* Exception already reported (safe to ignore). */
  }
}


// ==============================
// Confirm delete page / form.
// ==============================

/**
 * Page which handles the confirmation delete page.
 */
function _cdreams_profile_delete_page(cdProfileEntity $profile, $ajax = FALSE) {
  $form_id = 'cdreams_profile_delete_confirm';   

  // Determine if AJAX creation is being requested.
  if ($ajax) {
    iwapi_include('ajax');

    $state    = array();
    $commands = array();
    $form  = _iwapi_ajax_get_form($form_id, $state, $profile);
      
    // Check the submissions status of the form, and build appropriate command.
    if (!$command = iwapi_ajax_form_complete($state, NULL, TRUE)) {
      $commands[] = iwapi_ajax_command_dialog(drupal_get_title(), array(
        'messages' => array('#theme' => 'status_messages'), 
        'form' => $form,
      ));
    }    
    else {
      $commands[] = $command;
      $commands[] = iwapi_ajax_command_delete_tablerow();
    }

    return array('#type' => 'ajax', '#commands' => $commands);
  }
  else {
    return drupal_get_form($form_id, $profile);
  }
}

/**
 * Form builder which creates the delete confirmation form.
 */
function cdreams_profile_delete_confirm($form, &$form_state, cdProfileEntity $profile) {
  $form['#entity_type'] = $entity_type;
  $form['#entity']      = $profile;
  
  $form_state['cdreams_profile'] = $profile;
  
  drupal_set_title(t("Delete profile: @name", array('@name' => $profile->name)), PASS_THROUGH);
  
  $form['message'] = array(
    '#markup' => t(
      'Are you sure you want to delete this profile? '.
      'Once deleted this action cannot be undone.'
    ),
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'confirm' => array(
      '#type' => 'submit',
      '#value' => t('Confirm Delete'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => 'Cancel',
      '#href' => 'admin/content/cdreams-profiles',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
    
  // Build the confirmation form to make sure before deleting a product
  return $form;
}

/**
 * Form submit callback after deletion has been confirmed.
 */
function cdreams_profile_delete_confirm_submit($form, &$form_state) {  
  try {
    // We are using HOOK_entity_delete() to remove related product items
    // if this is a product line. This is so that the removal occurs
    // within the transaction, and can get backed out if something happens.
    $form_state['cdreams_profile']->delete();
  
    drupal_set_message(t('Successfully deleted profile'));
    $form_state['redirect'] = 'admin/content/cdreams-profiles';
  }
  catch (Exception $e) {
    drupal_set_message(t('Error deleting profile.'), 'error');
  }
}