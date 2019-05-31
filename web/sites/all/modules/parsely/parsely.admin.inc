<?php
/**
 * @file
 * admin page file containing the menu callback function.
 */

/**
 * Main settings form. 
 * @function - form elements for Parsely Drupal settings. 
 * returns an array of form elements to validate and submit functions. 
 */
function parsely_admin_settings($form, &$form_state) {  
  $form['parsely_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Parse.ly Site ID'),
    '#description' => t('You can find your Site ID on your your <a target="_blank" href="@parsely_settings">API settings page</a>.',
              array('@parsely_settings' => 'http://dash.parsely.com/to/settings/api?highlight=apikey')),
    '#default_value' => variable_get('parsely_apikey'),
  );
  


  $node_get_types = array_keys(node_type_get_types());
  foreach ($node_get_types as $key => $bid){ 
    $node_types[$bid] = $bid;
  }

  $form['parsely_section_logic'] = array(
    '#type' => 'radios',
    '#title' => t('Use content (node) types or vocabulary (taxonomy) as sections?'),    
    '#options' => array('node', 'taxonomy'),
    '#default_value' => variable_get('parsely_section_logic'),
    );


  if (module_exists('node')) {

    $form['parsely_nodes_wrap'] = array(
      '#type' => 'fieldset',
      '#title' => t('Node types to track'),
      '#description' => t('<em>Select which node types to track. (Note: If you selected use node types above, these will be your available sections.)</em>'),
    //'#collapsible' => TRUE,
    //'#collapsed' => FALSE,
  );
    
    $form['parsely_nodes_wrap']['parsely_nodes'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Available node types:'),    
      '#options' => $node_types,
      '#default_value' => variable_get('parsely_nodes', array()),
    ); 
  
  }

  if (module_exists('taxonomy')) {
        
    $form['parsely_sections_tax_wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Use Taxonomy Term as Section Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    
    $form['parsely_tag_vocabularies'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Tag Vocabularies'),
      '#options' => _parsely_vocab_array_format(taxonomy_get_vocabularies()),
      '#description' => t('<em>Select which taxonomy vocabularies will be tracked as tags.</em>'),
      '#default_value' => variable_get('parsely_tag_vocabularies', array()),
    );
    
    $form['parsely_sections_tax_wrapper']['parsely_section_vocabulary'] = array(
      '#type' => 'radios',
      '#title' => t('Section Vocabulary'),
      '#options' => _parsely_vocab_array_format(taxonomy_get_vocabularies()),
      '#description' => t('Select the taxonomy vocabulary to use for Parse.ly sections. A single term from this vocabulary will be chosen for each tracked node, where applicable, using the criterion specified below.'),
      '#default_value' => variable_get('parsely_section_vocabulary', ''),
    );
    $form['parsely_sections_tax_wrapper']['parsely_section_term_criterion'] = array(
      '#title' => t('Section Term Selection Criteria'),
      '#type' => 'radios',
      '#options' => array(
        'first' => t('First term'),
        'last' => t('Last term'),
        'highest' => t('Highest-level term'),
        'ancestor' => t('Highest-level ancestor'),
      ),
      // Provide a long-winded explanation of how this impacts term selection.
      // Note that there's currently no way to say "find the last/newest term,
      // and then select its highest-level ancestor" or "use the term that
      // appears first in the set of terms for a given term/entity reference
      // field," etc.
      '#description' => t('If a given node may be associated with multiple terms from the vocabulary you selected above, this setting can help determine which term to use. "First term" and "Last term" will choose a term based on the date/time the term was created (this may differ from the relative position of a term within a term reference field). "Highest term" will choose the term that lives closest to the root of the vocabulary (defaulting to the "first"/oldest of such terms if more than one exist). For hierarchical vocabularies, you can also opt to use the "Highest-level ancestor" of a given term, which means that if a given node is tagged with a lower-lever term (e.g. corresponding to a subsection of your site), we will report that term\'s parent (or grandparent, etc.) term as the section. Wherever a choice must be made between terms at the same depth, the first term will be chosen.'),
      '#default_value' => variable_get('parsely_section_term_criterion', 'first'),
    );
    
   
  }

  $parsely_ignore_format = 'text_filter';
  $form['parsely_ignore'] = array(
    '#type' => 'textarea',
    '#rows' => 5,
    '#resizable' => TRUE,
    '#title' => t('Paths to ignore'),
    '#description' => t('List each path to ignore on its own line'),
    '#default_value' => variable_get('parsely_ignore', array('admin','admin/*')),
    '#format' => $parsely_ignore_format,
  );


  // Metadata settings
  $form['parsely_metadata'] = array(
    '#type' => 'fieldset',
    '#title' => t('Metadata'),
    // '#description' => t('Customize the values reported to Parse.ly for each page. Read more about Parse.ly metadata !here.', array('!here' => l('here', 'http://www.parsely.com/docs/integration/metadata/ppage.html'))),

    '#description' => t('Read more about Parse.ly metadata !here.', array('!here' => l('here', 'http://www.parsely.com/docs/integration/metadata/ppage.html'))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Thumbnail image.
  $logo_url = ltrim(theme_get_setting('logo', variable_get('theme_default')), '/');
  $logo_url = url($logo_url, array('absolute' => TRUE));
  
  if (module_exists('token')) {
    $form['parsely_metadata']['#description'] .= ' ' . t('You can use tokens from the list below to specify dynamic patterns for each metadata item.');
    // Set up a token browser so admins can use relevant tokens to define
    // metadata values. Update all textfields in this fieldset accordingly.
    // Note: The ability to provide token_types param to theme_token_tree_link
    // was introduced by the token module on 2014-06-19 in v. 7.x-1.5+4-dev.
    // @see https://www.drupal.org/node/2289203.
    $token_module_info = system_get_info('module', 'token');
    $token_version = $token_module_info['version'];
    if (version_compare($token_version, '7.x-1.5+4-dev', 'ge')) {
      $theme_function = 'token_tree_link';
    }
    else {
      $theme_function = 'token_tree';
    }
    $form['parsely_metadata']['tokens'] = array(
      '#theme'        => $theme_function,
      '#token_types'  => array('node'),
      '#global_types' => TRUE,
      '#click_insert' => TRUE,
    );

    foreach (element_children($form['parsely_metadata']) as $field_key) {
      $field = $form['parsely_metadata'][$field_key];
      if ($field['#type'] == 'textfield') {
        // Note: because we're not requiring a minimum number of tokens for
        // these fields, the only way they can fail validation is if they
        // include a real token that belongs to a disallowed context.
        $field['#element_validate'] = array(
          'token_element_validate',
        );
        $field['#token_types'] = array('node');
        $form['parsely_metadata'][$field_key] = $field;
      }
    }
  }

  // Advanced settings
  $form['parsely_optional_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
    $form['parsely_optional_settings']['parsely_track_auth_users'] = array(
    '#type' => 'radios',
    '#title' => t('Track Authenticated Users'),
    '#description' => t('By default, the Parse.ly module will track the activity of users that are logged into this site. You can change this setting to only track the activity of anonymous visitors. Note: You will no longer see the Parse.ly tracking code on your site if you browse while logged in.'),
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => variable_get('parsely_track_auth_users', 1),
  );  
  $form['parsely_optional_settings']['parsely_content_id_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Content ID Prefix'),
    '#description' => t('If you use more than one content management system (e.g. Drupal and WordPress), you may end up with duplicate content IDs. Adding a Content ID Prefix will ensure the content IDs from Drupal will not conflict with other content management systems.'),
    '#default_value' => variable_get('parsely_content_id_prefix', ''),
  );
  $form['parsely_optional_settings']['parsely_metadata_thumbnail_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Override default thumbnail?'),
    '#description' => t('Enter the machine name of an image field.'),
    '#default_value' => variable_get('parsely_metadata_thumbnail_url', 'field_image'),
    '#maxlength' => 1024,
  );
  $form['parsely_optional_settings']['parsely_authors'] = array(
    '#type' => 'fieldset',
    '#title' => t('Multiple Authors Support'),
    '#description' => t('Setting for sites not using the standard configuration for content authoring.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
    $form['parsely_optional_settings']['parsely_authors']['parsely_authors_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Multiple Authors Support'),
    '#description' => t('If your site supports multiple authors per node, enter the author field machine_name here.'),
    '#default_value' => variable_get('parsely_authors_field', ''),
  );
    $form['parsely_optional_settings']['parsely_authors']['parsely_authors_field_type'] = array(
    '#type' => 'radios',
    '#title' => t('Multiple Authors Field Type'),
    '#description' => t('Indicate whether your custom author field is uses a reference.'),
    '#options' => array('N/A', 'field', 'node reference'),
    '#default_value' => variable_get('parsely_authors_field_type', ''),
  );
    $form['parsely_debug_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debug'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
    $form['parsely_debug_settings']['parsely_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debugging on?'),
    '#description' => t('Toggle this on to dump your Parsley data on every tracked page. <em>NB. You must have the Administer Parsely permission to view this data.</em>'),
    '#default_value' => variable_get('parsely_debug', ''),
  );

  return system_settings_form($form);
}


/**
 * Vocab format
 * @function - formats vocabularlies
 */
function _parsely_vocab_array_format($vocabularies = NULL) {
  $vocab_array = array();
  foreach ($vocabularies as $vocab) {
    $vocab_array[$vocab->vid] = $vocab->name;
  }

  return $vocab_array;
}

