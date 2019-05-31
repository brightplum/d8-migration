<?php
/**
 * @file iwsearch.admin.inc
 * The settings and administration form for building searches.
 */

/**
 * Form builder function to generate a configuration form.
 */
function iwsearch_manage_instances_page() {
  $providers = _iwsearch_get_search_providers(); // What search implementations are available.
  
  $page['instances'] = array(
    '#theme' => 'table',
    '#header' => array(t('Label'), t('Provider'), t('Entity'), t('Indexed'), t('Actions')),
    '#empty' => t('There are currently no search instances created. You may add one below.'),
  );
 
  // Find all the currently built search indexes.
  $names = db_query('SELECT name FROM {iwsearch_instances} ORDER BY label ASC')->fetchCol(); 
  foreach ($names as $name) {
    $inst        = _iwsearch_instance_load_unchanged($name);
    $uri_base    = 'admin/config/search/iwsearch/'. strtr($inst->getName(), '_', '-');
    $entity_info = entity_get_info($inst->getEntityType());
    
    $bundles = array_intersect_key($entity_info['bundles'], $inst->getBundles());
    $bundles = empty($bundles) ? 'ALL' : implode(', ', _iwapi_extract_field($bundles));
    
    $page['instances']['#rows'][$inst->getName()] = array(
      'name'     => check_plain(t($inst->label)) ." <small>({$inst->getName()})</small>",
      'provider' => check_plain($providers[get_class($inst)]),
      'entity'   => check_plain($entity_info['label']) .' <small>(Bundles: '. check_plain($bundles) .')</small>',  
      'indexed'  => $inst->hasLocalIndex() ? date('Y-m-d h:i:s', $inst->lastIndexed()) : '- NONE -',
      'actions'  => theme('links', array(
        'attributes' => array('class' => array('inline')),
        'links' => array(
          'edit'     => array('title' => t('Edit'), 'href' => "$uri_base/edit"),
          'elements' => array('title' => t('Fields'), 'href' => "$uri_base/elements"),
          'results'  => array('title' => t('Results'), 'href' => "$uri_base/results"),
          'delete'   => array('title' => t('Delete'), 'href' => "$uri_base/delete"),
        ),
      )),
    );
  }

  return $page;
}

/**
 * Form builder callback to generate an add new search provider instance.
 */
function iwsearch_instance_edit_form($form, &$form_state, $provider = NULL) {
  form_load_include($form_state, 'inc', 'iwapi', 'loadable/ajax');
  
  $providers = _iwsearch_get_search_providers();
  $values    = isset($form_state['values']) ? $form_state['values'] : array('provider' => '');
  
  $form = array(
    'provider' => array(
      '#type' => 'select',
      '#title' => t('Search provider'),
      '#empty_option' => t('- Select a search provider -'),
      '#options' => $providers,
      '#required' => TRUE,
      '#ajax' => array(
        'wrapper' => 'provider-properties',
        'callback' => '_iwapi_ajax_configurable_callback',
        'ref_element' => array('type' => 'sibling', 'value' => array('properties')),  
      ),
    ),
    'label' => array(
      '#type' => 'textfield',
      '#title' => t('Instance label'),
      '#size' => 24,
    ),
    'name' => array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#size' => 24,
      '#maxlength' => 32,
      '#required' => TRUE,
      '#machine_name' => array(
        'exists' => 'iwsearch_instance_load',
        'source' => array('label'),
      ),
    ),
    'properties' => array(
      '#type' => 'container',
      '#attributes' => array('id' => 'provider-properties'),
      '#parents' => array(),
    ),
  );

  if (empty($provider)) {
    drupal_set_title(t('Create New Search Instance'));
    
    $ptype = $values['provider'];
    if (!empty($values['provider'])) {
      if ($ptype == get_class($form_state['provider'])) {
        $provider = $form_state['provider'];
      }
      else {
        // If it doesn't exist or is of the wrong type, we need to rebuild it.
        $provider = (isset($form_state['provider']) && get_class($form_state['provider']) == $ptype)
          ? $form_state['provider'] : ($form_state['provider'] = new $ptype($values));
      }
    }
  }
  else {
    drupal_set_title(t('Edit %label Search', array('%label' => $provider->label)), PASS_THROUGH);
    
    if (isset($form_state['provider'])) {
      $provider = $form_state['provider'];
    }
    else {
      $form_state['provider'] = $provider;
    }
    
    // These are items that can't be changed once saved.
    $form['name']['#value']        = $provider->getName();
    $form['name']['#disabled']     = TRUE;
    $form['provider']['#value']    = get_class($provider);
    $form['provider']['#disabled'] = TRUE;
  }
  
  // Append the provider specific items
  if ($provider instanceof iwSearchProvider) {
    $provider->appendElements($form['properties'], $form, $form_state);
    
    $form['label']['#default_value'] = $provider->label;
    $form['name']['#default_value']  = $provider->getName();
  }

  // Submission options for the search profile.
  $form['actions'] = array(
    '#type' => 'actions',
      
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/search/iwsearch',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );

  return $form;
}

