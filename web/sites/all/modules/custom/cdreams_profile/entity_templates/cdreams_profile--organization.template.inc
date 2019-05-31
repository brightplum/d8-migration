<?php

$template = array(
  'field' => array(
    'field_desc' => array(
      'instance' => array(
        'label' => 'Description / Mission statement',
        'widget' => array(
          'weight' => '2',
          'type' => 'text_textarea',
          'module' => 'text',
          'active' => 1,
          'settings' => array(
            'rows' => '20',
          ),
        ),
        'settings' => array(
          'text_processing' => '1',
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
    'field_contact_info' => array(
      'instance' => array(
        'label' => 'Contact Info',
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
        'description' => 'Organization Contact Information',
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
    'field_image' => array(
      'instance' => array(
        'label' => 'Organization Image',
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
          'file_directory' => 'organizations',
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
              'field_classes' => array(),
            ),
            'module' => 'image',
            'weight' => '0',
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
        'description' => 'Image that represents this organization, and will be used as the image used by Open Graph APIs. ',
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
    'field_org_website' => array(
      'instance' => array(
        'label' => 'Organization website',
        'widget' => array(
          'weight' => '8',
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
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'link',
            'weight' => '4',
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
        'field_name' => 'field_org_website',
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
        'field_name' => 'field_org_website',
        'type' => 'link_field',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_pressmedia_link' => array(
      'instance' => array(
        'label' => 'Pressmedia link',
        'widget' => array(
          'weight' => '9',
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
            'label' => 'hidden',
            'type' => 'link_default',
            'settings' => array(
              'field_classes' => array(),
            ),
            'module' => 'link',
            'weight' => '5',
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
        'field_name' => 'field_pressmedia_link',
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
        'field_name' => 'field_pressmedia_link',
        'type' => 'link_field',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
    'field_action_link' => array(
      'instance' => array(
        'label' => 'Action page link',
        'widget' => array(
          'weight' => '10',
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
            'settings' => array(
              'empty_fields_handler' => '',
              'empty_fields_emptyfieldtext_empty_text' => '',
              'field_classes' => array(),
            ),
            'module' => 'link',
            'weight' => '6',
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
        'field_name' => 'field_action_link',
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
        'field_name' => 'field_action_link',
        'type' => 'link_field',
        'locked' => '0',
        'cardinality' => '1',
      ),
    ),
  ),
  'field_extras' => array(
    'form' => array(
      'metatags' => array(
        'weight' => '8',
      ),
      'title' => array(
        'weight' => '0',
      ),
      'path' => array(
        'weight' => '7',
      ),
      'redirect' => array(
        'weight' => '6',
      ),
    ),
    'display' => array(),
  ),
  'field_wrapper' => array(
    'form' => array(
      'wrapper_links' => array(
        'name' => 'links',
        'label' => 'Links',
        'weight' => '5',
        'children' => array(
          'field_org_website',
          'field_pressmedia_link',
          'field_action_link',
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

