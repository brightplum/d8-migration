<?php

$view = new view();
$view->name = 'views_articles';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Views Articles';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Views';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['use_more_text'] = 'More Views';
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '100';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Homepage thumbnail */
$handler->display->display_options['fields']['field_image']['id'] = 'field_image';
$handler->display->display_options['fields']['field_image']['table'] = 'field_data_field_image';
$handler->display->display_options['fields']['field_image']['field'] = 'field_image';
$handler->display->display_options['fields']['field_image']['relationship'] = 'field_author_profile_target_id';
$handler->display->display_options['fields']['field_image']['label'] = '';
$handler->display->display_options['fields']['field_image']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_image']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_image']['settings'] = array(
  'image_style' => 'cd_bio_small',
  'image_link' => '',
  'field_classes' => array(),
);
$handler->display->display_options['fields']['field_image']['group_column'] = 'entity_id';
/* Field: Content: Home Page Author */
$handler->display->display_options['fields']['field_hp_author']['id'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['table'] = 'field_data_field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['field'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['label'] = '';
$handler->display->display_options['fields']['field_hp_author']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_author']['settings'] = array(
  'field_classes' => array(),
);
/* Field: Content: Home page title */
$handler->display->display_options['fields']['field_hp_title']['id'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['table'] = 'field_data_field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['field'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['label'] = '';
$handler->display->display_options['fields']['field_hp_title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_hp_title']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_hp_title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_title']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['field_hp_title']['settings'] = array(
  'trim_length' => '300',
  'trim_type' => 'chars',
  'trim_suffix' => '...',
  'more_link' => '0',
  'more_text' => 'Read more',
  'trim_options' => array(
    'text' => 'text',
  ),
  'field_classes' => array(),
);
/* Field: Profile Types: Organization Image */
$handler->display->display_options['fields']['field_profile_img']['id'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['table'] = 'field_data_field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['field'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['relationship'] = 'field_author_profile_target_id';
$handler->display->display_options['fields']['field_profile_img']['label'] = '';
$handler->display->display_options['fields']['field_profile_img']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_profile_img']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_profile_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_profile_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_profile_img']['settings'] = array(
  'image_style' => 'cd_bio_small',
  'image_link' => '',
  'field_classes' => array(),
);
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'views_article' => 'views_article',
);

