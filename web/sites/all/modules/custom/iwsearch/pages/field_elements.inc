<?php
/**
 * @file
 * Generate the individualized search field element handler forms (edit / delete).
 */


// --------------------------------
// Field element edit form
// --------------------------------

/**
 * Generate the correct version and handling of the
 * search field element edit functionality. The
 * proper handling of the form submission relies on
 * if we are using AJAX or not. As a standard form
 * we are directly modifying the field, where AJAX
 * updates the field state from the calling edit
 * search instance form.
 * 
 * @param iwSearchProvider $provider
 *  The search provider for which the changes are made.
 * @param string $name
 *  Name of the element within the provider
 *  generate the edit form for.
 * @param bool $ajax
 *  If TRUE then the function will return data
 *  intended for an AJAX request. Otherwise this
 *  will be treated as a normal page request.
 * @return array | int
 *  A renderable array with the response data if
 *  a proper response can be returned. Otherwise
 *  an integer with the correct menu error code.
 */
function _iwsearch_field_element_edit_page(iwSearchProvider $provider, $name, $ajax = FALSE) {
  $stash    = new iwSearchStashController(); 
  $provider = $stash->fetch($provider);
  
  // See if the request element even exists.
  $name = strtr($name, '-', '_');
  if (!isset($provider->elements[$name])) {
    return MENU_NOT_FOUND;
  }
  
  $form_id = 'iwsearch_field_element_edit_form';
  if ($ajax) {
    iwapi_include('ajax');
    
    $state    = array();
    $commands = array();
    $form = _iwapi_ajax_get_form($form_id, $state, $provider, $name);
    if ($command = iwapi_ajax_form_complete($state, NULL, TRUE)) {
      $command['messages'] = '<div id="stash-messages" class="messages warning">'. t(
        'Changes are pending and will not take effect until '.
        'saved. You can cancel these changes at anytime.'
      ) .'</div>';
      
      $commands[] = $command;
      
      // Only thing that can change is the summary, update just the summary info
      $html_id    = "#edit-elements-row-$name .iwpackage-summary";
      $summary    = $state['search_element']->formatSummary();
      $commands[] = ajax_command_html($html_id, render($summary));
      
      // Depending on the form processing, the original object
      // might not be a reference to the updated version. Make
      // sure we point to the updated version.
      if (!empty($state['search_element'])) {
        $provider->elements[$name] = $state['search_element'];
        $stash->save($provider);
      }
    }
    else {
      $t_params = array('@name' => $provider->elements[$name]->label);
   
      // Return the dialog used to edit the form settings.
      $commands[] = iwapi_ajax_command_dialog(
        t('Edit @name field', $t_params),
        array('messages' => array('#theme' => 'status_messages'), 'content' => $form),
        array('minWidth' => 480, 'maxHeight' => 500)
      );
    }
  
    return array('#type' => 'ajax', '#commands' => $commands);
  }
  else {
    $state = array(
      're_render' => FALSE,
      'no_redirect' => TRUE,
      'build_info' => array('args' => array($provider, $name)),
    );
    
    // If not the AJAX version, persist the changes directly to
    // the {iwsearch_instance} table. Changes take effect directly.
    $form = drupal_build_form($form_id, $state);
    if (!empty($state['executed']) && empty($state['rebuild'])) {
      $provider->save(); // persist the actual changes

      $provider_uri = strtr($provider->getName(), '_', '-');
      drupal_goto("admin/config/search/iwsearch/$provider_uri/edit");
    }
    else {
      return $form;
    }
  }
}

/**
 * Form builder function to generate the search field
 * element edit form. This form is meant to alter the
 * current state of a provider's element instance.
 * 
 * It does not persist changes, that will be the
 * responsibility of the caller to know when to save.
 */
