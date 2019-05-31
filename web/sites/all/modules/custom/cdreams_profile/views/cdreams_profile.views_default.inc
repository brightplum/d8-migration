<?php
/**
 * @file
 * Contains HOOKs and definitions for default views.
 */


/**
 * Implements HOOK_views_default_views().
 */
function cdreams_profile_views_default_views() {  
  $view = new view();
  $view->name = 'author_articles';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Author Articles';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Articles by this author';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';

  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;

  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';

  /* Contextual filter: Content: Author Profile(s) (field_author_profile) */
  $handler->display->display_options['arguments']['field_author_profile_target_id']['id'] = 'field_author_profile_target_id';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['table'] = 'field_data_field_author_profile';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['field'] = 'field_author_profile_target_id';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_author_profile_target_id']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['field_author_profile_target_id']['validate']['type'] = 'numeric';

  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

  /* Display: Attachment */
  $handler = $view->new_display('attachment', 'Attachment', 'attachment_1');
  $handler->display->display_options['pager']['type'] = 'some';
  
  return array($view->name => $view);
}
