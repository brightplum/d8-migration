<?php

$template = array(
  'bundle_info' => array(
    'type' => 'breaking_news',
    'name' => 'Breaking News',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'Featured articles that appear just below menu.',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'breaking_news',
    'disabled_changed' => FALSE,
  ),
  'field' => array(
    'body' => array(
      'instance' => array(
        'label' => 'Body',
        'widget' => array(
          'weight' => '41',
          'type' => 'text_textarea_with_summary',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
            'summary_rows' => 5,
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'display_summary' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'text_default',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => 0,
          ),
          'teaser' => array(
            'label' => 'hidden',
            'type' => 'text_summary_or_trimmed',
            'settings' => array(
              'trim_length' => 600,
              'field_classes' => array(),
            ),
            'module' => 'text',
            'weight' => 0,
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'body',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'body',
        'type' => 'text_with_summary',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '40',
      ),
      'title' => array(
        'weight' => '-5',
      ),
      'path' => array(
        'weight' => '30',
      ),
      'redirect' => array(
        'weight' => '30',
      ),
    ),
    'display' => array(),
  ),
  'field_wrapper' => array(),
);