function iwsearch_field_element_edit_form(array $form, array &$form_state, iwSearchProvider $provider, $name) {
  $form_state['provider'] = $provider;
  $form_state['field_name'] = $name;
  
  $form['#parents']  = array();
  $form['#submit']   = array('iwsearch_field_element_edit_submit');
  $form['#validate'] = array('iwsearch_field_element_edit_validate');
  
  $element = $provider->elements[$name];
  $element->appendElements($form, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'actions',
    
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Field'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/search/iwsearch/'. $provider->getName() .'/edit',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  
  return $form;
}

/**
 * Form validation callback to ensure that values in the
 * edit form are valid. This will call the validateElements()
 * function of the iwSearchElement.
 */
function iwsearch_field_element_edit_validate(array $form, array &$form_state) {
  try {
    $provider = $form_state['provider'];
    $name     = $form_state['field_name'];
    $element  = $provider->elements[$name];

    $element->validateElements($form, $form_state['values'], $form_state);
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    drupal_set_message(t(
      'Unable to update field changes due to a validation error. ' .
      'Administrators should check the error logs for details.'
    ), 'error');
  }
}

/**
 * Form submit callback which will update the state
 * of the search field element.
 */
function iwsearch_field_element_edit_submit(array $form, array &$form_state) {
  try {
    $provider = $form_state['provider'];
    $name     = $form_state['field_name'];
    $element  = $provider->elements[$name];

    $element->submitElements($form, $form_state['values'], $form_state);
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    drupal_set_message(t(
      'Unable to update field changes due to an internal error. ' .
      'Administrators should check the error logs for details.'
    ), 'error');
  }
}


// ==================================
// Delete search field element
// ==================================

/**
 * Generate the correct version and handling of the
 * search field element edit functionality. The
 * proper handling of the form submission relies on
 * if we are using AJAX or not. As a standard form
 * we are directly modifying the field, where AJAX
 * updates the field state from the calling delete.
 * 
 * @param iwSearchProvider $provider
 *  The search provider for which the changes are made.
 * @param string $name
 *  Name of the element within the provider
 *  generate the edit form for.
 * @param bool $ajax
 *  If TRUE then the function will return data
 *  intended for an AJAX request. Otherwise this
 *  will be treated as a normal page request.
 * @return array | int
 *  A renderable array with the response data if
 *  a proper response can be returned. Otherwise
 *  an integer with the correct menu error code.
 */
function _iwsearch_field_element_delete_page(iwSearchProvider $provider, $name, $ajax = FALSE) {
  $stash    = new iwSearchStashController();
  $provider = $stash->fetch($provider);
  
  $name = strtr($name, '-', '_');
  if (!isset($provider->elements[$name])) {
    return MENU_NOT_FOUND;
  }
  
  $form_id = 'iwsearch_field_element_delete_confirm';
  if ($ajax) {
    iwapi_include('ajax');
    
    $state = array();
    $form  = _iwapi_ajax_get_form($form_id, $state, $provider, $name);
    if ($command = iwapi_ajax_form_complete($state, 'delete_tablerow', TRUE)) {
      unset($provider->elements[$name]);
      $stash->save($provider);
    }
    else {
      $t_params = array('%name' => $provider->elements[$name]->label);
   
      // Return the dialog used to edit the form settings.
      $command = iwapi_ajax_command_dialog(
        t('Delete %name field', $t_params),
        array('messages' => array('#theme' => 'status_messages'), 'content' => $form),
        array('minWidth' => 480, 'maxHeight' => 500)
      );
    }
  
    return array('#type' => 'ajax', '#commands' => array($command));
  }
  else {
    $state = array(
      're_render' => FALSE,
      'no_redirect' => TRUE,
      'build_info' => array('args' => array($provider, $name)),
    );
    
    // If not the AJAX version, persist the changes directly to
    // the {iwsearch_instance} table. Changes take effect directly.
    $form = drupal_build_form($form_id, $state);
    if (!empty($state['executed']) && empty($state['rebuild'])) {
      unset($provider->elements[$name]);
      
      $provider->save(); // persist the actual changes

      $provider_uri = strtr($provider->getName(), '_', '-');
      drupal_goto("admin/config/search/iwsearch/$provider_uri/edit");
    }
    else {
      return $form;
    }
  }
}

/**
 * Form callback to generate the deletion
 * confirmation page to ensure users didn't
 * click the link by mistake.
 */
function iwsearch_field_element_delete_confirm(array $form, array &$form_state, iwSearchProvider $provider, $name) {
  $form_state['provider'] = $provider;
  $form_state['field_name'] = $name;
  
  $form['message'] = array(
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#markup' => t(
      'Are you sure that you wish to delete this search field. Settings '.
      'could get lost, and this field will need to get reconfigured.'
    ),
  );
  
  $form['summary'] = $provider->elements[$name]->formatSummary() + array(
    '#prefix' => '<h4>Search field summary:</h4>',
  );

  $form['actions'] = array(
    '#type' => 'actions',

    'delete' => array(
      '#type' => 'submit',
      '#value' => t('Delete field'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/search/iwsearch/'. $provider->getName() .'/edit',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );

  return $form;
}
