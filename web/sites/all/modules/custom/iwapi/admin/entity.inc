<?php
/**
 * @file admin/entity.inc
 * Contains the forms and utilities to facilitate exporting, and cloning of entities.
 * 
 * @see includes/entity.inc for the entity handling functions and helpers.
 */

/**
 * Form builder callback to generate the elements for the
 * entity management forms. This allows cloning or export of
 * entity properties into file or another entity.
 * 
 * @see _iwapi_entity_modify_form_submit().
 * @see _iwapi_entity_clone_form_submit();
 */
function iwapi_entity_modify_form($form, &$form_state) {
  iwapi_include('entity');
  
  // If a source entity is not yet known, create form elements
  // for the user to select the entity and options to use.
  $fieldable_types = array();
  $entity_types = entity_get_info();
 
  foreach ($entity_types as $type => $info) {
    if (!empty($info['fieldable'])) {
      $fieldable_types[$type] = $info['label'];
    }
  }

  // Common AJAX event for the various form elements.
  $ajax_setup = array(
    'callback' => '_iwapi_entity_select_form_ajax',
    'wrapper'  => 'entity-export-bundles',
    'method'   => 'replace',
  );
  
  // Prepare some shared values.
  $values = isset($form_state['values']) ? $form_state['values'] : array();
  $label  = empty($form_state['source_entity']) ? t('Source Entity:') : t('Clone to:');
   
  $form['entity_type'] =  array(
    '#type' => 'select',
    '#title' => t('Entity type'),
    '#required' => TRUE,
    '#prefix' => '<h2>'. $label .'</h2>', 
    '#options' => $fieldable_types,
    '#ajax' => $ajax_setup,
  );
  
  $form['bundles'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array('id' => 'entity-export-bundles'),
  );

  // If an entity has been selected, we want to build the elements to select the bundle
  if (!empty($values['entity_type'])) {
    $type = $values['entity_type'];
    
    $form['bundles']['entity_bundle'] = array(
      '#type' => 'select',
      '#title' => t('Bundle'),
      '#required' => TRUE,
      '#options' => _iwapi_extract_field($entity_types[$type]['bundles']),
      '#ajax' => $ajax_setup,
    );
    
    // If a user has already submitted the source entity
    // we can skip to building a form to select the destination.
    if (!empty($form_state['source_entity'])) {
      unset($form['bundles']['entity_bundle']['#ajax']);
      
      $form['bundles']['actions'] = array(
        '#type' => 'actions',

        'clone' => array(
          '#type'   => 'submit',
          '#value'  => t('Clone Entity'),
          '#submit' => array('_iwapi_entity_clone_form_submit'),
        ),
      );
    }
    elseif (!empty($form_state['values']['entity_bundle'])) {
      $bundle = $form_state['values']['entity_bundle'];
      
      // Iterate through the registered entity attachers and try to run them.
      foreach (_iwapi_get_entity_attacher_info() as $attachTo => $info) {
        try {
          $attacher = new $info['handler']($type, $bundle);

          // Check for requirements and valid data before attaching.
          if ($attacher instanceof iwIConfigurable) {
            $attacher->values = isset($values[$attachTo]) ? $values[$attachTo] : array();
    
            // Generate a container for attacher items
            $form['bundles'][$attachTo] = array(
              '#type'  => 'fieldset',
              '#title' => 'Export '. ucfirst($attachTo),
              '#tree'  => TRUE,
            );
            $attacher->appendElements($form['bundles'][$attachTo], $form, $form_state);
          }
        }
        catch (iwException $e) {
          $e->logToMessages();
        }
      }
      
      // Create the set of actions that allow us to complete the process.
      $form['#submit'][] = '_iwapi_entity_modify_form_submit';
      $form['bundles']['actions'] = array(
        '#type' => 'actions',
        'export' => array('#type' => 'submit', '#value' => t('Export Template')),
        'clone'  => array('#type' => 'submit', '#value' => t('Apply or Clone Entity')),
      );
    }
  }
  
  // If no previous action has been set, we just want to create a "next" button.
  if (empty($form['bundles']['actions'])) {
    $form['bundles']['actions'] = array(
      '#type' => 'actions',
      'next' => array('#type' => 'submit', '#value' => t('Next')),
    );
  }
  return $form;
}

/**
 * Form builder submit callback to either export the entity template
 * to file or prepare for the entity clone form.
 */
function _iwapi_entity_modify_form_submit($form, &$form_state) {
  if (empty($form_state['values']['entity_type']) || empty($form_state['values']['entity_bundle'])) {
    $form_state['rebuild'] = TRUE;
    return;
  }

  $action = NULL;
  foreach ($form['bundles']['actions'] as $key => $element) {
    if ($key[0] !== '#' && !empty($element['#value']) && $form_state['values']['op'] === $element['#value']) {
      $action = $key;
      break;
    }
  }
  
  iwapi_include('entity');
  $template = _iwapi_extract_entity_template_data($form, $form_state);
  if ($action === 'export') {
    $type   = $form_state['values']['entity_type'];
    $bundle = $form_state['values']['entity_bundle'];

    // Generate the file contents of the template and present them as a file.
    $filename = "{$type}--{$bundle}.template".'.inc';
    drupal_add_http_header('Content-type', 'text/plain; charset=utf-8');
    drupal_add_http_header('Cache-Control', 'max-age=60, must-revalidate');
    drupal_add_http_header('Content-Disposition', 'attachment; filename="'. $filename .'"');

    include_once(DRUPAL_ROOT .'/includes/utility.inc');
    print "<?php\n\n";
    print '$template = '. drupal_var_export($template) .";\n";
    print "\n";
    drupal_exit();
  }
  elseif (!empty($action)) {
    $form_state['rebuild'] = TRUE;
    $form_state['source_entity'] = $template;
    unset($form_state['input']['entity_bundle']);
    unset($form_state['values']['entity_bundle']);
  }
}

/**
 * Form builder submit callback to apply the entity template to
 * either another entity or to generate a new entity.
 */
function _iwapi_entity_clone_form_submit($form, &$form_state) {
  if (empty($form_state['values']['entity_type']) || empty($form_state['values']['entity_bundle'])) {
    $form_state['rebuild'] = TRUE;
    return;
  }
  
  iwapi_include('entity');
  $type   = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['entity_bundle'];
  $template = $form_state['source_entity'];
  
  try {
    _iwapi_attach_entity_template($type, $bundle, $template, $force = FALSE);
    
    drupal_set_message(t('Successfully cloned entity elements to %type:%bundle', array('%type' => $type, '%bundle' => $bundle)));
    $form_state['redirect'] = 'admin/structure/types';
  }
  catch (Exception $e) {
    watchdog_exception('iwapi', $e);
  }
}


// =============================
// Utility and common elements
// =============================

/**
 * Form AJAX callback to replace a portion of the iwapi_entity_export_bundle() form.
 * 
 * @param $form
 *  The structure of the form. Newly rebuilt and processed form structure.
 * @param $form_state
 *  Variable containing the current form state and values.
 */
function _iwapi_entity_select_form_ajax($form, $form_state) {
  return $form['bundles'];
}