/**
 * Validate the values used to create the search provider.
 */
function iwsearch_instance_edit_form_validate($form, &$form_state) {
  if (isset($form_state['provider'])) {
    $provider = $form_state['provider'];
    $values   = &$form_state['values']; 
    
    $form_state['provider']->appendElements($form['properties'], $values, $form_state);
  }
}

/**
 * Form submit callback to generate a new iwSearchProvider instance.
 * 
 * @param array $form
 *  The form structure used to generate the add form.
 * @param array $form_state
 *  Current state and values of the form.
 */
function iwsearch_instance_edit_form_submit(array $form, array &$form_state) {
  $values   = &$form_state['values'];
  
  // No provider was specified, rebuild the form with provider information.
  if (!isset($form_state['provider'])) {
    $form_state['rebuild'] = TRUE;
    return;
  }
    
  try {
    $provider = $form_state['provider'];
    $provider->submitElements($form['properties'], $values, $form_state);
    $provider->save();
    
    // Changes cause the stash to get emptied.
    $stash = new iwSearchStashController();
    $stash->delete($provider);
    
    // Redirect to the newly build search instance edit form.
    $form_state['redirect'] = 'admin/config/search/iwsearch';
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    drupal_set_message(t(
      'Unable to create new search instance. Administrators should check for details on the error.'
    ));
  }
}


// ==============================
// Edit search instances
// ==============================

/**
 * Form builder callback to generate the search
 * behavior editing page
 */
