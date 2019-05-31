<?php

$template = array(
  'bundle_info' => array(
    'type' => 'video_article',
    'name' => 'Video Article',
    'base' => 'node_content',
    'module' => 'node',
    'description' => 'add video content, such as YouTube, Google, and RealNews',
    'help' => '',
    'has_title' => '1',
    'title_label' => 'Title',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'disabled' => '0',
    'orig_type' => 'video_article',
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
          'weight' => '6',
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
            'weight' => 0,
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
    'field_video' => array(
      'instance' => array(
        'label' => 'Video',
        'widget' => array(
          'weight' => '5',
          'type' => 'media_generic',
          'module' => 'media',
          'active' => 1,
          'settings' => array(
            'allowed_types' => array(
              'video' => 'video',
              'image' => 0,
              'audio' => 0,
              'document' => 0,
            ),
            'allowed_schemes' => array(
              'vimeo' => 'vimeo',
              'youtube' => 'youtube',
              'flickr' => 0,
              'public' => 0,
              'private' => 0,
            ),
            'browser_plugins' => array(
              'media_default--media_browser_1' => 'media_default--media_browser_1',
              'media_internet' => 'media_internet',
              'youtube' => 'youtube',
              'upload' => 0,
              'media_default--media_browser_my_files' => 0,
            ),
            'progress_indicator' => 'throbber',
          ),
        ),
        'settings' => array(
          'file_directory' => 'video_articles',
          'file_extensions' => 'txt',
          'max_filesize' => '',
          'description_field' => 0,
          'user_register_form' => FALSE,
        ),
        'display' => array(
          'default' => array(
            'label' => 'above',
            'type' => 'file_default',
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'file',
            'weight' => 1,
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
        'field_name' => 'field_video',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'display_field' => 0,
          'display_default' => 0,
          'uri_scheme' => 'public',
        ),
        'field_name' => 'field_video',
        'type' => 'file',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_hp_title' => array(
      'instance' => array(
        'label' => 'Main headline title',
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
            'weight' => 2,
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
    'field_featured_sentence' => array(
      'instance' => array(
        'label' => 'Featured home page video sentence',
        'widget' => array(
          'weight' => '4',
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
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
              'list_inline' => FALSE,
              'list_separator' => ', ',
            ),
            'module' => 'text',
            'weight' => 4,
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
        'field_name' => 'field_featured_sentence',
        'deleted' => '0',
      ),
      'field' => array(
        'translatable' => '0',
        'settings' => array(
          'max_length' => '255',
        ),
        'field_name' => 'field_featured_sentence',
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
            'weight' => 5,
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
    'field_src_url' => array(
      'instance' => array(
        'label' => 'Source title & URL',
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
            'weight' => 7,
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
    'field_subtitle' => array(
      'instance' => array(
        'label' => 'Subtitle',
        'widget' => array(
          'weight' => '3',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '1',
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
            'weight' => 8,
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
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '11',
      ),
      'title' => array(
        'weight' => '2',
      ),
      'path' => array(
        'weight' => '9',
      ),
      'redirect' => array(
        'weight' => '8',
      ),
      'flag' => array(
        'weight' => '10',
      ),
      'xmlsitemap' => array(
        'weight' => '12',
      ),
    ),
    'display' => array(),
  ),
  'field_wrapper' => array(
    'form' => array(
      'wrapper_source_info' => array(
        'name' => 'source_info',
        'label' => 'Source Info',
        'weight' => '7',
        'children' => array(
          'field_src_url',
          'field_copyright',
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
    ),
  ),
);