/* Display: Sidebar Block */
$handler = $view->new_display('block', 'Sidebar Block', 'block');
$handler->display->display_options['defaults']['use_more'] = FALSE;
$handler->display->display_options['use_more'] = TRUE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['use_more_always'] = TRUE;
$handler->display->display_options['defaults']['use_more_text'] = FALSE;
$handler->display->display_options['use_more_text'] = 'More Views';
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['defaults']['query'] = FALSE;
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '15';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['inline'] = array(
  'field_image' => 'field_image',
);
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_author_profile_target_id']['id'] = 'field_author_profile_target_id';
$handler->display->display_options['relationships']['field_author_profile_target_id']['table'] = 'field_data_field_author_profile';
$handler->display->display_options['relationships']['field_author_profile_target_id']['field'] = 'field_author_profile_target_id';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Profile Types: Organization Image */
$handler->display->display_options['fields']['field_profile_img']['id'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['table'] = 'field_data_field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['field'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['relationship'] = 'field_author_profile_target_id';
$handler->display->display_options['fields']['field_profile_img']['label'] = '';
$handler->display->display_options['fields']['field_profile_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_profile_img']['empty'] = '<i class="fa fa-user"></i>';
$handler->display->display_options['fields']['field_profile_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_profile_img']['settings'] = array(
  'image_style' => 'cd_bio_small',
  'image_link' => '',
  'field_classes' => array(),
);
$handler->display->display_options['fields']['field_profile_img']['group_column'] = 'entity_id';
/* Field: Content: Home Page Author */
$handler->display->display_options['fields']['field_hp_author']['id'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['table'] = 'field_data_field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['field'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['label'] = '';
$handler->display->display_options['fields']['field_hp_author']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_author']['settings'] = array(
  'field_classes' => array(),
);
/* Field: Content: Home page title */
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

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'article_display_page');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['grouping'] = array(
  0 => array(
    'field' => 'created',
    'rendered' => 1,
    'rendered_strip' => 0,
  ),
);
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'created' => 'created',
  'field_image' => 'field_image',
  'name' => 'name',
  'field_hp_author' => 'field_hp_author',
  'title' => 'field_hp_author',
  'body' => 'field_hp_author',
  'view_node' => 'field_hp_author',
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
  'created' => array(
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
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '<br />',
    'empty_column' => 0,
  ),
  'field_hp_author' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '<br />',
    'empty_column' => 0,
  ),
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'body' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'view_node' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = '';
$handler->display->display_options['fields']['created']['exclude'] = TRUE;
$handler->display->display_options['fields']['created']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'l, F j, Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'long';
/* Field: Content: Homepage thumbnail */
$handler->display->display_options['fields']['field_image']['id'] = 'field_image';
$handler->display->display_options['fields']['field_image']['table'] = 'field_data_field_image';
$handler->display->display_options['fields']['field_image']['field'] = 'field_image';
$handler->display->display_options['fields']['field_image']['label'] = '';
$handler->display->display_options['fields']['field_image']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_image']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_image']['settings'] = array(
  'image_style' => 'cd_small_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
$handler->display->display_options['fields']['field_image']['group_column'] = 'entity_id';
/* Field: Content: Home Page Author */
$handler->display->display_options['fields']['field_hp_author']['id'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['table'] = 'field_data_field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['field'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['label'] = '';
$handler->display->display_options['fields']['field_hp_author']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_author']['settings'] = array(
  'field_classes' => array(),
);
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['title']['alter']['link_class'] = 'article-title';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['alter']['strip_tags'] = TRUE;
$handler->display->display_options['fields']['body']['alter']['preserve_tags'] = '<p> <br>';
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '200',
  'trim_type' => 'chars',
  'trim_suffix' => '...',
  'more_link' => '0',
  'more_text' => 'Read more',
  'summary_handler' => 'trim',
  'trim_options' => array(
    'text' => 'text',
  ),
  'field_classes' => array(),
);
$handler->display->display_options['fields']['body']['group_column'] = 'entity_id';
/* Field: Content: Link */
$handler->display->display_options['fields']['view_node']['id'] = 'view_node';
$handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['view_node']['field'] = 'view_node';
$handler->display->display_options['fields']['view_node']['label'] = '';
$handler->display->display_options['fields']['view_node']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['view_node']['alter']['link_class'] = 'chevron';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read more';
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Authored on date (field_article_date) */
$handler->display->display_options['arguments']['field_article_date_value']['id'] = 'field_article_date_value';
$handler->display->display_options['arguments']['field_article_date_value']['table'] = 'field_data_field_article_date';
$handler->display->display_options['arguments']['field_article_date_value']['field'] = 'field_article_date_value';
$handler->display->display_options['arguments']['field_article_date_value']['default_argument_type'] = 'date';
$handler->display->display_options['arguments']['field_article_date_value']['default_argument_skip_url'] = TRUE;
$handler->display->display_options['arguments']['field_article_date_value']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_article_date_value']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_article_date_value']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['field_article_date_value']['year_range'] = '2008:+1';
$handler->display->display_options['arguments']['field_article_date_value']['granularity'] = 'day';
$handler->display->display_options['path'] = 'views';

