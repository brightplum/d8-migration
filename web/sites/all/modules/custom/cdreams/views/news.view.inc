<?php

$view = new view();
$view->name = 'news';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'News';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'News';
$handler->display->display_options['css_class'] = 'primary-news-article';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '1';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
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
/* Sort criterion: Content: Promoted to front page */
$handler->display->display_options['sorts']['promote']['id'] = 'promote';
$handler->display->display_options['sorts']['promote']['table'] = 'node';
$handler->display->display_options['sorts']['promote']['field'] = 'promote';
$handler->display->display_options['sorts']['promote']['order'] = 'DESC';
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
  'headline' => 'headline',
);

/* Display: Primary, 1 */
$handler = $view->new_display('block', 'Primary, 1', 'news_block_primary');
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
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

/* Display: Secondary, 2 */
$handler = $view->new_display('block', 'Secondary, 2', 'news_block_secondary');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'secondary-news-article';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '1';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
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
  'image_style' => 'cd_medium_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['alter']['strip_tags'] = TRUE;
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
$handler->display->display_options['fields']['body']['settings'] = array(
  'trim_length' => '100',
  'field_classes' => array(),
);
/* Field: Content: Link */
$handler->display->display_options['fields']['view_node']['id'] = 'view_node';
$handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['view_node']['field'] = 'view_node';
$handler->display->display_options['fields']['view_node']['label'] = '';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read More';

/* Display: Tertiary, 3-4 */
$handler = $view->new_display('block', 'Tertiary, 3-4', 'news_block_tertiary');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'tertiary-news-article';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '2';
$handler->display->display_options['pager']['options']['offset'] = '2';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
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
  'image_style' => 'cd_small_thumb',
  'image_link' => 'content',
  'field_classes' => array(),
);
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['alter']['strip_tags'] = TRUE;
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

/* Display: The Rest, 5-16 */
$handler = $view->new_display('block', 'The Rest, 5-16', 'news_block_therest');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'remaining-news-article grid-layout';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '12';
$handler->display->display_options['pager']['options']['offset'] = '4';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['style_options']['row_class'] = 'grid-tile';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['label'] = 'All News';
$handler->display->display_options['footer']['area']['content'] = '<a href="/news" class="chevron">All News</a>';
$handler->display->display_options['footer']['area']['format'] = 'full_html';
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

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_all_headline_news');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '20';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
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
  'title' => 'title',
  'body' => 'title',
  'view_node' => 'title',
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
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '<br />',
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
/* Field: Field: Organization Image */
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
$handler->display->display_options['fields']['view_node']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['view_node']['alter']['link_class'] = 'chevron';
$handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['view_node']['text'] = 'Read more';
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
$handler->display->display_options['path'] = 'news';

/* Display: Today's Top News */
$handler = $view->new_display('block', 'Today\'s Top News', 'block_todays_top_news');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Top News';
$handler->display->display_options['defaults']['use_more'] = FALSE;
$handler->display->display_options['use_more'] = TRUE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_text'] = FALSE;
$handler->display->display_options['use_more_text'] = 'More';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '4';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_article_img' => 'field_article_img',
  'field_hp_title' => 'field_hp_title',
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
  'field_article_img' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
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
/* Field: Content: Article Image */
$handler->display->display_options['fields']['field_article_img']['id'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['table'] = 'field_data_field_article_img';
$handler->display->display_options['fields']['field_article_img']['field'] = 'field_article_img';
$handler->display->display_options['fields']['field_article_img']['label'] = '';
$handler->display->display_options['fields']['field_article_img']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_article_img']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_article_img']['settings'] = array(
  'image_style' => 'thumbnail',
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

/* Display: Feed */
$handler = $view->new_display('feed', 'Feed', 'feed_1');
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'rss';
$handler->display->display_options['row_plugin'] = 'node_rss';
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
$handler->display->display_options['path'] = 'feed/headlines_rss';
