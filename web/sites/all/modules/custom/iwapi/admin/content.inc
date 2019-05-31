<?php
/**
 * @file content_order.form.inc
 * Contains the definition and callbacks for forms that manage content ordering.
 */

// =======================
// Reordering of content
// =======================

/**
 * Form callback used to build the form to reorder node content.
 * Finds all nodes that contains the field "field_weight" and
 * creates a table for setting this value.
 *
 * @see iwapi_order_content_form_submit();
 */
function iwapi_order_content_form($form, &$form_state) {
  $field = field_info_field('field_weight');

  // Build the sortable table of the resulting nodes.
  $form['table'] = array(
    '#type'   => 'table_orderable',
    '#header' => array(t('Title'), t('Node ID'), t('Type')),
    '#empty'  => t('No content available.'),
    '#weight' => 10,
  );

  if (!empty($field['bundles']['node'])) {
    $options = array_values($field['bundles']['node']);
    $bundle = empty($form_state['order_type']) ? reset($options) : $form_state['order_type'];

    $form['node_type'] = array(
      '#type' => 'fieldset',
      '#title' => t('Node Type'),

      'select_type' => array(
        '#type' => 'select',
        '#theme_wrappers' => array(),
        '#options' => array_combine($options, $options),
        '#default_value' =>$bundle,
      ),
      'switch_type' => array(
        '#type' => 'submit',
        '#value' => t('Change'),
      ),
    );

    $form['actions'] = array(
      '#type' => 'actions',
      'save_changes' => array(
        '#type' => 'submit',
        '#value' => t('Save Changes'),
      ),
    );

    $query = db_select('node', 'n')
      ->fields('n', array('nid', 'title', 'type', 'vid'))
      ->condition('type', $bundle)
      ->condition('status', NODE_PUBLISHED);

    $nodes = array();
    foreach($query->execute() as $data) {
      $nodes[$data->nid] = $data;
    }

    // We load and attach the weight field so we can sort by it.
    // This is done instead of EntityQuery, because it does not work if no weight values
    // have been set for the nodes, and we need all published nodes of a specific type.
    field_attach_load('node', $nodes, FIELD_LOAD_CURRENT, array('field_id' => $field['id']));
    usort($nodes, '_iwapi_order_content_compare_field_weight');
    foreach ($nodes as $node) {
      $form['table'][$node->nid] = array(
        'title' => array('#type' => 'markup', '#markup' => check_plain($node->title)),
        'nid' => array('#type' => 'markup', '#markup' => $node->nid),
        'type' => array('#type' => 'markup', '#markup' => check_plain($node->type)),
      );
    }
  }
  return $form;
}

/**
 * Comparison function for nodes by the field "field_weight"
 *
 * @return number
 *  0 = if both values are equal in weight
 *  0 < if the magnitude of $b is less than $a
 *  0 > if the magnitude of $b is greater than $a
 */
function _iwapi_order_content_compare_field_weight($a, $b) {
  $weightA = field_get_items('node', $a, 'field_weight');
  $weightB = field_get_items('node', $b, 'field_weight');
  $weightA = empty($weightA[0]) ? 0 : $weightA[0]['value'];
  $weightB = empty($weightB[0]) ? 0 : $weightB[0]['value'];

  return $weightA - $weightB;
}

/**
 * Form submit callback for iwapi_order_content_form();
 */
function iwapi_order_content_form_submit($form, &$form_state) {
  $values = &$form_state['values'];
  switch ($values['op']) {
    case t('Save Changes'):
      $field = 'field_weight';
      $nodes = node_load_multiple(array_keys($values['table']));

      // Save the weight changes, assigned by our administration table.
      foreach ($nodes as $nid => $node) {
        $langcode = field_language('node', $node, $field);
        $node->{$field}[$langcode][0]['value'] = $values['table'][$nid]['weight'];
        node_save($node);
      }
      // Intentional fall through to save changes and stay on this node type.
    case t('Change'):
      // Change the node type that is being re-ordered.
      $form_state['order_type'] = $values['select_type'];
      $form_state['rebuild'] = TRUE;
      break;
  }
}
