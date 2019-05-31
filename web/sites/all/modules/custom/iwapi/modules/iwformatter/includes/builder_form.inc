<?php
/**
 * @file
 * Contains the common content builder form handling functions.
 */


/**
 * Generate a form builder structure that uses construct
 * the IW formatter field builders.
 * 
 * @param array $form_state
 *  Form state contains the current form state parameters.
 * @param array $callbacks
 *  Context and callbacks to use for generating individual
 *  field setting forms, and elements.
 * @param bool $hasUnused
 *  Does this builder have an inactive / unused area.
 * @return array
 *  Content builder form structure.
 */
function _iwformatter_create_content_builder_form(array &$form_state, array $callbacks, $hasUnused = false) {
  form_load_include($form_state, 'inc', 'iwapi', 'loadable/ajax');
  form_load_include($form_state, 'inc', 'iwformatter', 'includes/builder_form');
  
  // Configuring a specific field / container variables.
  $configuring = isset($form_state['configuring_item']) ? $form_state['configuring_item'] : NULL;
  $entity_type = $form_state['entity_type'];
  $bundle      = $form_state['bundle_name'];
  
  // Ensure that all the administrative JS libraries are included.
  $path = drupal_get_path('module', 'iwapi');
  $form['#attached'] = array(
    'css' => array($path .'/theme/css/content-wrappers.css'),
    'library' => array(
      array('system', 'jquery.form'),
      array('iwapi', 'iwapi.ajax'),
      array('iwapi', 'iwapi.builders'),
      array('iwapi', 'iwapi.docking'),
    ),
  );
  
  $grid_size = intval(variable_get("iwformatter_{$entity_type}_{$bundle}_grid", 12));
  $form['grid_system'] = array(
    '#type' => 'select',
    '#title' => t('Grid Columns:'),
    '#options' => array('12' => '12', '16' => '16', '24' => '24'),
    '#default_value' => $grid_size,
    '#attributes' => array('id' => 'node-page-grid'),
  );
  
  // ----------------
  // Starting building the structure.
  // ----------------
  $form['canvas'] = array(
    '#prefix' => '<div id="content-canvas">',
    '#suffix' => '</div>',
    '#markup' => '<input class="container-value" type="hidden" value="<root>" />'.
      '<div class="content-container grid-size-'. $grid_size .' clearfix"></div>',
  );

  // Is there a region specified for unused fields?
  if ($hasUnused) {
    $form['unused_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Unused Items'),
      '#collapsible' => TRUE,
      
      'content' => array(
        '#markup' => '<input class="container-value" type="hidden" value="" /><div class="content-container clearfix"></div>',
      ),
    );
 
    $containers[''] = t('- Not used -');
  }
  $containers['<root>'] = '<root>';
  $containers += _iwapi_extract_field($form_state['content_wrappers']);
  
  $cmn_elements = array(
    'weight' => array(
      '#type' => 'textfield',
      '#title' => t('Weight'),
      '#size' => 5,
      '#attributes' => array('class' => array('field-weight')),
    ),
    'parent' => array(
      '#type' => 'select',
      '#title' => 'parent',
      '#options' => $containers,
      '#attributes' => array('class' => array('field-parent')),
      '#default_value' => '<root>',
    ),
  );
  
  // -------------------
  // Construct tools
  // -------------------
  $form['tools'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'edit-tools'),
  );
  
  if (!empty($callbacks['tools_callback'])) {
    $form['tools'] += $callbacks['tools_callback']($form_state, $callbacks['context']);
  }
  $form['tools'] += _iwformatter_builder_new_wrapper_form($form_state);
 
  // -------------------
  // Construct container
  // -------------------
  $form['containers'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'container-data'),
    '#tree' => TRUE,
  );

  $wrapper_types = iwFieldGroupController::GetWrapperTypes();
  foreach ($form_state['content_wrappers'] as $name => $wrapper) {
    $wrapper_uri = "{$callbacks['context']['base_uri']}/wrappers/$name";
    $wrapper_id  = 'wrapper-'. strtr($name, '_', '-');
    
    $form['containers'][$name] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => strtr($name, '_', '-') .'-container',
        'class' => array('draggable-container', 'clearfix'),
      ),
      
      'title' => array(
        '#prefix' => '<h3 class="item-handle"><span class="drag-icon"></span>',
        '#suffix' => '</h3>',
        '#weight' => -20,
        '#markup' => '<span class="item-label">'. check_plain($wrapper->label) .'</span>' 
          .' <small>('. check_plain(trim($wrapper_types[get_class($wrapper)])) .')</small>',
      ),
      'container_name' => array(
        '#type' => 'hidden',
        '#value' => $name,
        '#weight' => -10,
        '#attributes' => array('class' => array('container-value')),
      ),
      'op' => array(
        '#theme' => 'links',
        '#attributes' => array('class' => 'field-actions'),
        '#links' => array(
          'delete' => array('title' => 'delete', 'href' => $wrapper_uri ."/delete/$form_state[view_mode]")
        ),
      ),
      'configure' => array(
        '#type' => 'submit',
        '#name' => $wrapper_id,
        '#value' => html_entity_decode('&#xf013;', 0, 'UTF-8'),
        '#attributes' => array('class' => array('fa', 'draggable-field-op')),
        '#submit' => array('_iwformatter_builder_set_configure_submit'),
        '#validate' => array(),
        '#limit_validation_errors' => array(),
        '#ajax' => array('callback' => '_iwformatter_builder_set_configure_ajax'),
      ),
      'wrapper_summary' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-summary')),
      ) + $wrapper->formatSummary(),
      'settings' => array(
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array(
          'id' => $wrapper_id,
          'class' => array('content-wrapper-settings', 'clearfix'),
        ),
      ),
      'contents' => array(
        '#markup' => '<div class="content-container clearfix"></div>',
        '#weight' => 50,
      ),
    ) + $cmn_elements;
    
    $form['containers'][$name]['weight']['#default_value'] = $wrapper->weight;

    if ($wrapper->hasWidth() && $wrapper->settings['span'] > 0) {
      $form['containers'][$name]['#attributes']['class'][] = 'span-'. $wrapper->settings['span'];
    }
  }

  // Add configurations
  if (!empty($configuring) && $configuring[0] == 'containers') {
    $cont_name = $configuring[1];

    $wrapper = $form_state['content_wrappers'][$cont_name];
    $wrapper->appendElements($form['containers'][$cont_name]['settings'], $form, $form_state);
    
    $element = &$form['containers'][$cont_name];
    $element['settings']['actions'] = array(
      '#type' => 'actions',
      
      'settings_save' => array(
        '#type' => 'submit',
        '#name' => 'configuring-element-save',
        '#value' => t('Save Changes'),
        '#submit' => array(
          '_iwformatter_builder_configure_wrapper_submit',
          '_iwformatter_builder_cancel_configure_submit',
        ),
        '#validate' => array('_iwformatter_builder_configure_wrapper_validate'),
        '#limit_validation_errors' => array(array('containers', $cont_name, 'settings')),
        '#ajax' => array('callback' => '_iwformatter_builder_update_wrapper_ajax'),
      ),
      'settings_cancel' => array(
        '#type' => 'submit',
        '#name' => 'configuring-element-cancel',
        '#value' => t('Cancel'),
        '#attributes' => array('class' => array('cancel-action')),
        '#submit' => array('_iwformatter_builder_cancel_configure_submit'),
        '#validate' => array(),
        '#limit_validation_errors' => array(),
        '#ajax' => array('callback' => '_iwformatter_builder_set_configure_ajax'),
      ),
    );
  }
  
  // -----------------
  // Construct fields
  // -----------------
  $form['fields'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array('id' => 'field-data'),
  );  

  // Create field elements that can be moved around and put into the various containers.
  foreach ($form_state['content_fields'] as $fieldname => $data) {
    $form['fields'][$fieldname] = $callbacks['field_callback'](
      $data['field'],
      $data['instance'],
      $cmn_elements,
      $callbacks['context']
    ) + array('#type' => 'container');
    
    $form['fields'][$fieldname]['#attributes']['class'][] = 'draggable-field';
  }
  
  // Post processing for fields.
  if (function_exists($callbacks['field_post_callback'])) {
    $callbacks['field_post_callback']($form['fields'], $cmn_elements, $callbacks['context']);
  }
  
  foreach ($form_state['content_wrappers'] as $name => $wrapper) {
    foreach ($wrapper->children as $key) {
      if (isset($form['fields'][$key])) {
        $form['fields'][$key]['parent']['#default_value'] = $name;
      }
      elseif (isset($form['containers'][$key])) {
        $form['containers'][$key]['parent']['#default_value'] = $name;
      }
    }
  }
  
  return $form;
}


