<?php
/**
 * @file
 * File implements the views HOOKs.
 */

/**
 * Implements HOOK_views_plugins().
 */
function views_term_alias_views_plugins() {
  return array(
    'module' => 'views_term_alias', // This just says our themes are elsewhere.
    'argument validator' => array(
      'taxonomy_url_alias' => array(
        'title' => t('Taxonomy Term Alias'),
        'handler' => 'iwapi_plugin_argument_validate_term_alias',
        'path' => drupal_get_path('module', 'views_term_alias') . '/views', // not necessary for most modules
      ),
    ),
  );
}
