<?php
/**
 * @file
 * Add the common table building functions and helpers.
 */

/**
 * Modifies a form element so that it can contain
 * and faciliate the build of a table.
 * 
 * @param array $element
 *  Reference to element that gets turned into the form builder. 
 * @param array $form_state
 *  The current state of the form. The form state should contain
 *  the current setting values (columns + fields).
 */
function _iwapi_table_builder_form(&$element, &$form_state) {    
  $modpath = drupal_get_path('module', 'iwapi');
  
  $element['#attached']['css'][] = $modpath .'/theme/css/table-builders.css';
  $element['#attached']['js'][]  = $modpath .'/js/table-builders.js';
  $element['#attached']['library'][] = array('iwapi', 'iwapi.ajax');
  $element['#attached']['library'][] = array('system', 'ui.sortable');
  
  // Start building table container.
  $parents = $element['#parents'];
  $parent  = array_pop($parents);
  $info = $form_state['tables'][$parent];
  
  $element['#type'] = 'container';
  $element['#tree'] = true;
  $element['#attributes'] = array(
    'id' => implode('-', $element['#array_parents']) .'-table',
    'class' => array('table-builder-settings', 'clearfix'),
  );
  
  $allowedColumns = array();
  foreach ($info['header'] as $colname => $col) {
    $allowedColumns[$colname] = is_array($col) ? $col['data'] : $col;
    
    $parents = array_merge($element['#parents'], array('columns', $colname));
    $element['columns'][$colname] = array(
      '#prefix' => '<div class="column-wrapper"><h3 class="column-label">'. check_plain($allowedColumns[$colname]) .'</h3>',
      '#suffix' => '</div>',
      
      'column_value' => array(
        '#type' => 'hidden',
        '#value' => $colname,
        '#attributes' => array('class' => array('column-value')),
      ),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove column'),
        '#name' => 'column-'. preg_replace('#[^\w\-]+#', '-', $colname) .'-remove',
        '#limit_validation_errors' => array($parents),
        '#validate' => array(),
        '#submit' => array('_iwapi_table_builder_remove_column_submit'),
        '#attributes' => array('class' => array('remove-column-btn')),
        '#ajax' => array(
          'callback' => '_iwapi_table_builder_remove_column_ajax',
          'progress' => array('message' => NULL),
        ),
      ),
    );
  }
  
  // Add field properties and prepare for building.
  foreach ($info['fields'] as $name => $prop) {
    $prop += array('weight' => 0, 'column' => '');

    $element['fields'][$name] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('draggable-field')),
        
      'title' => array(
        '#prefix' => '<div class="handle-title">',
        '#suffix' => '</div>',
        '#markup' => '<div class=""></div>'. check_plain($prop['label']),
      ),
      'column' => array(
        '#type' => 'select',
        '#empty_option' => t('- Not used -'),
        '#empty_value' => '',
        '#options' => $allowedColumns,
        '#default_value' => $prop['column'],
        '#attributes' => array('class' => array('parent-column')),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight'),
        '#element_validate' => array('element_validate_integer'),
        '#attributes' => array('class' => array('weight-value')),
        '#default_value' => $prop['weight'],
      ),
      'edit' => array(
        '#type' => 'submit',
        '#name' => $parent .'-field-edit-'. strtr($name, '_', '-'),
        '#value' => t('Edit'),
        '#limit_validation_errors' => array(),
        '#validate' => array(),
        '#submit' => array('_iwapi_table_builder_select_field_submit'),
        '#attributes' => array('class' => array('table-edit-field')),
        '#ajax' => array(
          'callback' => '_iwapi_table_builder_update_field_ajax',
          'progress' => array('message' => NULL),
        ),
      ),
    );
  }
  
  if (!empty($info['edit_field']) && isset($element['fields'][$info['edit_field']])) {
    $edit_field = $info['edit_field'];
    $element['fields'][$edit_field] += iwsearch_instance_results_field_config($info['fields'][$edit_field]);
    
    $element['fields'][$edit_field]['save'] = array(
      '#type' => 'submit',
      '#name' => $parent .'-edit-field',
      '#value' => t('Save'),
      '#validate' => array(),
      '#submit' => array('_iwapi_table_builder_save_field_submit'),
      '#limit_validation_errors' => array(array_merge($element['#parents'], array('fields', $edit_field))),
      '#ajax' => array(
        'callback' => '_iwapi_table_builder_update_field_ajax',
        'progress' => array('message' => NULL),
      ),
    );
  }
  
  $create_parents = array_merge($element['#array_parents'], array('create_new'));
  $element['create_new'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#weight' => -10,
    '#attributes' => array('class' => array('add-new-column')),
    
    'column_label' => array(
      '#type' => 'textfield',
      '#title' => t('Column name'),
      '#size' => 10,
    ),
    'column_name' => array(
      '#type' => 'textfield',
      '#title' => t('Column value'),
      '#size' => 10,
      '#required' => FALSE,
      '#table_values_index' => $parent,
      '#machine_name' => array(
        'exists' => '_iwapi_table_builder_add_column_exists',
        'source' => array_merge($create_parents, array('column_label')),
      ),
    ),
    'add' => array(
      '#type' => 'submit',
      '#value' => t('Add Column'),
      '#parents' => array_merge($create_parents, array('add')),
      '#limit_validation_errors' => array($create_parents),
      '#validate' => array(),
      '#submit'   => array('_iwapi_table_builder_add_column_submit'),
      '#ajax' => array(
        'callback' => '_iwapi_table_builder_add_column_ajax',
        'progress' => array('message' => NULL),
      ),
    ),
  );
}


