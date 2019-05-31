<?php

$template = array(
  'bundle_info' => array(
    'type' => 'newswire',
    'name' => 'Newswire article',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'An article that will appear in the "newswire" section of the site.',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'newswire',
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
          'weight' => '4',
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
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => '5',
          ),
          'teaser' => array(
            'label' => 'hidden',
            'type' => 'text_summary_or_trimmed',
            'settings' => array(
              'trim_length' => 600,
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'text',
            'weight' => '0',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '1',
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
    'field_src_url' => array(
      'instance' => array(
        'label' => 'Organization URL',
        'widget' => array(
          'weight' => '1',
          'type' => 'link_field',
          'module' => 'link',
          'active' => 0,
          'settings' => array(),
        ),
        'settings' => array(
          'validate_url' => 1,
          'url' => 'optional',
          'title' => 'optional',
          'title_value' => '',
          'title_maxlength' => '128',
          'enable_tokens' => 1,
          'display' => array(
            'url_cutoff' => '80',
          ),
          'attributes' => array(
            'target' => 'user',
            'rel' => 'nofollow',
            'class' => '',
            'configurable_title' => 0,
            'title' => '',
          ),
          'rel_remove' => 'default',
          'linkit' => array(
            'enable' => 0,
            'profile' => '',
            'button_text' => 'Search',
          ),
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'link_default',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'link',
            'weight' => '2',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '4',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '4',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_src_url',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'attributes' => array(
            'target' => 'default',
            'class' => '',
            'rel' => '',
          ),
          'url' => 0,
          'title' => 'optional',
          'title_value' => '',
          'title_maxlength' => 128,
          'enable_tokens' => 1,
          'display' => array(
            'url_cutoff' => 80,
          ),
        ),
        'field_name' => 'field_src_url',
        'type' => 'link_field',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_desc' => array(
      'instance' => array(
        'label' => 'Description / Mission statement',
        'widget' => array(
          'weight' => '2',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'text_default',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => '3',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '5',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '6',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_desc',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_desc',
        'type' => 'text_long',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_location' => array(
      'instance' => array(
        'label' => 'Location',
        'widget' => array(
          'weight' => '8',
          'type' => 'text_textfield',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'size' => '60',
          ),
        ),
        'settings' => array(
          'text_processing' => '0',
          'linkit' => array(
            'enable' => 0,
            'profile' => '',
            'button_text' => 'Search',
          ),
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'text_default',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => '4',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '7',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '8',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => array(
          array(
            'value' => 'Washington',
          ),
        ),
        'field_name' => 'field_location',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_location',
        'type' => 'text',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_hp_title' => array(
      'instance' => array(
        'label' => 'Main headline title',
        'widget' => array(
          'weight' => '5',
          'type' => 'text_textfield',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'size' => '60',
          ),
        ),
        'settings' => array(
          'text_processing' => '0',
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'hidden',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'weight' => '2',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '10',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '11',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_hp_title',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_hp_title',
        'type' => 'text',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_menu_categories' => array(
      'instance' => array(
        'label' => 'Menu Categories',
        'widget' => array(
          'weight' => '2',
          'type' => 'options_buttons',
          'module' => 'options',
          'active' => 1,
          'settings' => array(),
        ),
        'settings' => array(
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'taxonomy_term_reference_link',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'taxonomy',
            'weight' => 11,
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '11',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '12',
          ),
        ),
        'required' => 1,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_menu_categories',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'allowed_values' => array(
            array(
              'vocabulary' => 'menu_categories',
              'parent' => '0',
            ),
          ),
        ),
        'field_name' => 'field_menu_categories',
        'type' => 'taxonomy_term_reference',
        'locked' => '0',
        'cardinality' => '-1',
      ),
    ),
    'field_article_img' => array(
      'instance' => array(
        'label' => 'Article Image',
        'widget' => array(
          'weight' => '9',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'thumbnail',
          ),
        ),
        'settings' => array(
          'file_directory' => 'newswire',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 1,
          'title_field' => 1,
          'default_image' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'image',
            'settings' => array(
              'image_style' => '',
              'image_link' => '',
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'image',
            'weight' => '2',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '13',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '14',
          ),
        ),
        'required' => 0,
        'description' => '',
        'field_name' => 'field_article_img',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'uri_scheme' => 'public',
          'default_image' => '54501',
        ),
        'field_name' => 'field_article_img',
        'type' => 'image',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_main_caption' => array(
      'instance' => array(
        'label' => 'Article Image Caption',
        'widget' => array(
          'weight' => '9',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'linkit' => array(
            'enable' => 0,
            'insert_plugin' => '',
          ),
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
            'weight' => '3',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '14',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '15',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_main_caption',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_main_caption',
        'type' => 'text_long',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_image' => array(
      'instance' => array(
        'label' => 'Homepage Thumbnail',
        'widget' => array(
          'weight' => '9',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'thumbnail',
          ),
        ),
        'settings' => array(
          'file_directory' => 'newswire/thumbs',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 1,
          'title_field' => 1,
          'default_image' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'hidden',
            'settings' => array(
              'image_style' => '',
              'image_link' => '',
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'weight' => '0',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '15',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '16',
          ),
        ),
        'required' => 1,
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
    'field_subhead_2' => array(
      'instance' => array(
        'label' => '2nd Subtitle',
        'widget' => array(
          'weight' => '7',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'linkit' => array(
            'enable' => 0,
            'insert_plugin' => '',
          ),
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
            'weight' => '1',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '16',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '17',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_subhead_2',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_subhead_2',
        'type' => 'text_long',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_subtitle' => array(
      'instance' => array(
        'label' => 'Subtitle',
        'widget' => array(
          'weight' => '6',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'linkit' => array(
            'enable' => 0,
            'insert_plugin' => '',
          ),
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
            'weight' => '0',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '17',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '18',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_subtitle',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_subtitle',
        'type' => 'text_long',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_article_date' => array(
      'instance' => array(
        'label' => 'Article Date',
        'widget' => array(
          'weight' => '1',
          'type' => 'date_popup',
          'module' => 'date',
          'active' => 1,
          'settings' => array(
            'input_format' => 'm/d/Y - H:i:s',
            'input_format_custom' => '',
            'year_range' => '-3:+3',
            'increment' => '15',
            'label_position' => 'above',
            'text_parts' => array(),
          ),
        ),
        'settings' => array(
          'default_value' => 'now',
          'default_value_code' => '',
          'default_value2' => 'same',
          'default_value_code2' => '',
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'date_default',
            'settings' => array(
              'format_type' => 'long',
              'multiple_number' => '',
              'multiple_from' => '',
              'multiple_to' => '',
              'fromto' => 'both',
              'field_classes' => array(),
            ),
            'module' => 'date',
            'weight' => 13,
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
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
        'field_name' => 'field_article_date',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'granularity' => array(
            'month' => 'month',
            'day' => 'day',
            'year' => 'year',
            'hour' => 0,
            'minute' => 0,
            'second' => 0,
          ),
          'tz_handling' => 'none',
          'timezone_db' => '',
          'cache_enabled' => 0,
          'cache_count' => '4',
          'todate' => '',
        ),
        'field_name' => 'field_article_date',
        'type' => 'datestamp',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_topics' => array(
      'instance' => array(
        'label' => 'Tags',
        'widget' => array(
          'weight' => '3',
          'type' => 'taxonomy_autocomplete',
          'module' => 'taxonomy',
          'active' => 0,
          'settings' => array(
            'size' => 60,
            'autocomplete_path' => 'taxonomy/autocomplete',
          ),
        ),
        'settings' => array(
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'taxonomy_term_reference_link',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'taxonomy',
            'weight' => 14,
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
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
        'default_value' => NULL,
        'field_name' => 'field_topics',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'allowed_values' => array(
            array(
              'vocabulary' => 'topic',
              'parent' => '0',
            ),
          ),
        ),
        'field_name' => 'field_topics',
        'type' => 'taxonomy_term_reference',
        'locked' => '0',
        'cardinality' => '-1',
      ),
    ),
    'field_organization' => array(
      'instance' => array(
        'label' => 'Organization',
        'widget' => array(
          'weight' => '1',
          'type' => 'entityreference_autocomplete',
          'module' => 'entityreference',
          'active' => 1,
          'settings' => array(
            'match_operator' => 'CONTAINS',
            'size' => '60',
            'path' => '',
          ),
        ),
        'settings' => array(
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'entityreference_label',
            'settings' => array(
              'link' => FALSE,
              'field_classes' => array(),
            ),
            'module' => 'entityreference',
            'weight' => 15,
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
        'default_value' => NULL,
        'field_name' => 'field_organization',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'target_type' => 'cdreams_profile',
          'handler' => 'base',
          'handler_settings' => array(
            'target_bundles' => array(
              'organization' => 'organization',
            ),
            'sort' => array(
              'type' => 'property',
              'property' => 'name',
              'direction' => 'ASC',
            ),
            'behaviors' => array(
              'views-select-list' => array(
                'status' => 0,
              ),
            ),
          ),
        ),
        'field_name' => 'field_organization',
        'type' => 'entityreference',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_contact_info' => array(
      'instance' => array(
        'label' => 'Contact Person',
        'widget' => array(
          'weight' => '3',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '5',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
          'linkit' => array(
            'enable' => 0,
            'profile' => '',
            'button_text' => 'Search',
          ),
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'text_default',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => 16,
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
        'default_value' => NULL,
        'field_name' => 'field_contact_info',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_contact_info',
        'type' => 'text_long',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '14',
      ),
      'title' => array(
        'weight' => '4',
      ),
      'path' => array(
        'weight' => '13',
      ),
      'redirect' => array(
        'weight' => '12',
      ),
      'flag' => array(
        'weight' => '11',
      ),
      'xmlsitemap' => array(
        'weight' => '15',
      ),
    ),
    'display' => array(
      'shareaholic_share_buttons_newswire_above_content' => array(
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '3',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '2',
        ),
      ),
      'cdreams_related' => array(
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '2',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '5',
        ),
      ),
    ),
  ),
  'field_wrapper' => array(
    'form' => array(
      'wrapper_article_images' => array(
        'name' => 'article_images',
        'label' => 'Article Images',
        'weight' => '10',
        'children' => array(
          'field_article_img',
          'field_main_caption',
          'field_image',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_content_fields' => array(
        'name' => 'content_fields',
        'label' => 'Content Fields',
        'weight' => '9',
        'children' => array(
          'field_article_date',
          'field_menu_categories',
          'field_topics',
          'body',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_org_info' => array(
        'name' => 'org_info',
        'label' => 'Organization Info',
        'weight' => '1',
        'children' => array(
          'field_organization',
          'wrapper_org_manual',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'span' => -1,
          'equal_heights' => FALSE,
          'attributes' => array(
            'class' => array(),
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_org_manual' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'collapsible' => 1,
          'collapsed' => 0,
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'org_manual',
        'label' => 'Organization Overrides',
        'weight' => '2',
        'children' => array(
          'field_src_url',
          'field_desc',
          'field_contact_info',
        ),
        'class_name' => 'iwFieldsetWrapper',
      ),
    ),
    'default' => array(
      'wrapper_organization_title' => array(
        'name' => 'organization_title',
        'label' => 'Organization Title',
        'weight' => '0',
        'children' => array(
          'field_override_org',
          'field_override_title',
        ),
        'settings' => array(
          'display_title' => 'hidden',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'separator' => array(
            'prefix' => '',
            'between' => '',
            'suffix' => '',
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'class_name' => 'iwInlineFieldWrapper',
      ),
      'wrapper_override_org' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'cond_field' => 'field_organization',
          'invert_cond' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'override_org',
        'label' => 'Organization Override',
        'weight' => '9',
        'children' => array(
          'field_src_url',
          'field_desc',
          'wrapper_organization_title',
        ),
        'class_name' => 'iwConditionalFieldWrapper',
      ),
    ),
  ),
);

