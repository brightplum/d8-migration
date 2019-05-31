<?php
/**
 * @file packages.form.inc
 * Contains the admin forms and pages for managing packages.
 */

/**
 * Display a page of available packages, and allow addition
 * of new packaged utilities. Packaged utilities provide
 * configurable administrator functionality.
 * 
 * @see _iwapi_packaged_utility_overview_submit().
 */
function _iwapi_packaged_utility_overview($form, &$form_state) {
  iwapi_include('elements');
  unset($form_state['input']);
  
  $form['#attached']['css'][] = drupal_get_path('module', 'iwapi') .'/admin/css/admin.css';
  
  //Get a list of all available packages
  $query = db_select('iwapi_packages', 'pkg')
    ->fields('pkg', array('name', 'type', 'label', 'description'))
    ->orderBy('type', 'ASC')
    ->orderBy('label', 'ASC');
  
  $form['packages'] = array(
    '#type' => 'table_elements',
    '#title' => t('Packaged Utilities'),
    '#header' => array(t('Label'), t('Type'), t('Description'), t('Actions')),
  );
  
  $type_labels = _iwapi_extract_field(iwapi_get_class_info('utility'));
  foreach ($query->execute() as $package) {
    $links = array();
    foreach (array('execute', 'edit', 'delete') as $op) {
      if ($uri = iwapi_package_uri($package, $op)) {
        $links[] = array(
          'title' => ucfirst($op),
          'href' => $uri['path'],
          'html' => FALSE,
        );
      }
    }
    
    // Add an individual package entry.
    $form['packages'][] = array(
      array('#markup' => check_plain($package->label) ." <small>(name: {$package->name})</small>"),
      array('#markup' => check_plain($type_labels[$package->type])),
      array('#markup' => check_markup($package->description, filter_fallback_format())),
      array(
        '#theme' => 'links',
        '#links' => $links,
        '#attributes' => array('class' => 'inline'),
      ),
    );
  }

  $form['packages']['add_new'] = array(
    'id' => array(
      '#prefix' => '<div class="add-new-placeholder">Add new:</div>',
      'label' => array(
        '#type' => 'textfield',
        '#size' => 16,
      ),
      'name' => array(
        '#type' => 'machine_name',
        '#size' => 16,
        '#required' => TRUE,
        '#machine_name' => array(
          'source' => array('packages', 'add_new', 'id', 'label'),
          'exists' => 'iwapi_package_load',
        ),
      ),
    ), 
    'type' => array(
      '#prefix' => '<div class="add-new-placeholder">&nbsp;</div>',
      '#type' => 'select',
      '#title' => t('Type'),
      '#title_display' => 'invisible',
      '#required' => TRUE,
      '#empty_value' => 0,
      '#options' => $type_labels,
    ),
    'description' => array(
      '#type' => 'textarea',
      '#rows' => 3,
    ),
    'add new' => array(
      '#type' => 'submit',
      '#value' => 'Add New',
      '#validate' => array(),
      '#submit' => array('_iwapi_packaged_utility_overview_submit'),
      '#limit_validation_errors' => array(array('packages', 'add_new')),
      '#cell_attributes' => array('class' => array('row-actions')),
    ),
    '#attributes' => array('class' => array('add-new-item')),
  );
  return $form;
}

/**
 * Save the newly created utility package.
 */
function _iwapi_packaged_utility_overview_submit($form, &$form_state) {
  if (!empty($form_state['values']['packages']['add_new'])) {
    $package = $form_state['values']['packages']['add_new'];
    $package += $package['id'];
    unset($package['id']);
    
    iwapi_package_save((object)$package, TRUE);
    $form_state['redirect'] = 'admin/config/system/iwapi-packages/'. $package->name;
  }
}


// =====================================
// Package utilize form
// =====================================

/**
 * 
 * @param $form
 * @param $form_state
 * @param $package
 */
function _iwapi_utilize_package_form($form, &$form_state, $package) {
  $form['#package'] = $package;
  
  $form['batching'] = array(
    '#type' => 'fieldset',
    '#title' => t('Batch Processing'),
    '#collapsible' => TRUE,

    'use_batching' => array(
      '#type' => 'checkbox',
      '#title' => t('Enable batch processing.'),
      '#default_value' => FALSE,
    ),
    'batch_size' => array(
      '#type' => 'textfield',
      '#title' => t('Maximum number of items per batch'),
      '#default_value' => 200,
    ),
  );
  
  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload importable file'),
    '#description' => t('Upload the appropriate file type (TODO: add better instructions based on package in use)'),
  
    'file' => array(
      '#type' => 'file',
      '#name' => 'files[' . implode('_', array('upload', 'file')) . ']',
      '#prefix' => '<div><label>' . t('Upload new file:') . '</label>',
      '#size' => 24,
      '#theme_wrappers' => array(),
    ),
    'button' => array(
      '#type' => 'submit',
      '#value' => 'Upload',
      '#suffix' => '</div>',
      '#submit'  => array('_iwapi_utilize_package_form_submit'),
    ),
  );
  return $form;
}

/**
 * 
 */
function _iwapi_utilize_package_form_submit($form, &$form_state) {
  $validators = array('file_validate_extensions' => array('zip csv'));
  
  if ($file = file_save_upload('upload_file', $validators, FALSE, FILE_EXISTS_REPLACE)) {
    $def = $form['#package']->data;

    try {
      _iwapi_load_class($className = $def['className']);
      $datamap = $className::unpackage($def);
      $datamap->addSource(drupal_realpath($file->uri));
      $datamap->execute();

      // If we've come this far, then no error exceptions where thrown
      // and we can assume that we were successful with the import.
      drupal_set_message('Import successful.');
    }
    catch (iwException $e) {
      drupal_set_message('Unable to use importer, or an error has occurred while loading.');
      $e->logToMessages();
    }
  }
  else {
    drupal_set_message('Unable to upload file. Currently only *.zip and *.csv are allowed.');
  }
}


