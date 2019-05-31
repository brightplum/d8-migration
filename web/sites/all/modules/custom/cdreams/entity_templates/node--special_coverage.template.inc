<?php

$template = array(
  'bundle_info' => array(
    'type' => 'special_coverage',
    'name' => 'Special Coverage',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'A Special Coverage Sidebar Block.  ',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'special_coverage',
    'disabled_changed' => FALSE,
  ),
  'field' => array(
    'body' => array(
      'instance' => array(
        'label' => 'Body',
        'widget' => array(
          'type' => 'text_textarea_with_summary',
          'settings' => array(
            'rows' => 20,
            'summary_rows' => 5,
          ),
          'weight' => '3',
          'module' => 'text',
        ),
        'settings' => array(
          'display_summary' => TRUE,
          'text_processing' => 1,
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
            'weight' => '2',
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
        'required' => FALSE,
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
    'field_featured_image' => array(
      'instance' => array(
        'label' => 'Featured Image',
        'widget' => array(
          'weight' => '2',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'cd_medium_thumb',
          ),
        ),
        'settings' => array(
          'file_directory' => '',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '370x30',
          'alt_field' => 0,
          'title_field' => 0,
          'default_image' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'image',
            'settings' => array(
              'image_style' => 'cd_special_coverage',
              'image_link' => '',
              'field_classes' => array(),
            ),
            'module' => 'image',
            'weight' => '1',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
          ),
        ),
        'required' => 0,
        'description' => '',
        'field_name' => 'field_featured_image',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'uri_scheme' => 'public',
          'default_image' => 0,
        ),
        'field_name' => 'field_featured_image',
        'type' => 'image',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '7',
      ),
      'title' => array(
        'weight' => '1',
      ),
      'path' => array(
        'weight' => '4',
      ),
      'redirect' => array(
        'weight' => '5',
      ),
      'xmlsitemap' => array(
        'weight' => '6',
      ),
    ),
    'display' => array(),
  ),
  'field_wrapper' => array(),
);

