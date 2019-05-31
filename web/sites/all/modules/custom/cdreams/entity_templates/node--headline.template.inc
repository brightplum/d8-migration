<?php

$template = array(
  'bundle_info' => array(
    'type' => 'headline',
    'name' => 'News',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'An article that will appear in the "News/Headlines" section of the site.',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'headline',
    'disabled_changed' => FALSE,
  ),
  'field' => array(
    'field_author_name' => array(
      'instance' => array(
        'label' => 'Author Name',
        'widget' => array(
          'weight' => '1',
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
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => 19,
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
        'field_name' => 'field_author_name',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_author_name',
        'type' => 'text',
        'locked' => '0',
        'cardinality' => '-1',
      ),
    ),
    'field_author_profile' => array(
      'instance' => array(
        'label' => 'Author Profile(s)',
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
            'weight' => 18,
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
        'field_name' => 'field_author_profile',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'target_type' => 'cdreams_profile',
          'handler' => 'base',
          'handler_settings' => array(
            'target_bundles' => array(
              'author' => 'author',
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
        'field_name' => 'field_author_profile',
        'type' => 'entityreference',
        'locked' => '0',
        'cardinality' => '-1',
      ),
    ),
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
            'weight' => '6',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
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
    'field_hp_title' => array(
      'instance' => array(
        'label' => 'Main headline title',
        'widget' => array(
          'weight' => '2',
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
            'weight' => '0',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '3',
          ),
          'related' => array(
            'type' => 'text_plain',
            'label' => 'hidden',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'weight' => '2',
            'module' => 'text',
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
    'field_src_url' => array(
      'instance' => array(
        'label' => 'Source URL',
        'widget' => array(
          'weight' => '2',
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
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'link_default',
            'weight' => '1',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'link',
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
    'field_copyright' => array(
      'instance' => array(
        'label' => 'Copyright info',
        'widget' => array(
          'weight' => '3',
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
            'type' => 'text_default',
            'weight' => '2',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '8',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '9',
          ),
        ),
        'required' => 0,
        'description' => '',
        'default_value' => NULL,
        'field_name' => 'field_copyright',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_copyright',
        'type' => 'text',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_article_img' => array(
      'instance' => array(
        'label' => 'Article Image',
        'widget' => array(
          'weight' => '4',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'thumbnail',
          ),
        ),
        'settings' => array(
          'file_directory' => 'headlines',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 1,
          'title_field' => 1,
          'default_image' => '54500',
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'image',
            'weight' => '4',
            'settings' => array(
              'image_style' => 'cd_large',
              'image_link' => '',
              'field_classes' => array(),
            ),
            'module' => 'image',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '9',
          ),
          'related' => array(
            'type' => 'image',
            'label' => 'hidden',
            'settings' => array(
              'image_style' => 'cd_medium_thumb',
              'image_link' => 'content',
              'field_classes' => array(),
            ),
            'weight' => '1',
            'module' => 'image',
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
    'field_image' => array(
      'instance' => array(
        'label' => 'Homepage thumbnail',
        'widget' => array(
          'weight' => '4',
          'type' => 'image_image',
          'module' => 'image',
          'active' => 1,
          'settings' => array(
            'progress_indicator' => 'throbber',
            'preview_image_style' => 'thumbnail',
          ),
        ),
        'settings' => array(
          'file_directory' => 'headline/thumbs',
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
            'weight' => '1',
            'settings' => array(
              'image_style' => '',
              'image_link' => '',
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
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
    'field_main_caption' => array(
      'instance' => array(
        'label' => 'Main Image Caption',
        'widget' => array(
          'weight' => '4',
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
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => '5',
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
        'required' => 0,
        'description' => 'This is the caption that appears under the primary article image on the display page. This is to allow long captions to appear under the main / primary article image.',
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
    'field_subtitle' => array(
      'instance' => array(
        'label' => 'Subtitle',
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
            'insert_plugin' => '',
          ),
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
            'weight' => '1',
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
            'input_format' => 'm/d/Y - g:i:sa',
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
            'weight' => 22,
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
    'field_menu_categories' => array(
      'instance' => array(
        'label' => 'Category',
        'widget' => array(
          'weight' => '2',
          'type' => 'options_select',
          'module' => 'options',
          'active' => 1,
          'settings' => array(),
        ),
        'settings' => array(
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'taxonomy_term_reference_link',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => 1,
              'list_separator' => ', ',
            ),
            'module' => 'taxonomy',
            'weight' => '15',
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
            'weight' => 16,
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
    'field_hp_author' => array(
      'instance' => array(
        'label' => 'Homepage Author',
        'widget' => array(
          'weight' => '2',
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
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => 20,
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => 0,
          ),
        ),
        'required' => 0,
        'description' => 'Author listing or name that will appear on the home page. If left blank, this field will auto populate with profiles or author name fields.',
        'default_value' => NULL,
        'field_name' => 'field_hp_author',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_hp_author',
        'type' => 'text',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_src_profile' => array(
      'instance' => array(
        'label' => 'Source Profile',
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
            'weight' => 21,
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
        'field_name' => 'field_src_profile',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'target_type' => 'cdreams_profile',
          'handler' => 'base',
          'handler_settings' => array(
            'target_bundles' => array(
              'source' => 'source',
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
        'field_name' => 'field_src_profile',
        'type' => 'entityreference',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '13',
      ),
      'title' => array(
        'weight' => '1',
      ),
      'path' => array(
        'weight' => '11',
      ),
      'redirect' => array(
        'weight' => '10',
      ),
      'flag' => array(
        'weight' => '12',
      ),
      'xmlsitemap' => array(
        'weight' => '14',
      ),
    ),
    'display' => array(
      'shareaholic_share_buttons_headline_above_content' => array(
        'default' => array(
          'visible' => TRUE,
          'weight' => '3',
        ),
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '1',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '2',
        ),
      ),
      'shareaholic_share_buttons_headline_below_content' => array(
        'default' => array(
          'visible' => TRUE,
          'weight' => '1',
        ),
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '2',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '3',
        ),
      ),
      'cdreams_related' => array(
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '4',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '4',
        ),
        'default' => array(
          'visible' => TRUE,
          'weight' => '13',
        ),
      ),
    ),
  ),
  'field_wrapper' => array(
    'form' => array(
      'wrapper_author_manual' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'collapsible' => 1,
          'collapsed' => 1,
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'author_manual',
        'label' => 'Manual',
        'weight' => '2',
        'children' => array(
          'field_author_name',
          'field_hp_author',
        ),
        'class_name' => 'iwFieldsetWrapper',
      ),
      'wrapper_content_fields' => array(
        'name' => 'content_fields',
        'label' => 'Content Fields',
        'weight' => '5',
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
      'wrapper_images' => array(
        'name' => 'images',
        'label' => 'Article Image',
        'weight' => '4',
        'children' => array(
          'field_image',
          'field_main_caption',
          'field_article_img',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'attributes' => array(
            'class' => array(),
          ),
          'span' => '-1',
          'equal_heights' => 0,
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_profiles' => array(
        'name' => 'profiles',
        'label' => 'Author Info',
        'weight' => '6',
        'children' => array(
          'field_author_profile',
          'wrapper_author_manual',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'attributes' => array(
            'class' => array(),
          ),
          'span' => '-1',
          'equal_heights' => 0,
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_source_data' => array(
        'name' => 'source_data',
        'label' => 'Source Data',
        'weight' => '7',
        'children' => array(
          'field_src_profile',
          'wrapper_source_info',
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
      'wrapper_source_info' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'collapsible' => 1,
          'collapsed' => 1,
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'source_info',
        'label' => 'Source Manual',
        'weight' => '2',
        'children' => array(
          'field_src_url',
          'field_copyright',
        ),
        'class_name' => 'iwFieldsetWrapper',
      ),
    ),
    'default' => array(
      'wrapper_share_this_article' => array(
        'name' => 'share_this_article',
        'label' => 'Share This Article',
        'weight' => '9',
        'children' => array(
          'shareaholic_share_buttons_headline_below_content',
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
      'wrapper_source_override' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'cond_field' => 'field_src_profile',
          'invert_cond' => 1,
          'attributes' => array(
            'class' => array(),
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'source_override',
        'label' => 'Source Override',
        'weight' => '10',
        'children' => array(
          'field_src_url',
          'field_copyright',
        ),
        'class_name' => 'iwConditionalFieldWrapper',
      ),
    ),
  ),
);

