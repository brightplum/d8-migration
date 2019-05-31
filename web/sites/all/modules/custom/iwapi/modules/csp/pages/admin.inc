<?php
/**
 * @file csp.admin.inc
 *  Provides the administration forms for CSP instances.
 */


/**
 * The overview form to display available workflows and add new ones.
 */
function csp_workflow_overview_page() {
  $page['workflows'] = array(
    '#theme' => 'table',
    '#header' => array(t('Workflow'), t('Administrators'), t('States'), t('Actions')),
    '#empty' => t('There are currently no workflows on this site. Add a new one !link.', array(
      '!link' => l('here', 'admin/config/content/csp/add-workflow'),
    )),
  );
  
  $ajax_link['attributes'] = array('class' => array('use-ajax'));
  $flow_ids = db_select('csp_flows', 'fl')
    ->fields('fl', array('flow'))
    ->orderBy('label', 'ASC')
    ->execute()->fetchCol();
  
  $rows = array();
  foreach (CspController::LoadMultiple($flow_ids) as $id => $flow) {
    $states = array_map('_csp_workflow_format_state', $flow->states);

    $rows[] = array(
      'flow' => check_plain($flow->label) ."<small>($flow->name)</small>",
      'admins' => check_plain(implode(', ', _iwapi_extract_field($flow->getAdmins(), 'name'))),
      'states' => theme('item_list', array('items' => $states)),
      'actions' => theme('links', array(
        'links' => array(
          'edit' => array('title' => t('Edit'), 'href' => "admin/config/content/csp/$id/edit"),
          'delete' => array(
            'title' => t('Delete'),
            'href' => "admin/config/content/csp/no-js/$id/delete", $ajax_link),
        ),
        'attributes' => array('class' => array('inline')),
      )),
    );
  }
  
  $page['workflows']['#rows'] = $rows;
  return $page;
}


// ===============================
// Edit workflow properties
// ===============================

/**
 * Menu page callback to use or generate a CspFlow
 * object to use with the workflow edit form.
 * 
 * @param CspFlow $flow
 *  The workflow object to use for editing. NULL means
 *  to create a new one.
 * @return array
 *  Form structure array to be used to render
 *  the edit workflow form. 
 */
function csp_workflow_edit_page($flow = NULL) {
  if (!isset($flow)) {
    $flow = new CspFlow(array());
  }
  
  // Return the edit form for managing workflow settings.
  return drupal_get_form('csp_workflow_edit_form', $flow);  
}

/**
 * Edit workflow settings and states.
 */
