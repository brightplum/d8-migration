<?php

$view = new view();
$view->name = 'categorized_news';
$view->description = 'Headline news articles with taxonomy arguments.';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Categorized News';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'News';
$handler->display->display_options['css_class'] = 'tertiary-news-article';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class'] = 'grid-tile';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['label'] = 'All...';
$handler->display->display_options['footer']['area']['empty'] = TRUE;
$handler->display->display_options['footer']['area']['content'] = '<a href="/more/%1" class="more-in chevron">All %1</a>';
$handler->display->display_options['footer']['area']['format'] = 'full_html';
$handler->display->display_options['footer']['area']['tokenize'] = TRUE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Article Image */
$handler->display->display_options['fields']['field_article_img']['id'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['table'] = 'field_data_field_article_img';
$handler->display->display_options['fields']['field_article_img']['field'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['label'] = '';
$handler->display->display_options['fields']['field_article_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_article_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_article_img']['settings'] = array(
  'image_style' => 'cd_small_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
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
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Has taxonomy term ID (with depth) */
$handler->display->display_options['arguments']['term_node_tid_depth']['id'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['table'] = 'node';
$handler->display->display_options['arguments']['term_node_tid_depth']['field'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_action'] = 'empty';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['term_node_tid_depth']['depth'] = '0';
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
  'headline' => 'headline',
);

/* Display: Primary Display */
$handler = $view->new_display('attachment', 'Primary Display', 'categorized_news_primary');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'primary-news-article';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Main headline title */
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
/* Field: Content: Article Image */
$handler->display->display_options['fields']['field_article_img']['id'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['table'] = 'field_data_field_article_img';
$handler->display->display_options['fields']['field_article_img']['field'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['label'] = '';
$handler->display->display_options['fields']['field_article_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_article_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_article_img']['settings'] = array(
  'image_style' => 'cd_large',
  'image_link' => 'content',
  'field_classes' => array(),
);

/* Display: Secondary Display */
$handler = $view->new_display('attachment', 'Secondary Display', 'categorized_news_secondary');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'secondary-news-article';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '1';
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
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
/* Field: Content: Article Image */
$handler->display->display_options['fields']['field_article_img']['id'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['table'] = 'field_data_field_article_img';
$handler->display->display_options['fields']['field_article_img']['field'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['label'] = '';
$handler->display->display_options['fields']['field_article_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_article_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_article_img']['settings'] = array(
  'image_style' => 'cd_medium_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '350',
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
/* Field: Content: Link */
$handler->display->display_options['fields']['view_node']['id'] = 'view_node';
$handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['view_node']['field'] = 'view_node';
$handler->display->display_options['fields']['view_node']['label'] = '';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read More';

/* Display: Tertiary Display */
$handler = $view->new_display('attachment', 'Tertiary Display', 'categorized_news_tertiary');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'tertiary-news-article';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '2';
$handler->display->display_options['pager']['options']['offset'] = '2';
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
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
/* Field: Content: Article Image */
$handler->display->display_options['fields']['field_article_img']['id'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['table'] = 'field_data_field_article_img';
$handler->display->display_options['fields']['field_article_img']['field'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['label'] = '';
$handler->display->display_options['fields']['field_article_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_article_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_article_img']['settings'] = array(
  'image_style' => 'cd_small_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '100',
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
/* Field: Content: Link */
$handler->display->display_options['fields']['view_node']['id'] = 'view_node';
$handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['view_node']['field'] = 'view_node';
$handler->display->display_options['fields']['view_node']['label'] = '';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read More';

/* Display: Grid Tiles */
$handler = $view->new_display('attachment', 'Grid Tiles', 'categorized_news_remaining');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'remaining-news-article grid-layout';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '12';
$handler->display->display_options['pager']['options']['offset'] = '4';
