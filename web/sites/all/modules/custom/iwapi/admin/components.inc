<?php
/**
 * @file components.admin.inc
 * 
 */

function &_iwapi_retrieve_component(&$package, $parents) {
  $def = &$package->data;
  foreach ($parents as $delta) {
    if (empty($def[$delta])) {
      throw new iwException('Component @component was not found in package @name', array(
        '@component' => $delta,
        '@name' => $package->name,
      ));
    }
    $def = &$def[$delta];
  }
  return $def;
}

function _iwapi_edit_package_component_form($form, &$form_state, $package, $type, $name) {
  drupal_set_title(t('@type Component: @name', array('@type' => ucfirst($package->type), '@name' => $name)));

  // Store information that will be needed later to find this element again.
  $form['#package'] = $package;
  $form['#component_parents'] = array($type, $name);

  try {
    $def = &_iwapi_retrieve_component($package, $form['#component_parents']);
    _iwapi_load_class($className = $def['className']);

    // Append the object's configuration form elements into the form.
    // Also attach package information within the scope configurable class.
    $form['component'] = array(
      '#parents' => array('component'),
      '#package' => $package,
      '#definition' => $def,
      '#tree' => TRUE,
    );
    
    $validation_scope = NULL;
    $callbacks = array('_iwapi_edit_package_component_form_submit');
    $className::appendFormElements($form['component'], $callbacks, $validation_scope);    
     
    // Create the possible save actions
    $form['actions'] = array(
      '#type' => 'actions',

      // The usually save and delete actions
      'save_btn' => array('#type' => 'submit', '#value' => t("Save")),
      'cancel' => array('#markup' => l('Cancel', "admin/config/iwapi/package/{$form['#package']->name}/edit")),
    );
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
  return $form;
}

function _iwapi_edit_package_component_form_validate(&$form, &$form_state) {
  $context   = 'component';
  $package   = $form['#package'];

  // Determine if the triggering element belongs to the inner package configurations.
  try {
    $def = &_iwapi_retrieve_component($package, $form['#component_parents']);
    _iwapi_load_class($className = $def['className']);
      
    // attempt to validate the form component settings
    $trigger = $form_state['triggering_element'];
    $className::validateFormElements($form[$context], $form_state['values'][$context], $trigger);
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
}

function _iwapi_edit_package_component_form_submit($form, &$form_state) {
  $context   = 'component';
  $package   = $form['#package'];

  try {
    $def = &_iwapi_retrieve_component($package, $form['#component_parents']);
    _iwapi_load_class($className = $def['className']);

    $trigger = $form_state['triggering_element'];
    $result = $className::submitFormElements($form[$context], $form_state['values'][$context], $trigger);

    // update and save the element definition
    if (!empty($result['save'])) {
      $def = $form[$context]['#definition'];
      iwapi_package_save($package, FALSE);
    }

    $default_url = "admin/config/iwapi/package/{$package->name}/edit";
    $form_state['redirect'] = empty($result['redirect']) ? $default_url : $result['redirect'];
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
}


// ==========================================
// Component Delete Confirmation
// ==========================================

/**
 * Generates a form for deleting a specific component of an package.
 * 
 * @param $package
 *  The package information and definition.
 * @param $type
 *  The type of component we might be deleting
 * @param $name
 *  The actual identifier / name of the element being added.
 */
function _iwapi_delete_package_component_confirm($form, &$form_state, $package, $type, $name) {
  $form['#package'] = $package;
  $form['#component_parents'] = array($type, $name);
  
  $question = t(
    'Are you sure you wish to delete the component %name from package %package',
    array('%name' => $name, '%package' => $package->name)
  );
  
  // Attempt to build a reasonable summary of what's being deleted.
  $description = '';
  try {
    $def =&_iwapi_retrieve_component($package, $form['#component_parents']);
    _iwapi_load_class($className = $def['className']);
    $description = nl2br($className::summarize($def));
  }
  catch (iwException $e) {
    $e->logToMessages();
  }
  
  $path = "admin/config/iwapi/package/{$package->name}/edit";
  return confirm_form($form, $question, $path, $description);
}

/**
 * 
 */
function _iwapi_delete_package_component_confirm_submit($form, &$form_state) {
  $package = $form['#package'];
  $parents = $form['#component_parents'];
  
  // Drill down and remove the selected element from the package data,
  // and then resave the package without the component.
  $def = &$package->data;
  if (count($parents) > 0) {
    reset($parents);
    while (($parent = current($parents)) !== FALSE) {
      if (next($parents) === FALSE) {
        unset($def[$parent]);
        break;
      }
      $def = &$def[$parent];
    }
    
    // Save the removal of this field from the package.
     iwapi_package_save($package, FALSE);
  }
  $form_state['redirect'] = "admin/config/iwapi/package/{$package->name}/edit";
}