// =================================
// Manage fields
// =================================

/**
 * A request to update field data has been made
 * set the field to edit, and request a form rebuild.
 */
function _iwapi_table_builder_select_field_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#array_parents'];
  array_pop($parents);
  
  $field = array_pop($parents);
  array_pop($parents);
  $index = array_pop($parents);

  // Use the gear as a toggle if already selected.
  if (empty($form_state['tables'][$index]['edit_field']) || $form_state['tables'][$index]['edit_field'] != $field) {
    $form_state['tables'][$index]['edit_field'] = $field;  
  }
  else {
    unset($form_state['tables'][$index]['edit_field']);
  }
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Save the field changes.
 */
function _iwapi_table_builder_save_field_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#parents'];
  
  array_pop($parents);
  $values = &drupal_array_get_nested_value($form_state['values'], $parents);
  
  $field = array_pop($parents);
  array_pop($parents);
  $index = array_pop($parents);
  
  unset($form_state['tables'][$index]['edit_field']);
  
  // Store the saved fields.
  $form_state['tables'][$index]['fields'][$field]['title'] = $values['label'];
  $form_state['tables'][$index]['fields'][$field]['display']['label'] = $values['settings']['label'];
  $form_state['tables'][$index]['fields'][$field]['cardinality'] = $values['cardinality'];
  
  $form_state['rebuild'] = TRUE;
}

/**
 * Return altered field data, and send an updated version
 * back to the UI to replace.
 */
function _iwapi_table_builder_update_field_ajax($form, $form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#array_parents'];
  
  array_pop($parents);
  $element = &drupal_array_get_nested_value($form, $parents);
  
  $command = array(
    'command' => 'updateTableField',
    'html' => render($element),
  );
  
  return array('#type' => 'ajax', '#commands' => array($command));
}


// =================================
// Add column handling
// =================================

/**
 * Ensure that this column header doesn't already exists.
 */
function _iwapi_table_builder_add_column_exists($value, $element, $form_state) {
  $header = $form_state['tables'][$element['#table_values_index']]['header'];
  
  return isset($header[$value]);
}

/**
 * Adjust column information to add the new data.
 */
function _iwapi_table_builder_add_column_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#parents'];
  
  array_pop($parents);
  $values = &drupal_array_get_nested_value($form_state['values'], $parents);
  array_pop($parents);
  $index = array_pop($parents);
  
  $form_state['tables'][$index]['header'][$values['column_name']] = array(
    'data' => $values['column_label'],
  );

  $form_state['rebuild'] = TRUE;
}

/**
 * A request to add a column has been made. The form
 * structure has been internally updates, but we now
 * need to update the UI of the change.
 * 
 * @param array $form
 *  The current form.
 * @param array $form_state
 *  The current state of the form.
 * @return array
 *  Renderable array of AJAX commands that can be send as an
 *  AJAX request for form processing and updates.
 */
function _iwapi_table_builder_add_column_ajax(array $form, array $form_state) {  
  $trigger = $form_state['triggering_element'];
  
  $parents = $trigger['#parents'];
  array_pop($parents);
  $values = &drupal_array_get_nested_value($form_state['values'], $parents);
  
  $eparents = $trigger['#array_parents'];
  array_pop($eparents);
  array_pop($eparents);
  $element = &drupal_array_get_nested_value($form, $eparents);

  $commands[] = array(
    'command' => 'addTableColumn',
    'column'  => $values['column_name'],
    'html'    => render($element['columns'][$values['column_name']]),
  );

  return array('#type' => 'ajax', '#commands' => $commands);
}


// =================================
// Remove column handling
// =================================

/**
 * Remove a single column from the header and rebuild the
 * from without this column element.
 */
function _iwapi_table_builder_remove_column_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#array_parents'];
  
  array_pop($parents);
  $colname = array_pop($parents);
  array_pop($parents);
  $index = array_pop($parents);

  unset($form_state['tables'][$index]['header'][$colname]);
  
  // Rebuild this form, minus that last column.
  $form_state['rebuild'] = TRUE;
}

/**
 * Signal the removal of column. The internal form processing should
 * have taken care of this already, just update the UI of the change.
 * 
 * @param array $form
 *  The current form.
 * @param array $form_state
 *  The current state of the form.
 * @return array
 *  Renderable array of AJAX commands that can be send as an
 *  AJAX request for form processing and updates.
 */
function _iwapi_table_builder_remove_column_ajax($form, $form_state) {
  $trigger = $form_state['triggering_element'];
  $parents = $trigger['#array_parents'];
  
  array_pop($parents);
    
  $commands[] = array(
    'command' => 'removeTableColumn',
    'column' => array_pop($parents),
  );

  return array('#type' => 'ajax', '#commands' => $commands);
}