// =====================================
// Package edit form
// =====================================

/**
 * Page callback
 * 
 * @param stdClass $package
 */
function _iwapi_packaged_utility_edit_page($package) {
  // Determine if we are looking at a component or the whole utility.
  $uri = iwapi_package_uri($package, 'edit');
  $comp_path = substr($_GET['q'], strlen($uri['path']) + 1);
  if (!empty($comp_url)) {
    $component = explode('/', $comp_path);
    $ajax = (array_shift($component) === 'ajax');
  }
 
  try {
    $form = array();
    $className = $package->type;
    $utilities = iwapi_get_class_info('utility');
    if (array_key_exists($className, $utilities) && class_exists($className, TRUE)) {
      // Append the object's configuration form elements into the form.
      // Also attach package information within the scope configurable class.
      if (empty($component)) {
        $form = drupal_get_form('iwapi_packaged_utility_edit', $package);
      }
      else {
        foreach ($component as $path) {
          $className = get_class($package->object);
        }
      }
    }
    return $form;
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
  catch (Exception $e)   { 
    drupal_set_message(t('Error on @line (@file): @msg', array(
      '@msg' => $e->getMessage(),
      '@file' => $e->getFile(),
      '@line' => $e->getLine(),
    )));
  }
  
  // Return an unable to render page error.
  return array(
    'content' => array(
      '#markup' => t('Unable to generate edit from for this package.'),
    ),
  );
}

/**
 * Generate a form that allows a user to edit packages.
 *
 * @param $package
 *  If we are creating a new package, this can just be a string
 *  with the name of the package type, otherwise it should contain
 *  the package definition.
 *
 * @see iwapi_edit_package_form_validate()
 * @see iwapi_edit_package_form_sumbit()
 */
function iwapi_packaged_utility_edit($form, &$form_state, stdClass $package) {
  $form['#iwapi_package'] = $package;
  
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#size' => 32,
    '#maxlength' => 64,
    '#default_value' => $package->label,
    '#description' => t('User friendly display name for the packaged utility.'),
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#maxlength' => 32,
    '#disabled' => TRUE,
    '#default_value' => $package->name,
    '#machine_name' => array(
      'source' => array('label'),
    ),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 5,
    '#default_value' => $package->description,
  );
  
  // Check for previously submitted values
  $state = array();
  if (isset($form_state['values']['utility'])) {
    $state = &$form_state['values']['utility'];
  }
  
  // Build the form container for the package results
  $form['package'] = array(
    '#parents' => array('package'),
    '#array_parents' => array('package'),
    '#tree' => TRUE,
  );
  $package->object->appendElements($form['package'], $state, $form);
  
  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );
 
  return $form;
}

/**
 * validate callback for _iwapi_edit_package_form()
 */
function iwapi_packaged_utility_edit_validate(&$form, &$form_state) {
/*  $context = 'package';
  $package = $form[$context]['#package'];
  $trigger = $form_state['triggering_element'];

  // Pass validation data into the customized package class.
  try {
    _iwapi_load_class($className = $package->data['className']);
    $className::validateFormElements($form[$context], $form_state['values'][$context], $trigger);
  }
  catch (iwException $e) {
    $e->logToMessages();
  }*/
}

/**
 * submit callback for _iwapi_edit_package_form()
 */
function iwapi_edit_package_form_save($form, &$form_state) {/*
  $context = 'package';
  $package = $form[$context]['#package'];
  $trigger = $form_state['triggering_element'];

  // Update the top level package settings.
  $is_new = empty($package->name);
  if ($is_new) {
    $package->name = $form_state['values']['name'];
  }
  $package->label = $form_state['values']['label'];
  $package->description = filter_xss_admin($form_state['input']['description']);

  // Determine if the triggering element belongs to the inner package configurations.
  try {
    _iwapi_load_class($className = $package->data['className']);

    $results =  $className::submitFormElements($form[$context], $form_state['values'][$context], $trigger);
    if (!empty($results['save'])) {
      $package->data = $form[$context]['#definition'];
    }
 
    // Update the form with redirect or rebuild information.
    foreach (array('redirect', 'rebuild') as $delta) {
      if (isset($results[$delta])) {
        $form_state[$delta] = $results[$delta];
      }
    }
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
  iwapi_package_save($package, $is_new);*/
}


// =====================================
// Package delete confirmation form
// =====================================

/**
 * Generates a confirmation form which when accepted will delete
 * the referenced package information from the database.
 *
 * @see _iwapi_package_delete_confirm_submit()
 */
function _iwapi_package_delete_confirm($form, &$form_state, $package) {
  $form['#package'] = $package;
  $question = t('Are you sure you wish to delete the package: %name', array('%name' => $package->name));
  $path = 'admin/config/system/iwapi/packages';

  return confirm_form($form, $question, $path, $package->description);
}

/**
 * Submit callback for _iwapi_package_delete_confirm(), which
 * performs package deletion.
 */
function _iwapi_package_delete_confirm_submit($form, &$form_state) {
  $result = db_delete('iwapi_packages')
    ->condition('name', $form['#package']->name)
    ->execute();

  drupal_set_message(t('Successfully deleted package %name.', array('%name' => $form['#package']->name)));
  $form_state['redirect'] = 'admin/config/system/iwapi/packages';
}