function iwsearch_instance_fields_form(array $form, array &$form_state, iwSearchProvider $provider) {
  form_load_include($form_state, 'inc', 'iwapi', 'loadable/ajax');
  
  $form['#attached']['css'][]     = drupal_get_path('module', 'iwsearch') .'/theme/css/search-admin.css';
  $form['#attached']['library'][] = array('iwapi', 'iwapi.ajax');

  // Check / manage stash and form values.
  $stash   = new iwSearchStashController();
  $stashed = $stash->load($provider);
  $values  = isset($form_state['values']) ? $form_state['values'] : array('elements' => array());
  
  $form['stash_msg'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'stash-messages'),
  );
  
  if ($stashed) {
    $provider = $stashed->data;

    $form['stash_msg']['#attributes']['class'] = array('messages', 'warning');
    $form['stash_msg']['message'] = array(
      '#markup' => t(
        'Changes are pending and will not take effect until '.
        'saved. You can cancel these changes at anytime.'
      ),
    );
  }

  $available = $provider->getAvailFilters();
  $uri_base  = 'admin/config/search/iwsearch/'. strtr($provider->getName(), '_', '-') .'/elements/nojs';
  
  $form['elements'] = array(
    '#type' => 'table_orderable',
    '#title' => t('Search Fields'),
    '#header' => array(t('Field'), t('Type'), t('Summary'), t('Actions')),
  );
  
  // Bundles element is special and always needs to get updated.
  if (isset($provider->elements['bundles'])) {
    $provider->elements['bundles']->allowedBundles = $provider->getBundles();
  }
  $form_state['provider'] = $provider;
  
  foreach ($provider->elements as $fieldname => $element) {
    $field_uri = strtr($fieldname, '_', '-');
 
    // Build some helpful variable information.      
    $form['elements'][$fieldname] = array(
      'field' => array('#markup' => $fieldname),
      'type-wrapper' => array(
        'type' => array(
          '#type' => 'select',
          '#required' => TRUE,
          '#options' => $available[$fieldname]['types'],
          '#default_value' => get_class($element),
          '#parents' => array('elements', $fieldname, 'type'),
          '#ajax' => array(
            'trigger_as' => array('name' => 'update-elements-'. $fieldname),
            'progress' => 'none',
          ),
        ),
        'update' => array(
          '#type' => 'submit',
          '#value' => t('Update'),
          '#submit' => array('_iwsearch_update_element_field_type'),
          '#name' => 'update-elements-'. $fieldname,
          '#limit_validation_errors' => array(array('elements', $fieldname)),
          '#attributes' => array('class' => array('element-invisible')),
          '#ajax' => array(
            'callback' => '_iwsearch_update_element_field_type_ajax',
            'progress' => 'none',
          ),
        ),
      ),
      'summary' => array(
        '#cell_attributes' => array('class' => array('iwpackage-summary')),
        'data' => $element->formatSummary(),
      ),
      'actions' => array(
        '#theme' => 'links',
        '#attributes' => array('class' => array('inline')),
        '#links' => array(
          'edit' => array(
            'title' => t('Edit'),
            'href' => "$uri_base/$field_uri/edit",
            'attributes' => array('class' => array('use-ajax')),
          ),
          'delete' => array(
            'title' => t('Delete'),
            'href' => "$uri_base/$field_uri/delete",
            'attributes' => array('class' => array('use-ajax')),
          ),
        ),
      ),
    );
  }
    
  // Allow the addition of new fields. If any are available.
  $remaining = array_diff_key($available, $form['elements']);
  if (!empty($remaining)) {
    $widgets = array();

    if (isset($values['elements']['__add_new']['field'])) {
      $fieldname = $values['elements']['__add_new']['field'];

      // What widget / element type handles this.
      if (isset($remaining[$fieldname]['types'])) {
        $widgets = $remaining[$fieldname]['types'];
      }
    }

    // Provide the option to add more elements.
    $form['elements']['__add_new'] = array(
      'field' => array(
        '#type' => 'select',
        '#prefix' => '<div class="add-new-placeholder">'. t('Add search field') .'</div>',
        '#empty_option' => t('- Select a field -'),
        '#options' => _iwapi_extract_field($remaining, 'fieldName'),
        '#ajax' => array(
          'wrapper' => 'iwsearch-add-field-type',
          'callback' => '_iwapi_ajax_configurable_callback',
          'ref_element' => array('type' => 'sibling', 'value' => array('type')),
          'progress' => 'none',
        ),
      ),
      'type' => array(
        '#type' => 'select',
        '#prefix' => '<div id="iwsearch-add-field-type"><div class="add-new-placeholder">&nbsp</div>',
        '#suffix' => '</div>',
        '#empty_option' => t('- Select the widget -'),
        '#required' => !empty($widgets),
        '#disabled' => empty($widgets),
        '#options' => $widgets,
      ),
      'summary' => array(
        '#prefix' => '<div class="add-new-placeholder">&nbsp</div>',
        '#markup' => t('Add a new search field by selecting a field and widget type.'),
      ),
      'action' => array('#markup' => '&nbsp;'),
    );
  }
  
  // Add the proper submission information.
  $form['actions'] = array(
    '#type' => 'actions',

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel' => array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#submit' => array('_iwsearch_instance_fields_cancel'),
      '#attributes' => array('class' => array('cancel-action')),
      '#weight' => 10,
    ),
  );
  
  return $form;
}

