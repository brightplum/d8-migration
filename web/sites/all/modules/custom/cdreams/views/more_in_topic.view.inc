<?php


$view = new view();
$view->name = 'more_in_topic';
$view->description = 'Headline news articles with taxonomy arguments.';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'More In Topic';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'More In...';
$handler->display->display_options['css_class'] = 'tertiary-news-article';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_article_img' => 'field_article_img',
  'type' => 'type',
  'title' => 'type',
  'body' => 'type',
  'view_node' => 'type',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 1,
  ),
  'field_article_img' => array(
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
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['label'] = '';
$handler->display->display_options['fields']['type']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['type']['alter']['text'] = '<span class="node-type">[type]</span>';
$handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['title']['alter']['link_class'] = 'article-title';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['element_wrapper_type'] = 'span';
$handler->display->display_options['fields']['title']['element_wrapper_class'] = 'article-title';
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '300',
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
$handler->display->display_options['fields']['view_node']['alter']['text'] = '<span class="read-more">[view_node]</a>';
$handler->display->display_options['fields']['view_node']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['view_node']['alter']['link_class'] = 'read-more';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read more';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Has taxonomy term ID (with depth) */
$handler->display->display_options['arguments']['term_node_tid_depth']['id'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['table'] = 'node';
$handler->display->display_options['arguments']['term_node_tid_depth']['field'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['title_enable'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['title'] = 'All %1';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['term_node_tid_depth']['specify_validation'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['validate']['type'] = 'taxonomy_term';
$handler->display->display_options['arguments']['term_node_tid_depth']['validate_options']['type'] = 'convert';
$handler->display->display_options['arguments']['term_node_tid_depth']['validate_options']['transform'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['validate']['fail'] = 'empty';
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
  'further' => 'further',
  'headline' => 'headline',
  'views_article' => 'views_article',
);

/* Display: Grid Tiles */
$handler = $view->new_display('attachment', 'Grid Tiles', 'categorized_news_remaining');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'remaining-news-article grid-layout';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '0';
$handler->display->display_options['pager']['options']['offset'] = '0';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['fields'] = FALSE;
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
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['label'] = '';
$handler->display->display_options['fields']['type']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['type']['alter']['text'] = '<span class="node-type">[type]</span>';
$handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['title']['alter']['link_class'] = 'article-title';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['element_wrapper_type'] = 'span';
$handler->display->display_options['fields']['title']['element_wrapper_class'] = 'article-title';
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'smart_trim_format';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '300',
  'trim_type' => 'chars',
  'trim_suffix' => '...',
  'more_link' => '0',
  'more_text' => 'Read more',
  'summary_handler' => 'ignore',
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
$handler->display->display_options['fields']['view_node']['alter']['text'] = '<span class="read-more">[view_node]</a>';
$handler->display->display_options['fields']['view_node']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['view_node']['alter']['link_class'] = 'read-more';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read more';
$handler->display->display_options['path'] = 'more';
