<?php

$view = new view();
$view->name = 'most_popular';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Most Popular';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Most popular this week';
$handler->display->display_options['use_more'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['use_more_text'] = 'More';
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '3';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_image' => 'field_image',
  'type' => 'type',
  'field_hp_title' => 'type',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_image' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '<br />',
    'empty_column' => 0,
  ),
  'field_hp_title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Field: Organization Image */
$handler->display->display_options['fields']['field_image']['id'] = 'field_image';
$handler->display->display_options['fields']['field_image']['table'] = 'field_data_field_image';
$handler->display->display_options['fields']['field_image']['field'] = 'field_image';
$handler->display->display_options['fields']['field_image']['label'] = '';
$handler->display->display_options['fields']['field_image']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_image']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_image']['settings'] = array(
  'image_style' => 'thumbnail',
  'image_link' => 'content',
  'field_classes' => array(),
);
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['label'] = '';
$handler->display->display_options['fields']['type']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['type']['alter']['text'] = '<span class="node-type">[type]</span>';
$handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
/* Field: Content: Main headline title */
$handler->display->display_options['fields']['field_hp_title']['id'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['table'] = 'field_data_field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['field'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['label'] = '';
$handler->display->display_options['fields']['field_hp_title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_hp_title']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_hp_title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_title']['settings'] = array(
  'field_classes' => array(),
);
/* Sort criterion: Content statistics: Total views */
$handler->display->display_options['sorts']['totalcount']['id'] = 'totalcount';
$handler->display->display_options['sorts']['totalcount']['table'] = 'node_counter';
$handler->display->display_options['sorts']['totalcount']['field'] = 'totalcount';
$handler->display->display_options['sorts']['totalcount']['order'] = 'DESC';
/* Sort criterion: Content statistics: Most recent view */
$handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
$handler->display->display_options['sorts']['timestamp']['table'] = 'node_counter';
$handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
$handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content statistics: Total views */
$handler->display->display_options['filters']['totalcount']['id'] = 'totalcount';
$handler->display->display_options['filters']['totalcount']['table'] = 'node_counter';
$handler->display->display_options['filters']['totalcount']['field'] = 'totalcount';
$handler->display->display_options['filters']['totalcount']['operator'] = '>';
$handler->display->display_options['filters']['totalcount']['value']['value'] = '0';
$handler->display->display_options['filters']['totalcount']['group'] = 1;
/* Filter criterion: Content statistics: Most recent view */
$handler->display->display_options['filters']['timestamp']['id'] = 'timestamp';
$handler->display->display_options['filters']['timestamp']['table'] = 'node_counter';
$handler->display->display_options['filters']['timestamp']['field'] = 'timestamp';
$handler->display->display_options['filters']['timestamp']['operator'] = '>=';
$handler->display->display_options['filters']['timestamp']['value']['value'] = '-7 day';
$handler->display->display_options['filters']['timestamp']['value']['type'] = 'offset';
$handler->display->display_options['filters']['timestamp']['group'] = 1;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['defaults']['filter_groups'] = FALSE;
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content statistics: Total views */
$handler->display->display_options['filters']['totalcount']['id'] = 'totalcount';
$handler->display->display_options['filters']['totalcount']['table'] = 'node_counter';
$handler->display->display_options['filters']['totalcount']['field'] = 'totalcount';
$handler->display->display_options['filters']['totalcount']['operator'] = '>';
$handler->display->display_options['filters']['totalcount']['value']['value'] = '0';
$handler->display->display_options['path'] = 'most-popular';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_most_popular');