// ==========================
// Expand the configuration
// ==========================

/**
 * Content building submit callback to set the configuring
 * element for the form. Force a rebuild too.
 */
function _iwformatter_builder_set_configure_submit($form, &$form_state) {
  $parents = $form_state['triggering_element']['#array_parents'];
  array_pop($parents);
  
  $form_state['configuring_reset'] = NULL;
  if (!empty($form_state['configuring_item'])) {
    $form_state['configuring_reset'] = $form_state['configuring_item'];
  }
    
  if (!empty($form_state['configuring_item']) && $form_state['configuring_item'] === $parents) {
    unset($form_state['configuring_item']);
  }
  else {
    $form_state['configuring_item'] = $parents;
  }
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Validate changes made to a content wrapper, to ensure
 */
function _iwformatter_builder_configure_wrapper_validate($form, &$form_state) {
  $configuring = $form_state['configuring_item'];
  if ($configuring[0] === 'containers') {
    $cont_name = $configuring[1];

    $container = $form_state['content_wrappers'][$cont_name];
    $container->validateElements(
      $form['containers'][$cont_name]['settings'],
      $form_state['values']['containers'][$cont_name]['settings'],
      $form_state
    );
  }
}

/**
 * Make changes to the content wrapper who's settings just
 * got changed. This will adjust the summary and label.
 */
function _iwformatter_builder_configure_wrapper_submit($form, &$form_state) {
  $configuring = $form_state['configuring_item'];
  if ($configuring[0] === 'containers') {
    $cont_name = $configuring[1];

    $container = $form_state['content_wrappers'][$cont_name];
    $container->submitElements(
      $form['containers'][$cont_name]['settings'],
      $form_state['values']['containers'][$cont_name]['settings'],
      $form_state
    );
  }
}

/**
 * Form builder which is used to cancel configuring of a particular item.
 */
function _iwformatter_builder_cancel_configure_submit($form, &$form_state) {  
  $form_state['configuring_reset'] = NULL;
  
  if (!empty($form_state['configuring_item'])) {
    $form_state['configuring_reset'] = $form_state['configuring_item'];
    unset($form_state['configuring_item']);
  }
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Form builder AJAX handler which will update the field or
 * container settings handler.
 */
function _iwformatter_builder_set_configure_ajax($form, $form_state) {
  $replace[] = $form_state['configuring_item'];
  $replace[] = $form_state['configuring_reset'];
  
  foreach (array_filter($replace) as $parents) {
    $key_exists = false;
    array_push($parents, 'settings');
  
    $elements = drupal_array_get_nested_value($form, $parents, $key_exists);
    if ($key_exists) {
      $commands[] = ajax_command_replace('#' . $elements['#attributes']['id'], render($elements));
    }
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Update configuration chanages. This will update label, summary, etc.
 */
function _iwformatter_builder_update_wrapper_ajax($form, $form_state) {
  $retval = _iwformatter_builder_set_configure_ajax($form, $form_state);
  
  list($group, $name) = $form_state['configuring_reset'];
  if (isset($form_state['content_wrappers'][$name])) {
    $src = $form_state['content_wrappers'][$name];

    $summary = $src->formatSummary();
    $summary += array('#attributes' => array('class' => array('field-summary')));
    
    $retval['#commands'][] = array(
      'command' => 'updateDraggableItem',
      'selector' => '#'. strtr($name, '_', '-') .'-container',
      'label' => check_plain($src->label),
      'span' => $src->settings['span'],
      'summary' => render($summary),
    );
  }
  
  return $retval;
}


// ===========================
// Add new wrapper items
// ===========================

/**
 * Create the generic form for adding new content wrappers.
 */
function _iwformatter_builder_new_wrapper_form(&$form_state) {
  unset($form_state['input']['add_content_wrapper']);
  
  $types = iwFieldGroupController::GetWrapperTypes();
  $tools['add_content_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add New Content Wrapper'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
    
    'label' => array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#size' => 15,
    ),
    'name' => array(
      '#type' => 'machine_name',
      '#field_prefix' => '<span dir="ltr">wrapper_',
      '#field_suffix' => '</span>&lrm;',
      '#size' => 15,
      '#machine_name' => array(
        'label' => t('Wrapper name'),
        'exists' => '_iwformatter_builder_wrapper_exists',
        'source' => array('tools', 'add_content_wrapper', 'label'),
        'standalone' => TRUE,
      ),
    ),
    'type' => array(
      '#type' => 'select',
      '#empty_option' => t('- Select new wrapper type -'),
      '#options' => $types,
    ),
    
    'actions' => array(
      '#type' => 'actions',
      
      'add_wrapper' => array(
        '#type' => 'submit',
        '#value' => t('Add Content Wrapper'),
        '#submit' => array('_iwformatter_builder_add_wrapper_submit'),
        '#validate' => array('_iwformatter_builder_add_wrapper_validate'),
        '#limit_validation_errors' => array(array('add_content_wrapper')),
        '#ajax' => array('callback' => '_iwformatter_builder_add_wrapper_ajax'),
      ),
    ),
  );
  
  return $tools;
}

/**
 * Machine name exists callback to determine if the machine
 * name is already in use or not. Since the callback doesn't
 * recieve the proper calling context, it attempts to find it
 * through the current menu item. Assumes this will point
 * it to the correct entity type / bundle information.
 * 
 * @param string $value
 *  String value to use as the value of the machine name.
 */
function _iwformatter_builder_wrapper_exists($value) {
  // Attempt to determine some of the context information required.
  $item = menu_get_item();
  if ($item['page_callback'] == 'ajax_form_callback') {
    list($form, $form_state) = ajax_get_form();
 
    return isset($form_state['content_wrappers']['wrapper_'. $value]);
  }
  else {
    $args = $item['page_arguments'];

    array_shift($args);
    $entity_type = array_shift($args);
    $bundle      = array_shift($args);
    $view        = empty($args) ? 'form' : array_shift($args);

    // Get the correct bundle just as a string / name.
    $bundle_name = field_extract_bundle($entity_type, $bundle);
  
    $scope = iwFieldGroupController::GetScope($entity_type, $bundle_name, $view);
    $query = db_select('iwapi_containers', 'c')
      ->condition('scope', $scope)
      ->condition('name', $value);

    $query->addExpression('COUNT(1)');
    return $query->execute()->fetchField();
  }
}

/**
 * Form builder validate the addition of a new wrapper.
 */
function _iwformatter_builder_add_wrapper_validate($form, &$form_state) {
  $values   = $form_state['values']['add_content_wrapper'];
  $elements = $form['tools']['add_content_wrapper'];
  
  if (empty($values['label'])) {
    form_set_error($elements['label'], t('Wrappers are required to have a label field.'));
  }
  
  if (empty($values['type'])) {
    form_set_error($elements['type'], t('A content wrapper type must be specified in order to create a new wrapper.'));
  }
}

/**
 * Form builder submit, to store the addition of a new wrapper item.
 */
function _iwformatter_builder_add_wrapper_submit($form, &$form_state) {
  $values = $form_state['values']['add_content_wrapper'];
  $id = 'wrapper_' . $values['name'];
  $classType = $values['type'];
  
  $values['scope'] = iwFieldGroupController::GetScope(
    $form_state['entity_type'],
    $form_state['bundle_name'], 
    $form_state['view_mode']
  );
  
  unset($values['actions']);
  
  try {
    $wrapper = new $classType('iwFieldGroupController', $values);
    $form_state['content_wrappers'][$id] = $wrapper;
    $wrapper->save();
  }
  catch (Exception $e) {
    drupal_set_error(t('Unable to save new field wrapper due to an internal error.'), 'error');
  }
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Form AJAX callback to send the commands to add a new content
 * wrapper element to the content builder.
 */
function _iwformatter_builder_add_wrapper_ajax($form, $form_state) {
  $values = $form_state['values']['add_content_wrapper'];
  $wrapper = $form['containers']['wrapper_'. $values['name']];
  
  $errors = form_get_errors();
  $commands = array();
  if (empty($errors)) {
    $commands[] = array('command' => 'addContentWrapper', 'html' => render($wrapper));
  }
  else {
    foreach ($errors as $keys => $error) {
      $commands[] = ajax_command_alert($error);
    }
  }
  
  // Send the wrapper definition to the server.
  return array('#type' => 'ajax', '#commands' => $commands);
}


// =============================
// Utility functions
// =============================

/**
 * Find all the field instances that belong to a bundle
 * and entity field.
 * 
 * @param string $entity_type
 *  Then machine name of the entity type being used.
 * @param <mixed> $bundle
 *  Either a string or object from which to get the
 *  bundle information or name from.
 */
function _iwformatter_get_entity_fields($entity_type, $bundle) {  
  $fields    = array();
  $instances = field_info_instances($entity_type, $bundle);
  
  if (!empty($instances)) {
    foreach ($instances as $fieldname => $instance) {
      $fields[$fieldname] = array(
        'field'    => field_info_field_by_id($instance['field_id']),
        'instance' => $instance,
      );
    }
  }
  
  return $fields;
}
