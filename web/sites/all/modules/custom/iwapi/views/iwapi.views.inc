<?php
/**
 * @file
 * Contains core views HOOKs.
 */

/**
 * Implements hook_views_plugins().
 */
function iwapi_views_plugins() {
  // Have views cache the table list for us so it gets
  // cleared at the appropriate times.
  $cache = views_cache_get('iwentity_base_tables', TRUE);
  if (!empty($cache->data)) {
    $tables = $cache->data;
  }
  else {
    $tables = array();

    foreach (views_fetch_data() as $table => $data) {
      if (!empty($data['table']['entity type']) && !empty($data['table']['base'])) {
        $info = entity_get_info($data['table']['entity type']);

        if (isset($info['entity class']) && @is_subclass_of($info['entity class'], 'iwEntity')) {
          $tables[] = $table;
        }
      }
    }
    views_cache_set('iwentity_base_tables', $tables, TRUE);
  }
  
  // If we found some entity tables supported by views, create the entity view.
  if (!empty($tables)) {
    return array(
      'module' => 'iwapi',
      'row' => array(
        'entity' => array(
          'title' => t('View iwEntity'),
          'help' => t('Renders a single entity in a specific view mode (e.g. teaser).'),
          'handler' => 'iwapi_views_plugin_row_entity',
          'uses fields' => FALSE,
          'uses options' => TRUE,
          'type' => 'normal',
          'base' => $tables,
        ),
      ),
    );
  }
}
