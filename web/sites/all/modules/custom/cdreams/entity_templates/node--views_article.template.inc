<?php

$template = array(
  'bundle_info' => array(
    'type' => 'views_article',
    'name' => 'Views',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'An article that will appear in the "views" section of the site.',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'views_article',
    'disabled_changed' => FALSE,
  ),
  'field' => array(
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
            'label' => 'above',
            'type' => 'taxonomy_term_reference_link',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'taxonomy',
            'weight' => '7',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '2',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '1',
          ),
        ),
        'required' => 0,
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
            'weight' => '3',
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
            'weight' => '2',
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
            'type' => 'hidden',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'weight' => '1',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '3',
          ),
          'related' => array(
            'type' => 'text_default',
            'label' => 'hidden',
            'settings' => array(
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'weight' => '3',
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
    'field_copyright' => array(
      'instance' => array(
        'label' => 'Copyright info',
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
            'weight' => '6',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '5',
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
    'field_author_bio' => array(
      'instance' => array(
        'label' => 'Author Info',
        'widget' => array(
          'weight' => '1',
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
          'display_summary' => 0,
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
            'weight' => '5',
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
            'weight' => '7',
          ),
        ),
        'required' => 0,
        'description' => "Use this field to override the Author Info found on the author's profile page.",
        'default_value' => NULL,
        'field_name' => 'field_author_bio',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(),
        'field_name' => 'field_author_bio',
        'type' => 'text_with_summary',
        'locked' => '0',
        'cardinality' => '1',
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
            'weight' => 10,
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
    'field_subtitle' => array(
      'instance' => array(
        'label' => 'Subtitle',
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
          'linkit' => array(
            'enable' => 0,
            'insert_plugin' => '',
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
            'weight' => '0',
          ),
          'teaser' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '9',
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
    'field_src_url' => array(
      'instance' => array(
        'label' => 'Source URL',
        'widget' => array(
          'weight' => '1',
          'type' => 'link_field',
          'module' => 'link',
          'active' => 0,
          'settings' => array(),
        ),
        'settings' => array(
          'validate_url' => 1,
          'url' => 0,
          'title' => 'optional',
          'title_value' => '',
          'title_maxlength' => '128',
          'enable_tokens' => 1,
          'display' => array(
            'url_cutoff' => '80',
          ),
          'attributes' => array(
            'target' => '_blank',
            'rel' => 'nofollow',
            'class' => '',
            'configurable_title' => 0,
            'title' => '',
          ),
          'rel_remove' => 'default',
          'linkit' => array(
            'enable' => 0,
            'insert_plugin' => '',
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
            'weight' => '11',
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
    'field_image' => array(
      'instance' => array(
        'label' => 'Homepage Thumbnail',
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
          'file_directory' => 'views-article/thumbs',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 1,
          'title_field' => 1,
          'default_image' => '54523',
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
            'weight' => '13',
          ),
          'related' => array(
            'type' => 'hidden',
            'label' => 'above',
            'settings' => array(),
            'weight' => '13',
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
          'file_directory' => 'views-article',
          'file_extensions' => 'png gif jpg jpeg',
          'max_filesize' => '',
          'max_resolution' => '',
          'min_resolution' => '',
          'alt_field' => 0,
          'title_field' => 0,
          'default_image' => '54522',
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'image',
            'settings' => array(
              'image_style' => '',
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
            'weight' => '14',
          ),
          'related' => array(
            'type' => 'image',
            'label' => 'hidden',
            'settings' => array(
              'image_style' => 'cd_medium_thumb',
              'image_link' => 'content',
              'field_classes' => array(),
            ),
            'weight' => '2',
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
    'field_main_caption' => array(
      'instance' => array(
        'label' => 'Article Image Caption',
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
            'weight' => '2',
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
            'weight' => '14',
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
    'field_author_name' => array(
      'instance' => array(
        'label' => 'Author Name',
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
            'weight' => 9,
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
            'weight' => 11,
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
            'weight' => 8,
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
            'weight' => 12,
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
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '14',
      ),
      'title' => array(
        'weight' => '1',
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
      'shareaholic_share_buttons_views_article_above_content' => array(
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '4',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '6',
        ),
        'default' => array(
          'visible' => TRUE,
          'weight' => '-1000',
        ),
      ),
      'cdreams_related' => array(
        'teaser' => array(
          'visible' => FALSE,
          'weight' => '1',
        ),
        'related' => array(
          'visible' => FALSE,
          'weight' => '3',
        ),
        'default' => array(
          'visible' => TRUE,
          'weight' => '20',
        ),
      ),
    ),
  ),
  'field_wrapper' => array(
    'form' => array(
      'wrapper_article_images' => array(
        'name' => 'article_images',
        'label' => 'Article Images',
        'weight' => '4',
        'children' => array(
          'field_main_caption',
          'field_image',
          'field_article_img',
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
        'label' => 'Author Manual',
        'weight' => '2',
        'children' => array(
          'field_author_bio',
          'field_author_name',
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
          'span' => -1,
          'equal_heights' => FALSE,
          'attributes' => array(
            'class' => array(),
          ),
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_source_info' => array(
        'name' => 'source_info',
        'label' => 'Source Info',
        'weight' => '10',
        'children' => array(
          'field_src_profile',
          'wrapper_source_override',
        ),
        'settings' => array(
          'display_title' => 'visible',
          'attributes' => array(
            'class' => array(),
          ),
          'span' => -1,
          'equal_heights' => FALSE,
        ),
        'class_name' => 'iwContentWrapper',
      ),
      'wrapper_source_override' => array(
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
        'name' => 'source_override',
        'label' => 'Source Override',
        'weight' => '2',
        'children' => array(
          'field_src_url',
          'field_copyright',
        ),
        'class_name' => 'iwFieldsetWrapper',
      ),
    ),
    'default' => array(
      'wrapper_author_override' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'cond_field' => 'field_author_profile',
          'invert_cond' => 0,
          'attributes' => array(
            'class' => array(),
          ),
          'actions' => array(
            'settings_save' => 'Save Changes',
            'settings_cancel' => 'Cancel',
          ),
        ),
        'name' => 'author_override',
        'label' => 'Author Override',
        'weight' => '5',
        'children' => array(
          'field_orig_author',
          'field_author_bio',
        ),
        'class_name' => 'iwConditionalFieldWrapper',
      ),
      'wrapper_source_override' => array(
        'settings' => array(
          'display_title' => 'visible',
          'span' => '-1',
          'equal_heights' => 0,
          'cond_field' => 'field_src_profile',
          'invert_cond' => 0,
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
        'weight' => '7',
        'children' => array(
          'field_src_author',
          'field_src_url',
          'field_copyright',
        ),
        'class_name' => 'iwConditionalFieldWrapper',
      ),
    ),
  ),
);