/**
 * Form validation callback to ensure that search field settings
 * are valid. This will ensure all values set are clean and correct.
 */
function iwsearch_instance_fields_form_validate($form, &$form_state) {
  $provider  = $form_state['provider'];
  $values    = &$form_state['values']['elements'];
  $form_elem = $form['elements'];
  
  foreach ($provider->elements as $name => $element) {
    $element->attachValidate($form_elem[$name], $values[$name], $form_state);
  }
}

/**
 * Form submit callback to save changes to the search provider
 * instance. This will save the elements ordering and add new
 * field elements if a new one was added.
 */
function iwsearch_instance_fields_form_submit($form, &$form_state) {
  $values = &$form_state['values'];
  
  try {
    $elements  = array();
    $stash     = new iwSearchStashController();
    $provider  = $stash->fetch($form_state['provider']);
    $available = $provider->getAvailFilters();

    // Re-order, rebuild or just refresh the list of elements.
    foreach ($values['elements'] as $key => $data) {
      if (isset($available[$key]) && isset($provider->elements[$key])) {
        $provider->updateFieldElement($key, $data);
        $elements[$key] = $provider->elements[$key];
      }    
      elseif ($key == '__add_new') { // Add the new field.
        if (!empty($data['field']) && isset($available[$data['field']])) {
          $key = $data['field'];
          $className = $data['type'];

          $elements[$key] = new $className($available[$key]);
        }
      }
    }

    $provider->elements = $elements; // update with the new ordering or changes.
    $provider->save();
    
    $stash->delete($provider); // clean the slate.
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    drupal_set_message(t(
      'Unable to update and save this search instance. Administrators '.
      'should check logs for detailed information on the error.'
    ), 'error');
  }
}

/**
 * Form submit callback to reset pending changes from
 * the iwSearch instance.
 */
function _iwsearch_instance_fields_cancel($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Cancel'):
      $form_state['redirect'] = 'admin/config/search/iwsearch';
    case t('Reset'):
      try {
        $stash = new iwSearchStashController();
        $stash->delete($form_state['provider']);
      
        drupal_set_message(t('Pending search changes have been cleared.'));
      }
      catch (Exception $e) {
        unset($form_state['redirect']);
        watchdog_exception('iwsearch', $e);
        drupal_set_message(t(
          'Unable to purge pending changes due to an internal error. '.
          'Administrators are advised to check the logs for details.'
        ), 'error');
      }
  }
}

/**
 * Form submit callback to rebuild the field information
 * because the class handler type has been changed.
 */