/* Display: Top Views */
$handler = $view->new_display('block', 'Top Views', 'block_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Today\'s Views';
$handler->display->display_options['defaults']['use_more'] = FALSE;
$handler->display->display_options['use_more'] = TRUE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['use_more_always'] = TRUE;
$handler->display->display_options['defaults']['use_more_text'] = FALSE;
$handler->display->display_options['use_more_text'] = 'More Views';
$handler->display->display_options['defaults']['cache'] = FALSE;
$handler->display->display_options['cache']['type'] = 'time';
$handler->display->display_options['cache']['results_lifespan'] = '1800';
$handler->display->display_options['cache']['results_lifespan_custom'] = '0';
$handler->display->display_options['cache']['output_lifespan'] = '1800';
$handler->display->display_options['cache']['output_lifespan_custom'] = '0';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '12';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['inline'] = array(
  'field_image' => 'field_image',
);
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Homepage thumbnail */
$handler->display->display_options['fields']['field_image']['id'] = 'field_image';
$handler->display->display_options['fields']['field_image']['table'] = 'field_data_field_image';
$handler->display->display_options['fields']['field_image']['field'] = 'field_image';
$handler->display->display_options['fields']['field_image']['label'] = '';
$handler->display->display_options['fields']['field_image']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_image']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_image']['settings'] = array(
  'image_style' => 'cd_tiny_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
$handler->display->display_options['fields']['field_image']['group_column'] = 'entity_id';
/* Field: Content: Home Page Author */
$handler->display->display_options['fields']['field_hp_author']['id'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['table'] = 'field_data_field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['field'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['label'] = '';
$handler->display->display_options['fields']['field_hp_author']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_author']['settings'] = array(
  'field_classes' => array(),
);
/* Field: Content: Home page title */
$handler->display->display_options['fields']['field_hp_title']['id'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['table'] = 'field_data_field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['field'] = 'field_hp_title';
$handler->display->display_options['fields']['field_hp_title']['label'] = '';
$handler->display->display_options['fields']['field_hp_title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_hp_title']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_hp_title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_title']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['field_hp_title']['settings'] = array(
  'trim_length' => '300',
  'trim_type' => 'chars',
  'trim_suffix' => '...',
  'more_link' => '0',
  'more_text' => 'Read more',
  'trim_options' => array(
    'text' => 'text',
  ),
  'field_classes' => array(),
);

/* Display: Categorized Block */
$handler = $view->new_display('attachment', 'Categorized Block', 'attachment_1');
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['defaults']['query'] = FALSE;
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['inline'] = array(
  'field_image' => 'field_image',
);
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['content'] = '<div class="more-in"><a class="chevron" href="/more/!1">More %1 Views</a></div>';
$handler->display->display_options['footer']['area']['format'] = 'full_html';
$handler->display->display_options['footer']['area']['tokenize'] = TRUE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_author_profile_target_id']['id'] = 'field_author_profile_target_id';
$handler->display->display_options['relationships']['field_author_profile_target_id']['table'] = 'field_data_field_author_profile';
$handler->display->display_options['relationships']['field_author_profile_target_id']['field'] = 'field_author_profile_target_id';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Profile Types: Organization Image */
$handler->display->display_options['fields']['field_profile_img']['id'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['table'] = 'field_data_field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['field'] = 'field_profile_img';
$handler->display->display_options['fields']['field_profile_img']['relationship'] = 'field_author_profile_target_id';
$handler->display->display_options['fields']['field_profile_img']['label'] = '';
$handler->display->display_options['fields']['field_profile_img']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_profile_img']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_profile_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_profile_img']['empty'] = '<i class="fa fa-user"></i>';
$handler->display->display_options['fields']['field_profile_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_profile_img']['settings'] = array(
  'image_style' => 'cd_bio_small',
  'image_link' => '',
  'field_classes' => array(),
);
$handler->display->display_options['fields']['field_profile_img']['group_column'] = 'entity_id';
/* Field: Content: Home Page Author */
$handler->display->display_options['fields']['field_hp_author']['id'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['table'] = 'field_data_field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['field'] = 'field_hp_author';
$handler->display->display_options['fields']['field_hp_author']['label'] = '';
$handler->display->display_options['fields']['field_hp_author']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hp_author']['settings'] = array(
  'field_classes' => array(),
);
/* Field: Content: Home page title */
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
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Has taxonomy term ID (with depth) */
$handler->display->display_options['arguments']['term_node_tid_depth']['id'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['table'] = 'node';
$handler->display->display_options['arguments']['term_node_tid_depth']['field'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_action'] = 'empty';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['term_node_tid_depth']['specify_validation'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['validate']['type'] = 'taxonomy_term';
$handler->display->display_options['arguments']['term_node_tid_depth']['validate_options']['vocabularies'] = array(
  'menu_categories' => 'menu_categories',
  'topic' => 'topic',
);
$handler->display->display_options['arguments']['term_node_tid_depth']['depth'] = '0';

/* Display: Feed */
$handler = $view->new_display('feed', 'Feed', 'feed_1');
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'rss';
$handler->display->display_options['row_plugin'] = 'node_rss';
$handler->display->display_options['path'] = 'feed/views_rss';
