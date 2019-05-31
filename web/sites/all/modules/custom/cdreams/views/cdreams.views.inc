<?php
/**
 * @file
 * Contains the default / core views hooks.
 */


/**
 * Implements HOOK_views_data().
 */
function cdreams_views_data() {
  $ordering = array(
    'table' => array(
      'group' => 'Common Dreams',
      'title' => t('Home page ordering'),
      'help' => t('Contains the publishing status of entities, relative to their assigned workflows.'),
    ),
  );

  $ordering['promoted'] = array(
    'title' => t('Promoted'),
    'help' => t('Whether or not the content is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'published-notpublished' => array(t('Promoted'), t('Not promoted')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
      'use equal' => TRUE, // Use status = 1 instead of status <> 0 in WHERE statment
    ),
    'sort' => array('handler' => 'views_handler_sort'),
  );

  $ordering['date'] = array(
    'title' => t('Ordering date'),
    'help' => t('The date used for ordering content. This is set to a specific granularity.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort'     => array('handler' => 'views_handler_sort_date'),
    'filter'   => array('handler' => 'views_handler_filter_date'),
    'argument' => array('handler' => 'views_handler_argument_date'),
  );
  
  $ordering['weight'] = array(
    'title' => t('Ordering weight'),
    'help' => t('Weight used to '),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort'     => array('handler' => 'views_handler_sort'),
    'filter'   => array('handler' => 'views_handler_filter_numeric'),
    'argument' => array('handler' => 'views_handler_argument_numeric'),
  );
  
  $ordering['created'] = array(
    'title' => t('Creation date'),
    'help' => t('Timestamp of the entity creation date.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort'     => array('handler' => 'views_handler_sort_date'),
    'filter'   => array('handler' => 'views_handler_filter_date'),
    'argument' => array('handler' => 'views_handler_argument_date'),
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

    $ordering['table']['join'][$info['base table']] = $join_def;
  }

  return array('cdreams_ordering' => $ordering);
}
