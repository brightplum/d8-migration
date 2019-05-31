<?php

$template = array(
  'field' => array(
    'field_desc' => array(
      'instance' => array(
        'label' => 'Bio',
        'widget' => array(
          'weight' => '2',
          'type' => 'text_textarea_with_summary',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '20',
            'summary_rows' => 5,
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'display_summary' => 1,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'text_default',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => '2',
          ),
          'teaser' => array(
            'label' => 'hidden',
            'type' => 'smart_trim_format',
            'settings' => array(
              'trim_length' => '600',
              'trim_type' => 'chars',
              'trim_suffix' => '...',
              'more_link' => '0',
              'more_text' => 'Read more',
              'summary_handler' => 'full',
              'trim_options' => array(
                'text' => 0,
              ),
              'field_classes' => array(),
            ),
            'module' => 'smart_trim',
            'weight' => '1',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
          ),
        ),
        'required' => 1,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_desc',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_desc',
        'type' => 'text_with_summary',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_image' => array(
      'instance' => array(
        'label' => 'Photo',
        'widget' => array(
          'weight' => '1',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'thumbnail',
          ),
        ),
        'settings' => array(
          'file_directory' => 'authors',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 1,
          'title_field' => 0,
          'default_image' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'image',
            'settings' => array(
              'image_style' => 'cd_bio_small',
              'image_link' => 'content',
              'field_classes' => array(),
            ),
            'module' => 'image',
            'weight' => '1',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'hidden',
            'settings' => array(
              'image_style' => 'cd_bio_small',
              'image_link' => 'content',
              'field_classes' => array(),
            ),
            'weight' => '2',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
          ),
        ),
        'required' => 0,
        'description' => '',
        'field_name' => 'field_image',
        'deleted' => '0',
      ),
      'field' => array(
        'settings' => array(
          'uri_scheme' => 'public',
          'default_image' => 0,
        ),
        'translatable' => '0',
        'field_name' => 'field_image',
        'type' => 'image',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '5',
      ),
      'title' => array(
        'weight' => '0',
      ),
      'path' => array(
        'weight' => '4',
      ),
      'redirect' => array(
        'weight' => '3',
      ),
    ),
    'display' => array(),
  ),
  'field_wrapper' => array(),
);