function csp_workflow_edit_form($form, &$form_state, Cspflow $workflow) {  
  if (isset($form_state['workflow'])) {
    $workflow = $form_state['workflow'];
  }
  else {
    $form_state['workflow'] = $workflow;
  }
  
  $form['#attached'] = array(
    'library' => array(array('iwapi', 'iwapi.ajax')),
  );
  
  
  if (!empty($workflow->flow)) {
    $form['flow'] = array(
      '#type' => 'value',
      '#value' => $workflow->flow,
    );
  }
  
  // User friendly label
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#size' => 32,
    '#require' => TRUE,
    '#default_value' => $workflow->label,
    '#description' => t('Human friendly display name for this workflow.'),
  );
  
  // Machine name for this workflow
  $form['name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#size' => 32,
    '#maxlength' => 32,
    '#disabled' => !empty($workflow->name),
    '#machine_name' => array(
      'source' => array('label'),
      'exists' => '_csp_workflow_name_exists',
    ),
    '#default_value' => $workflow->name,
  );

  $form['states'] = array(
    '#type' => 'table_orderable',
    '#header' => array(t('State'), t('Publishes'), t('Administrative'), t('Actions')),
    '#description' => t(
      'Note when ordering states, that the first state will be used as the '.
      'default / starting state. This is important when states are deleted, as '.
      'content using the deleted state will be converted to use the default state.'
    ),
  );
  
  foreach ($workflow->states as $name => $info) {
    $form['states'][$name] = array(
      'label' => array(
        '#type' => 'textfield',
        '#size' => 16,
        '#default_value' => $info['label'],
        '#suffix' => "<div style=\"margin-left: 24px;\"><small>(Machine: $name)</small></div>",
      ),
      CspFlow::FLAG_PUBLISH => array(
        '#type' => 'checkbox',
        '#default_value' => (bool)($info['flags'] & CspFlow::FLAG_PUBLISH),
      ),
      CspFlow::FLAG_ADMIN => array(
        '#type' => 'checkbox',
        '#default_value' => (bool)($info['flags'] & CspFlow::FLAG_ADMIN),
      ),
      'remove' => array(
        '#type' => 'submit',
        '#name' => 'remove-state-'. $name,
        '#value' => t('Remove'),
        '#validate' => array(),
        '#submit' => array('_csp_workflow_state_remove_submit'),
        '#ajax' => array('callback' => '_csp_workflow_row_remove_ajax'),
      ),
    );
  }
  
  // Special row for adding new form element.
  $form['states']['__add_state'] = array(
    'naming' => array(
      'label' => array(
        '#type' => 'textfield',
        '#size' => 16,
        '#maxlength' => 255,
        '#parents' => array('states', '__add_state', 'label'),
      ),
      'name' => array(
        '#type' => 'machine_name',
        '#size' => 20,
        '#maxlength' => 32,
        '#required' => FALSE,
        '#parents' => array('states', '__add_state', 'name'),
        '#machine_name' => array(
          'source' => array('states', '__add_state', 'naming', 'label'),
          'label' => t('machine name'),
          'exists' => '_csp_workflow_state_exists',
          'standalone' => TRUE,
        ),
      ),
    ),
    CspFlow::FLAG_PUBLISH => array('#type' => 'checkbox'),
    CspFlow::FLAG_ADMIN   => array('#type' => 'checkbox'),
    'add_state' => array(
      '#type' => 'submit',
      '#value' => t('Add State'),
      '#submit' => array('_csp_workflow_state_add_submit'),
      '#validate' => array('_csp_workflow_state_add_validate'),
      '#limit_validation_errors' => array(array('states')),
    ),
  );
  
  
  // Administrators for this workflow.
  $form['admins'] = array(
    '#type' => 'table_elements',
    '#title' => t('Workflow Administrators'),    
  );
  
  foreach ($workflow->getAdmins() as $uid => $admin) {
    $form['admins'][$uid] = array(
      'adminstrator' => array('#markup' => theme('username', array('account' => $admin))),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "administrator-{$uid}-remove",
        '#submit' => array('_csp_workflow_admin_remove_submit'),
        '#validate' => array(),
        '#limit_validation_errors' => array(),
        '#ajax' => array('callback' => '_csp_workflow_row_remove_ajax'), 
      ),
    );
  }
  
  $form['admins']['__add_admin'] = array(
    'username' => array(
      '#type' => 'textfield',
      '#title' => t('User name'),
      '#size' => 40,
      '#autocomplete_path' => 'user/autocomplete',
    ),
    'add_admin_submit' => array(
      '#type' => 'submit',
      '#value' => t('Add Administrator'),
      '#submit' => array('_csp_workflow_admin_add_submit'),
      '#validate' => array('_csp_workflow_admin_add_validate'),
      '#limit_validation_errors' => array(array('admins', '__add_admin')),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#validate' => array('_csp_workflow_admin_add_validate'),
      '#submit' => array(
        '_csp_workflow_state_add_submit',
        '_csp_workflow_admin_add_submit',
        'csp_workflow_edit_form_submit',
      ),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/content/csp',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  
  // Clear previous entries.
  unset($form_state['input']['states']['__add_state']);
  unset($form_state['input']['admins']['__add_admin']);
  return $form;
}

/**
 * Form submit callback to save the workflow changes.
 */
function csp_workflow_edit_form_submit($form, &$form_state) {
  $workflow = $form_state['workflow'];
  
  if (empty($workflow->name)) {
    $workflow->name  = $form_state['values']['name'];
  }
  $workflow->label = $form_state['values']['label'];
  
  try {
    $workflow->save();
    
    $form_state['redirect'] = 'admin/config/content/csp';
    $form_state['rebuild']  = FALSE;
    
    drupal_set_message(t('Workflow changes have been successfully changed.'));
  }
  catch (Exception $e) {
    drupal_set_message(t('Unable to save workflow changes.'), 'error');
    watchdog_exception('csp', $e);
  }
}

// ============================
// Workflow add state
// ============================

/**
 * Form state validation callback for adding a new state.
 */
function _csp_workflow_state_add_validate($form, &$form_state) {
  if (empty($form_state['values']['states']['__add_state']['name'])) {
    form_error($form['states']['__add_state']['naming'], t(
      'The machine name for this state cannot be blank.'
    ));
  }
}

/**
 * Add a new state to the work flow in memory.
 */
function _csp_workflow_state_add_submit($form, &$form_state) {
  $states = array();
  $flags  = array_fill_keys(array(CspFlow::FLAG_ADMIN, CspFlow::FLAG_PUBLISH), TRUE);
  
  foreach ($form_state['values']['states'] as $name => $values) {
    $key = ($name === '__add_state') ? $values['name'] : $name;
    if (empty($key)) {
      continue;
    }
    
    $states[$key] = array(
      'label' => $values['label'],
      'flags' => 0,
    );

    // Successively apply various flags.
    $filters = array_filter(array_intersect_key($values, $flags));
    foreach ($filters as $flag => $val) {
      $states[$key]['flags'] |= $flag;
    }
  }
  
  $form_state['workflow']->states = $states;
  $form_state['rebuild'] = TRUE;
}

/**
 * Check if the state name within a workflow already exists.
 * 
 * @param $name
 *  The state name to check.
 * @param $element
 *  The machine name form element structure / values.
 * @param $form_state
 *  Current status of the entire form. Here is where we'll
 *  get the workflow values and state.
 */
function _csp_workflow_state_exists($name) {  // Attempt to determine some of the context information required.
  $item = menu_get_item();

  if ($item['page_callback'] == 'ajax_form_callback') {
    list($form, $form_state) = ajax_get_form();

    $workflow = $form_state['workflow'];
  }
  elseif (!empty($item['page_arguments'])) {
    $workflow = reset($item['page_arguments']);
  }
  else {
    return FALSE;
  }
  
  return array_key_exists($name, $workflow->states);
}


// =============================
// Remove workflow state
// =============================

/**
 * Form submit callback to remove a state information.
 */
function _csp_workflow_state_remove_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  
  if (preg_match('#remove-state-(.+)#', $trigger['#name'], $matches)) {
    unset($form_state['workflow']->states[$matches[1]]);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Form AJAX callback to remove a row from the table. This
 * will remove the row wrapping the input that triggers the submit.
 */
function _csp_workflow_row_remove_ajax($form, &$form_state) {
  iwapi_include('ajax');
  
  return array(
    '#type' => 'ajax',
    '#commands' => array(iwapi_ajax_command_delete_tablerow()),
  );
}


// ============================
// Add Administrator
// ============================

/**
 * Form validation callback to check if a user
 * actually exists, and translates the name to ID.
 */
function _csp_workflow_admin_add_validate($form, &$form_state) {
  $add_values = &$form_state['values']['admins']['__add_admin'];

  if (!empty($add_values['username'])) {
    $add_values['uids'] = db_select('users', 'u')
      ->fields('u', array('uid'))
      ->condition('name', db_like($add_values['username']), 'LIKE')
      ->condition('status', 1)
      ->execute()->fetchCol();
    
    // Check if any users were found.
    if (empty($add_values['uids'])) {
      form_error($form['admins']['__add_admin'], t(
        'Unable to add %name as an administator. Account could not be found.',
        array('%name' => $add_values['username'])
      ));
    }
  }
}

/**
 * Form submit callback for adding a administrator.
 */
function _csp_workflow_admin_add_submit($form, &$form_state) {
  $workflow   = $form_state['workflow'];
  $add_values = $form_state['values']['admins']['__add_admin'];
  
  // Add to the list of administrators available.
  if (!empty($add_values['uids'])) {
    $admins = &$workflow->getAdmins();
    $admins += user_load_multiple($add_values['uids']);
  }
  
  // Request to rebuild the admins array.
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit callback to remove an administrator.
 */
function _csp_workflow_admin_remove_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  
  if (preg_match('/admin-(\d+)-remove/', $trigger['#name'], $matches)) {
    $admins = &$form_state['workflow']->getAdmins();
    unset($admins[intval($matches[1])]);
  }
}


// ============================
// Delete workflow
// ============================

/**
 * Create the confirmation page before deleting.
 */
function csp_workflow_delete_confirm_page(CspFlow $workflow, $ajax = FALSE) {
  $form_id = 'csp_workflow_delete_confirm';
    
  if ($ajax) {
    $state = array();
    $commands = array();
    
    iwapi_include('ajax');
    $form  = _iwapi_ajax_get_form($form_id, $state, $workflow);
    
    // Check the submissions status of the form, and build appropriate command.
    if (!$command = iwapi_ajax_form_complete($state, 'delete_tablerow', TRUE)) {
      $commands[] = iwapi_ajax_command_dialog(drupal_get_title(), array(
        'messages' => array('#theme' => 'status_messages'), 
        'form' => $form,
      ));
    }
 
    return array('#type' => 'ajax', '#commands' => $commands);
  }
  else {
    return drupal_get_form($form_id, $workflow);
  }
}

/**
 * Generate a confirmation form for deleting a workflow
 * 
 * @param $info
 *  Information about the workflow to be deleted
 */
function csp_workflow_delete_confirm($form, &$form_state, CspFlow $flow) {
  $form_state['workflow']    = $flow;
  $form_state['return_link'] = 'admin/config/content/csp';
  
  drupal_set_title(t('Are you sure you wish to delete the %label workflow.', array($flow->label)), PASS_THROUGH);
  
  $form['details'] = array(
    '#theme' => 'item_list',
    '#title' => t('Workflow phases:'),
    '#items' => array_map('_csp_workflow_format_state', $flow->states),
  );
  
  $form['message'] = array(
    '#prefix' => '<div class="delete-confirm-msg"><p>',
    '#suffix' => '</p></div>',
    '#markup' => t(
      'Are you sure you wish to delete @name workflow? This action cannot be undone',
      array('@name' => $flow->name)
    ),
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Confirm Delete'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'admin/config/content/csp',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  return confirm_form($form, $question, 'admin/config/content/csp');
}

/**
 * User has confirmed that they want to delete the selected workflow,
 *  clear out the {csp} and {csp_node} tables of referenced items.
 *  Also clear the related variable from the {variables} table.
 */
function csp_workflow_delete_confirm_submit($form, &$form_state) {
  $workflow = $form_state['workflow'];
    
  try {
    $workflow->delete();
    
    // Take this workflow out of commission for all entity types.
    foreach (entity_get_info() as $type => $info) {
      $settings = variable_get("csp_{$type}_bundles", NULL);
 
      if ($settings) {
        foreach ($settings as $bundle => $flows) {
          $tmp = array_flip($flows);
          unset($tmp[$workflow->flow]);

          $settings[$bundle] = array_flip($tmp);
        }
        variable_set("csp_{$type}_bundles", $settings);
      }
    }
    
    $form_state['redirect'] = 'admin/config/content/csp';
    drupal_set_message(t('Workflow @name was removed.', array('@name' => $workflow->label)));
  }
  catch (Exception $e) {
    watchdog_exception('csp', $e);
  }
}


// ================================
// Entity type handling
// ================================

/**
 * Manage which workflows belong with each entity type.
 */
function csp_workflow_manage_types($form, &$form_state, $entity_type) {
  $form_state['entity_type'] = $entity_type;

  $entity_info = entity_get_info($entity_type);
  $bundles     = _iwapi_extract_field($entity_info['bundles']);

  $rs = db_select('csp_flows', 'fl')
    ->fields('fl', array('flow', 'label', 'publishes'))
    ->orderBy('label', 'ASC')
    ->execute();

  $workflows = array();
  foreach ($rs as $flow_info) {
    $workflows[$flow_info->flow] = t(check_plain($flow_info->label));

    if ($flow_info->publishes) {
      $workflows[$flow_info->flow] .= t(' (publishes)');
    }
  }

  $form['associations'] = array(
    '#type' => 'table_elements',
    '#title' => t('Workflow associations'),
    '#header' => array(t('Bundle'), t('Workflow')),
    '#empty' => t(
      'There are currently no elgible bundles for <strong>@entity_type</strong> types',
      array('@entity_type' => $entity_info['label'])
    ),
  );

  // Build a table of values
  $previous = variable_get("csp_{$entity_type}_bundles", array());
  foreach ($bundles as $bundle => $name) {
    $form['associations'][$bundle] = array(
      'name' => array(
        '#markup' => t('@label <small>(%type)</small>', array('@label' => $name, '%type' => $bundle)),
      ),
      'workflow' => array(
        '#type' => 'select',
        '#options' => $workflows,
        '#empty_option' => t('- No Workflow -'),
        '#default_value' => empty($previous[$bundle]) ? 0 : reset($previous[$bundle]),
      ),
    );
  }
  
  $form['actions'] = array(
    '#type' => 'actions',
    'save_action' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel_action' => array(
      '#type' => 'link',
      '#title' => 'Cancel',
      '#href' => 'admin/config/content/csp',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    ),
  );
  return $form;
}

/**
 * Submit the node type associations to workflows.
 */
function csp_workflow_manage_types_submit($form, &$form_state) {
  $type = $form_state['entity_type'];
  $entity_info = entity_get_info($type);
  
  $values = $form_state['values']['associations'];
  $workflows = _iwapi_extract_field($values, 'workflow');
  
  $by_flow = array();
  $settings = array_filter($workflows);
  foreach ($settings as $bundle => $flow) {
    $settings[$bundle] = array(intval($flow));
    $by_flow[$flow][] = $bundle;
  }
  
  // Save the new workflow changes.
  variable_set("csp_{$type}_bundles", $settings);
  
  drupal_set_message(t(
    'Changes for %type bundles have been saved.',
    array('%type' => $entity_info['label'])
  ));
  
  // Generate the batch which will set default values for published entities.
  $batch = array(
    'operations' => array(),
    'finished' => '_csp_batch_set_state_finished',
    'title' => t('Processing Workflow updating batch'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Example Batch has encountered an error.'),
    'file' => drupal_get_path('module', 'csp') .'/pages/admin.inc',
  );

  foreach($by_flow as $flow => $bundles) {
    $batch['operations'][] = array(
      '_csp_batch_set_default_state',
      array($flow, $type, $bundles),
    );
  }

  batch_set($batch);
}


// ============================
// Utility and helpers
// ============================

/**
 * Check if this machine name already exists among CSP workflows.
 *  Only validation needed before creating a new workflow.
 *
 * @param $name
 *  The name to test for.
 * @return boolean
 *  TRUE if name already exists, FALSE otherwise.
 */
function _csp_workflow_name_exists($name) {
  $query = db_select('csp_flows', 'fl')->condition('name', $name);
  $query->addExpression('COUNT(1)');
  
  return (bool)$query->execute()->fetchField();
}

/**
 * Nicely format the workflow state information.
 * 
 * @param array $state
 *  Information about the state to direct how the
 *  state should be displayed.
 * @return string
 *  String representation of the state information.
 */
function _csp_workflow_format_state(array $state) {
  $properties = array();
  
  if ($state['flags'] & CspFlow::FLAG_ADMIN) {
    $properties[] = t('Administrative');
  }

  if ($state['flags'] & CspFlow::FLAG_PUBLISH) {
    $properties[] = t('Publishes');
  }

  return t(
    empty($properties) ? '@label' : '@label <small>(%props)</small>',
    array('@label' => $state['label'], '%props' => implode('|', $properties))
  );
}


// =============================
// Batch processing
// =============================

/**
 * Batch processing callback function to update published entities.
 */
function _csp_batch_set_default_state($flow_id, $entity_type, $bundles, &$context) {
  $entity_info = entity_get_info($entity_type);
  $idKey = $entity_info['entity keys']['id'];

  $query = db_select($entity_info['base table'], 'ent')
    ->fields('ent', array($idKey))
    ->orderBy($idKey, 'ASC');
  
  if (!empty($entity_info['entity keys']['bundle'])) {
    $query->condition($entity_info['entity keys']['bundle'], $bundles);
  }
  
  $query->leftJoin(
    'csp_entities', 'csp',
    "csp.entity_type=:type AND csp.entity_id=ent.{$idKey} AND csp.flow=:flow",
    array(':type' => $entity_type, ':flow' => $flow_id)
  );
  $query->isNull('csp.state_id');
 
  // determine what the default values are.
  $flow = csp_workflow_load($flow_id);
  
  if (!isset($context['sandbox']['cur'])) {
    $context['sandbox']['cur'] = 0;
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = $query->countQuery()->execute()->fetchField();
    
    foreach ($flow->states as $name => $info) {
      if (($info['flags'] & CspFlow::FLAG_PUBLISH) != 0) {
        $context['sandbox']['publish_state'] = $name;
        break;
      }
    }
    
    if (empty($context['sandbox']['publish_state'])) {
      $context['finished'] = 1;  // report completion / no publish state.
      return;
    }
  }

  $query->condition($idKey, $context['sandbox']['cur'], '>')->range(0, 50);
  $ids = $query->execute()->fetchCol();
  
  if (empty($ids)) {
    $context['finished'] = 1;  // report completion / no publish state.
    return;
  }

  if ($entity_type == 'node') {
    foreach (node_load_multiple($ids) as $entity) {
      if ($entity->status == NODE_PUBLISHED) {
        $flow->saveState($entity, $entity_type, $context['sandbox']['publish_state']);
      }
      
      $context['results'][] = $entity->{$idKey} .': '. check_plain($entity->title);
      $context['sandbox']['cur'] = $entity->{$idKey};
      $context['sandbox']['progress']++;
    }
    
    $context['finished'] = 1;
  }
  elseif (!empty($entity_info['entity class']) && is_subclass_of($entity_info['entity class'], iwEntity)) {
    foreach (entity_load($entity_type, $ids) as $entity) {
      if ($entity->isActive()) {
        $flow->saveState($entity, $entity_type, $state);
      }
      
      $context['results'][] = $entity->{$idKey} .': '. entity_label($entity_type, $entity);
      $context['sandbox']['cur'] = $entity->{$idKey};
      $context['sandbox']['progress']++;
    }
  }
  
  // Update the progress.
  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * The clear entity bundle content completion callback. Reports the results
 * of the entity clearing processing.
 */
function _csp_batch_set_state_finished($success, $results, $operations) {
  // The 'success' parameter means no fatal PHP errors were detected. All
  // other error management should be handled using 'results'.
  if ($success) {
    $message = format_plural(count($results), 'One entity updated.', '@count entities updated.');
  }
  else {
    $message = t('Finished with an error.');
    
    // Providing data for the redirected page is done through $_SESSION.
    /* TODO: Figure out what to do with this stuff, should we try to 
     * report what was actually deleted? 
    foreach ($results as $result) {
      $items[] = t('Deleted entity %title.', array('%title' => $result));
    }
    $_SESSION['iwapi_clear_entity_bundle_results'] = $items;
    */
  }
  drupal_set_message($message);
}