function _iwsearch_update_element_field_type(&$form, &$form_state) {
  // Determine who was triggered and what changed.
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#parents'];
  $parts   = array_splice($parents, -3);
  
  // Get the setup data, and request that the provider update it
  $key  = array_shift($parts);
  $data = $form_state['values']['elements'][$key];
  unset($data['weight'], $data['type-wrapper']);
  
  // Update the field to use the new type data.
  $stash    = new iwSearchStashController();
  $provider = $stash->fetch($form_state['provider']);
  $provider->updateFieldElement($key, $data);
  $stash->save($provider);
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Form AJAX callback to change the summary information
 * pertaining to a changed field type.
 */
function _iwsearch_update_element_field_type_ajax($form, $form_state) {
  // Determine who was triggered and what changed.
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#parents'];
  $parts   = array_splice($parents, -3);
  $key     = array_shift($parts);
  
  $stash    = new iwSearchStashController();
  $provider = $stash->fetch($form_state['provider']);
  
  // Update the field to use the new type data.
  $summary  = $provider->elements[$key]->formatSummary();
  $table    = $form['search_by']['elements'];
  $table_id = drupal_html_id('edit-' . implode('-', $table['#parents']));
  
  // Add command to replace the summary information.
  return array('#type' => 'ajax', '#commands' => array(
    ajax_command_replace('#stash-messages', render($form['stash_msg'])),
    ajax_command_html("#$table_id-row-$key .iwpackage-summary", render($summary)),
  ));
}


// ======================================
// Results page setup
// ======================================

/**
 * Page callback which displays the available results
 * display pages, and provides a simple add form.
 */
function _iwsearch_instance_results_overview_form($form, &$form_state, iwSearchProvider $provider) {
  $rs = db_select('iwsearch_results', 'result')
    ->fields('result', array('id', 'title', 'label', 'handler', 'uri'))
    ->condition('name', $provider->getName())
    ->orderBy('result.label')
    ->execute();
  
  $rows = array();
  
  require_once dirname(__DIR__) .'/includes/results_handlers.inc';
  $handlers = _iwsearch_get_results_handlers();
  
  $base_uri = 'admin/config/search/iwsearch/'. $provider->getName() .'/results';
  foreach ($rs as $result) {    
    $rows[] = array(
      'title' => check_plain($result->title),
      'label' => check_plain($result->label),
      'handler' => $handlers[$result->handler]['label'],
      'uri' => '/'. $result->uri,
      'actions' => theme('links', array(
        'attributes' => array('class' => array('inline')),
        'links' => array(
          'edit'   => array('title' => 'edit', 'href' => $base_uri ."/{$result->id}/edit"),
          'delete' => array('title' => 'delete', 'href' => $base_uri ."/{$result->id}/delete"),
        ),
      )),
    );
  }
  
  $form['current_pages'] = array(
    '#theme' => 'table',
    '#header' => array(t('Title'), t('Label'), t('Type'), t('URI'), t('Actions')),
    '#rows' => $rows,
    '#empty' => t('There are no results pages, you can add the first below.'),
  );
  
  $form['add_new'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Results Page'),
    
    'name' => array(
      '#type' => 'value',
      '#value' => $provider->getName(),
    ),
    'handler' => array(
      '#type' => 'select',
      '#title' => t('Results display type'),
      '#required' => TRUE,
      '#options' => _iwapi_extract_field($handlers),
    ),
    'title' => array(
      '#type' => 'textfield',
      '#title' => t('Page title'),
      '#required' => TRUE,
    ),
    'label' => array(
      '#type' => 'textfield',
      '#title' => t('Results setting label'),
      '#required' => TRUE,
    ),
    'uri' => array(
      '#type' => 'textfield',
      '#title' => t('URI'),
      '#field_prefix' => 'http://'. $_SERVER['HTTP_HOST'] .'/',
      '#required' => TRUE,
      '#element_validate' => array('iwsearch_element_validate_result_path'),
    ),
    
    'create' => array(
      '#type' => 'submit',
      '#value' => t('Create Page'),
      '#submit' => array('iwsearch_instance_results_add_submit'),
    ),
  );
  
  return $form;
}

/**
 * Form submit callback to generate a new search page
 * and redirect to the edit form.
 */
function iwsearch_instance_results_add_submit($form, &$form_state) {
  $values = $form_state['values'];
  
  try {
    $className = $values['handler'];
    
    $result = new $className(iwsearch_instance_load($values['name']), $values);
    $result->save();
    
    $form_state['redirect'] = "admin/config/search/iwsearch/$values[name]/results/$result->id/edit";
  }
  catch (Exception $e) {
    drupal_set_message('Error while saving new search results page.');
  }
}

/**
 * Form builder to manage how results are handled with
 * this search provider.
 */
function iwsearch_instance_results_form($form, &$form_state, iwSearchResults $results) {
  form_load_include($form_state, 'inc', 'iwapi', 'loadable/ajax');
  
  // Ensure we are using the correct version of results handler.
  if (isset($form_state['search_results'])) {
    $results = $form_state['search_results'];
  }
  else {
    $form_state['search_results'] = $results;
  }
  
  $form = array();
  $results->appendElements($form, $form, $form_state);
  
  // set the save functions.
  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/search/iwsearch',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );

  return $form;
}

/**
 * Form validation callback to ensure results settings are properly checked.
 */
function iwsearch_instance_results_form_validate($form, &$form_state) {
  if (isset($form_state['search_results'])) {
    $results = $form_state['search_results'];
    
    $results->validateElements($form, $form_state['values'], $form_state);
  }
}

/**
 * 
 */
function iwsearch_instance_results_field_config($field) {
  $field += array(
    'display' => array('label' => 'above'),
    'cardinality' => 1,
  );
  
  return array(
    'label' => array(
      '#type' => 'textfield',
      '#title' => t('Field label'),
      '#size' => 16,
      '#default_value' => $field['title'],
    ),
    'settings' => array(
      'label' => array(
        '#type' => 'select',
        '#title' => t('Display type'),
        '#options' => array(
          'hidden' => t('Invisible'),
          'above' => t('Above'),
          'inline' => t('Inline'),
        ),
        '#default_value' => $field['display']['label'],
      ),
    ),
    'cardinality' => array(
      '#type' => 'textfield',
      '#title' => t('Max. items'),
      '#size' => 5,
      '#element_validate' => array('element_validate_integer'),
      '#default_value' => $field['cardinality'],
      '#description' => t('0 means unlimited')
    ),
  );
}

/**
 * Form builder callback which will save the results settings.
 */
function iwsearch_instance_results_form_submit($form, &$form_state) {
  $values   = &$form_state['values'];
  $results  = $form_state['search_results'];
  
  $results->submitElements($form, $values, $form_state);
  
  if ($form_state['values']['op'] === t('Save')) {
    try {
      $results->save();

      $form_state['redirect'] = 'admin/config/search/iwsearch';
    }
    catch (Exception $e) {
      watchdog_exception('iwsearch', $e);
      drupal_set_message(t('Unable to save settings'), 'error');
    }
  }
}


// ======================================
// Delete search instance confirm
// ======================================

/**
 * Form callback to generate a delete callback to
 * remove a Search provider instance.
 */
function iwsearch_instance_delete_confirm($form, array &$form_state, iwSearchProvider $provider) {
  $title = 'Are you sure you want to delete search %name?';
  drupal_set_title(t($title, array('%name' => $provider->label)), PASS_THROUGH);

  // Stash to retrieve later.
  $form_state['provider'] = $provider;
  
  // Warn users of what they are potentially doing.
  $form['message'] = array(
    '#markup' => t('This action cannot be undone. One removed this search will no longer be available.'),
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'delete' => array(
      '#type' => 'submit',
      '#value' => t('Confirm Delete'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/search/iwsearch',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  
  return $form;
}

/**
 * Form submit callback to remove the search instance from
 * existance. At this point the user has confirmed the
 * deletion and it can be "safely" completed. 
 */
function iwsearch_instance_delete_confirm_submit($form, array &$form_state) {
  $trans = db_transaction();
  try {
    // Find and remove all result pages that depend on this provider.
    $rs = db_select('iwsearch_results', 'r')
      ->fields('r', array('id'))
      ->condition('name', $form_state['provider']->getName())
      ->execute()->fetchCol();
    
    foreach ($rs as $id) {
      $handler = iwsearch_result_load($id);
      $handler->delete();
    }
    
    $form_state['provider']->delete();
    
    $stash = new iwSearchStashController();
    $stash->delete($form_state['provider']);
    
    menu_rebuild(); // Potentially we are destroying search pages.
    
    $form_state['redirect'] = 'admin/config/search/iwsearch';
  }
  catch (Exception $e) {
    $trans->rollback();

    watchdog_exception('iwsearch', $e);
    drupal_set_message(t(
      'Unable to delete this search instance. Administrators should '.
      'check the logs for detailed information about the error.'
    ), 'error');
  }
}
