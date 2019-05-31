<?php
/**
 * @file
 * Handle the base views module HOOKs.
 */

/**
 * Implements HOOK_views_data()
 */
function csp_views_data() {
  $csp_entities['table'] = array(
    'group' => 'CSP',
    'title' => t('Entity CSP States'),
    'help' => t('Contains the publishing status of entities, relative to their assigned workflows.'),
    'base' => array(
      'field' => 'state_id',
      'title' => t('CSP Workflows'),
      'weight' => -10,
      'defaults' => array('fields' => array('state')),
    ),
  );

  $csp_entities['entity_type'] = array(
    'title' => t('Entity Type'),
    'help' => t('The type of entity for this state entry.'),
    'field' => array(
      'handler' => 'csp_views_handler_field_entity_type',
    ),
    'filter' => array(
      'handler' => 'csp_views_handler_filter_entity_type',
    ),
  );

  $csp_entities['entity_id'] = array(
    'title' => t('Entity'),
    'help' => t('The content (entity) that the state belongs to.'),
    'field' => array(
      'handler' => 'csp_views_handler_field_entity',
    ),
  );

  // Describe the workflow that the state belongs to.
  $csp_entities['flow'] = array(
    'title' => t('Workflow'),
    'help' => t('The workflow for which the state belongs to.'),
    'field' => array(
      'handler' => 'csp_views_handler_field_workflow',
    ),
    'filter' => array(
      'handler' => 'csp_views_handler_filter_workflow',
    ),
  );

  // Describe how the workflow state field can be used.
  $csp_entities['state'] = array(
    'title' => t('Workflow State'),
    'help'  => t('The current status of the workflow.'),
    'field' => array(
      'handler' => 'csp_views_handler_field_csp_state',
    ),
    'filter' => array(
      'handler' => 'csp_views_handler_filter_csp_state',
    ),
  );
  
  // Tracking the last time a state was modified.
  $csp_entities['changed'] = array(
    'title' => t('Last changed'),
    'help' => t('Date time of the last change for this entity.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  // Create the joins so that CSP information can be
  // included into the various entity queries.
  $types = array('node' => 'node') + _iwapi_get_api_entity_types();

  foreach ($types as $type) {
    $info = entity_get_info($type);

    // Tell views how to join our state information to an entity table.
    $join_def = array(
      'left_table' => $info['base table'],
      'left_field' => $info['entity keys']['id'],
      'field' => 'entity_id',
      'extra' => array(
        array('field' => 'entity_type', 'value' => $type, 'operator' => '='),
      ),
    );

    if (!empty($info['entity keys']['revision'])) {
      $join_def['handler'] = 'iwapi_views_join_revisioned_entity';
      $join_def['entity_type'] = $type;
    }

    $csp_entities['table']['join'][$info['base table']] = $join_def;
  }

  // Return all the tables we wish to expose to views.
  return array('csp_entities' => $csp_entities);
}
