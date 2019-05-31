<?php
/**
 * @file includes/elements.inc
 * Contains the process, validation and other general utilities for iwapi provided form elements.
 */


// ===============================
// General Element information.
// ===============================

/**
 * Callback to describe the form elements provided by
 * the iwapi module. This supplies the element defaults
 * used by the Drupal form system.
 */
function _iwapi_element_info() {
  $path = drupal_get_path('module', 'iwapi');
  return array(
    'popup_help' => array(
      '#input'          => FALSE,
      '#tooltip_label'  => '?',
      '#theme'          => 'popup_help',
      '#theme_wrappers' => array('container'),
      '#attributes'     => array('class' => array('iw-popup-help')),
      '#attached'       => array(
        'js' => array($path .'/js/elements.js'),
        'css' => array($path . '/theme/css/elements.css'),
        'library' => array(
          array('system', 'ui.dialog'),
          array('iwapi', 'iwapi.core'),
        ),
      ),
    ),
    'email_confirm' => array(
      '#input'            => TRUE,
      '#process'          => array('_iwapi_element_email_confirm_process'),
      '#value_callback'   => '_iwapi_element_email_confirm_value',
      '#element_validate' => array('_iwapi_element_email_confirm_validate'),
    ),
    'select_or_text' => array(
      '#input'            => TRUE,
      '#size'             => 32,
      '#maxlength'        => 128,
      '#process'          => array('_iwapi_element_select_or_text_process'),
      '#element_validate' => array(),
      '#theme_wrappers'   => array('form_element'),
      '#empty_option'     => t('Add new item: '),
      '#placeholder'      => t('Enter new item'),
      '#return_source'    => FALSE,
      '#attached'         => array(
        'js' => array($path .'/js/elements.js'),
        'css' => array($path .'/theme/css/elements.css'),
      ),
    ),
    'table_elements' => array(
      '#input'          => TRUE,
      '#process'        => array('_iwapi_element_table_process'),
      '#theme'          => 'table_elements',
    ),
    'table_orderable' => array(
      '#input'          => TRUE,
      '#process'        => array('_iwapi_element_table_orderable_process'),
      '#value_callback' => '_iwapi_element_table_orderable_value',
      '#theme'          => 'table_elements',
    ),
    'sort_bins' => array(
      '#input'          => TRUE,
      '#process'        => array('_iwapi_element_sort_bins_process'),
      '#theme_wrappers' => array('container'),
      '#value_callback' => '_iwapi_element_sort_bin_value',
      '#inactive_bin'   => TRUE,
      '#attached'       => array(
        'js' => array($path .'/js/group_items.js'),
        'css' => array($path .'/theme/css/elements.css'),
        'library' => array(
          array('system', 'ui.sortable'),
          array('iwapi', 'iwapi.core'),
        ),
      ),
    ),
    'image_selector' => array(
      '#input'              => TRUE,
      '#process'            => array('_iwapi_element_image_selector_process'),
      '#value_callback'     => '_iwapi_element_image_selector_value',
      '#element_validate'   => array('_iwapi_element_image_selector_validate'),
      '#progress_indicator' => 'bar',
      '#image_style'        => FALSE,
      '#extensions'         => 'jpg jpeg png gif',
      '#directory'          => '',
      '#theme_wrappers'     => array('form_element'),
      '#attached'           => array(
        'js' => array($path . '/js/image_selector.js'),
        'css' => array($path . '/theme/css/elements.css'),
        'library' => array(
          array('system', 'ui.dialog'),
          array('iwapi', 'iwapi.core'),
        ),
      ),
    ),
  );
}


// ============================
// Confirm e-mail element
// ============================

/**
 * Form element process callback, to expand the
 * confirm_email element into a 2 textfield item.
 * 
 * @param array $element
 *  Current element settings to be processed.
 * @return array
 *  Returns the fully formed element to be processed
 *  and rendered into the completed form.
 */
function _iwapi_element_email_confirm_process($element) {
  // These are the keys that transfer to the underlying textfield.
  $keys = array_fill_keys(array('#title', '#description', '#required'), TRUE);
  
  // Allow the parent element attributes to propagate to the child items.
  $attributes = array();
  if (isset($element['#input_autocomplete'])) {
    $attributes['autocomplete'] = $element['#input_autocomplete'] ? 'on' : 'off';
  }
  
  $size = isset($element['#size']) ? $element['#size'] : 64;

  $element['email'] = array(
    '#type' => 'textfield',
    '#size' => $size,
    '#default_value' => isset($element['#value']) ? $element['#value'] : '',
    '#attributes' => $attributes,
  ) + array_intersect_key($element, $keys);
  
  if (!isset($element['#confirm_title'])) {
    $element['#confirm_title'] = t("Re-type @title", array('@title' => $element['#title']));
  }
  
  unset($attributes['id']);
  $element['confirm'] = array(
    '#type' => 'textfield',
    '#title' => $element['#confirm_title'],
    '#size' => $size,
    '#default_value' => isset($element['#value']) ? $element['#value'] : '',
    '#attributes' => $attributes,
  );
  
  $element['#tree'] = TRUE;
  $element['#required'] = FALSE;
  return $element;
}

/**
 * Element value callback to determine the current element value.
 * 
 * This can be called when first processed or after validation / submission.
 */
function _iwapi_element_email_confirm_value(&$element, $input = FALSE, $form_state = NULL) {
  if ($input && is_array($input)) {
    return $input['email'];
  }
  else {
    return isset($element['#default_value']) ? $element['#default_value'] : NULL;
  }
}

/**
 *  Element validate callback to confirm email addresses match,
 *  and is a valid e-mail address.
 */
function _iwapi_element_email_confirm_validate(&$element, &$form_state) {
  $email   = trim($element['email']['#value']);
  $confirm = trim($element['confirm']['#value']);

  // Ensure that we are handling a valid e-mail address,
  // then check to ensure that both e-mail fields match.
  if (!(empty($email) || valid_email_address($email))) {
    form_error($element['email'], t('The e-mail address %mail is not valid.', array('%mail' => $email)));
  }
  elseif (strcasecmp($email, $confirm) != 0) {
    // Validate that both e-mail entry fields match.
    form_error($element['confirm'], t('The e-mail fields must match'));
  }
  
  // We should set the value to a the single e-mail field,
  // other methods will be expecting this to be a single field.
  form_set_value($element, $email, $form_state);
}


// ==============================================
// Select or text element functions
// ==============================================

/**
 * Process callback for the select_or_text element.
 * Will expand the element into a select field and
 * a text field.
 * 
 * @param array $element
 *  The form definition of the element to be processed
 *  into its components.
 * @return array
 *  The expanded form element, ready to be further
 *  processed by its subcomponents.
 */
function _iwapi_element_select_or_text_process($element) {
  $options = $element['#options'];
  unset($element['#options']);
  
  if (!empty($options)) {
    $element['select'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#empty_option' => $element['#empty_option'],
      '#value_callback' => '_iwapi_prevent_child_value',
      '#theme_wrappers' => array(),
    );
    
    // Any element AJAX settings apply to the select element only
    if (!empty($element['#ajax'])) {
      $element['select']['#ajax'] = $element['#ajax'];
      unset($element['#ajax']);
    }
  }
    
  $element['text'] = array(
    '#type' => 'textfield',
    '#size' => $element['#size'],
    '#maxlength' => $element['#maxlength'],
    '#value_callback' => '_iwapi_prevent_child_value',
    '#theme_wrappers' => array(),
    '#attributes' => array('placeholder' => $element['#placeholder']),
    '#element_validate' => $element['#element_validate'],
  );
  
  // Determine if a value is set, and how we should set it.
  if (!empty($element['#value'])) {
    $value = is_array($element['#value']) ? $element['#value']['value'] : $element['#value'];
    
    $bucket = isset($options[$value]) ? 'select' : 'text';
    $element[$bucket]['#default_value'] = $value;
  }

  // All the validation only needs to happen on the text field.
  unset($element['#element_validate']);
  
  $element['#markup'] = '';  // Without this, no ID will be tagged.
  $element['#tree'] = TRUE;
  return $element;
}

/**
 * Determine what the values of this select_or_text
 * element currently is. Handles default or
 * submitted values.
 * 
 * @param array $element
 *  The current element definitions.
 * @param string $input
 *  Raw input from the form submission. Can be NULL
 *  if no values or submit hasn't happened.
 * @return array|string|NULL:
 *  Returns a new value for the select_or_text element.
 */
function form_type_select_or_text_value(array &$element, $input = FALSE) {
  if ($input !== FALSE) {
    if ($element['#return_source']) {
      return empty($input['select'])
        ? array('value' => $input['text'],   'source' => 'text')
        : array('value' => $input['select'], 'source' => 'select');
    }
    else {
      return empty($input['select']) ? $input['text'] : $input['select'];
    }
  }
  else {
    if (isset($element['#default_value'])) {
      return $element['#default_value'];
    }
   
    return NULL;
  }
}


// ==============================================
// Table based element functions
// ==============================================

/**
 * Expand the "table_element" element.
 * - Basically only needes to ensure "tree" property is set.
 *
 * @param $element
 *  Currently set element values with defaults assigned in HOOK_element_info().
 */
function _iwapi_element_table_process($element) {
  $element['#tree'] = TRUE;
  $element['#attributes']['class'][] = 'form-elements-table';

  return $element;
}

/**
 * Expand the "table_orderable" element and ensure it is properly structured.
 * 
 * @param $element
 *  Currently set element values with defaults assigned in HOOK_element_info().
 */
function _iwapi_element_table_orderable_process($element) {
  $element['#tree'] = TRUE;

  array_push($element['#header'], t('Weight'));
  if (empty($element['#attributes']['id'])) {
    $element['#attributes']['id'] = $element['#id'] . '-orderable-table';
  }
  $element['#attributes']['class'][] = 'orderable-table';
  
  // Attach the appropriate JS handlers for making the table draggable
  drupal_add_tabledrag($element['#attributes']['id'], 'order', 'sibling', 'weight');

  // Expand the actual form elements.
  $count = 0;
  foreach ($element as $key => &$row) {
    if ($key[0] === '#' || empty($row)) {
      continue;
    }

    // If there is a weight already set by the user, use it as the default value
    if (!empty($row['weight']) && is_numeric($element[$key]['weight'])) {
      $count = intval($row['weight']);
    }
    
    // Append a weight data field, to determine the order of results.
    $row['weight'] = array(
      '#type' => 'textfield',
      '#size' => 5, 
      '#default_value' => $count++,
      '#attributes' => array('class' => array('weight')),
    );
    $row['#attributes']['class'][] = 'draggable';
  }
  return $element;
}

/**
 * Determine the value of the orderable table element. The end result
 * should be all the table values sorted by weight values.
 */
function _iwapi_element_table_orderable_value(&$element, $input = FALSE, $form_state = NULL) {
  if ($input && is_array($input)) {
    uasort($input, 'drupal_sort_weight');
    return $input;
  }
  else {
    return array();
  }
}

/**
 * Determine which header element key is currently active.
 * 
 * @param array $header
 *  Array of table header elements as would be used with
 *  theme_table_elements.
 * @param array $ts
 *  Tablesort settings returned by tablesort_init().
 * @return string|NULL
 *  The header column key if a match to the current sorting
 *  field is found.
 */
function _iwapi_get_header_sort_col(array $header, array $ts) {
  if (!empty($ts['name'])) {
    foreach ($header as $key => $cell) {
      if (isset($cell['data']) && $cell['data'] == $ts['name']) {
        return $key;
      }
    }
  }
  return NULL;
}

/**
 * Transform a row data from a table_elements into HTML output.
 * 
 * @param string $key
 *  The index from the original table, used for this row.
 * @param array $row
 *  The row being rendered.
 * @param array $table
 *  The full table that this row is a part of.
 * @param string $sort_col
 *  The index name of the column which is being sorted on.
 * @return string
 *  HTML row content for this row.
 */
function _iwapi_render_element_row($key, $row, $table, $sort_col = NULL) {
  $row['#attributes']['id'] = $table['#id'] .'-row-'. $key;
  
  if (!empty($item['#no_striping'])) {
    $row['#attributes']['class'][] = 'no-striping';
  }
  
  $output = '<tr'. drupal_attributes($row['#attributes']) .'>';
  
  // Render the cells
  foreach ($row as $name => $item) {
    if ($name[0] !== '#' && (empty($item['#type']) || $item['#type'] !== 'value')) {
      $cell_attr = isset($item['#cell_attributes']) ? $item['#cell_attributes'] : array();
      
      if ($name === $sort_col) {
        $cell_attr['class'][] = 'active'; 
      }
      $output .= '<td'. drupal_attributes($cell_attr).'>'. render($item) .'</td>';
    }
  }
  return $output ."</tr>";
}


// ==============================================
// Sort bins element
// ==============================================

/**
 * Element process callback to expand sort bins element
 * into a renderable and input elements.
 * 
 * @param $element
 *  The form element definition. Function will return
 *  an expanded version of this form element.
 */
function _iwapi_element_sort_bins_process($element) {
  $element['#tree'] = TRUE;
  $element['#attributes']['class'][] = 'sort-bin-wrapper';
  
 
  if (!empty($element['#sort_events'])) {
    $settings[$element['#id']] = $element['#sort_events'];
  }
  
  $settings[$element['#id']]['inactive_bin'] = (bool)$element['#inactive_bin'];
  
  if (!empty($settings)) {
    drupal_add_js(array('iwSortBins' => $settings), 'setting');
  }
  
  $weight = 0;
  $options = array();
  foreach ($element['bins'] as $key => &$info) {
    $options[$key] = (isset($info['#title'])) ? $info['#title'] : ucfirst($key);
    
    $info['#theme_wrappers'] = array('sort_bin_element');
    $info['#attributes']['class'][] = 'sort-bin';
 
    $info['value'] = array(
      '#type' => 'hidden',
      '#value' => $key,
      '#attributes' => array('class' => array('bin-value')),
    );
    
    if (empty($info['weight'])) {
      $info['weight'] = array(
        '#title' => t('Weight'),
        '#type' => 'textfield',
        '#size' => 5,
        '#default_value' => ++$weight,
        '#element_validate' => array('element_validate_integer'),
        '#attributes' => array('class' => array('bin-weight'))
      );
    }
  }
  
  $defaults = array(
    '#type' => 'select',
    '#field_prefix' => 'Group',
    '#options' => $options,
    '#attributes' => array('class' => array('parent-value')),
  );
  
  if (empty($element['#inactive_bin'])) {
    $defaults['#required'] = TRUE;
  }
  else {
    $defaults['#empty_option'] = t('- Inactive -');
  }
  
  // Shift through all the bin items to set the theme wrappers, and parent bins.
  foreach ($element['items'] as $key => &$info) {
    $info['#theme_wrappers'] = array('sort_bin_element');
    $info['#attributes']['class'][] = 'bin-item';
    
    $info['parent_bin'] = $defaults;
    $info['parent_bin']['#default_value'] = isset($info['#bin']) ? $info['#bin'] : ''; 

    $info['delta'] = array(
      '#type' => 'hidden',
      '#value' => $key,
      '#attributes' => array('class' => array('item-delta')),
    );
    unset($info['#bin']);
  }
  return $element;
}

/**
 * Value callback for sort_bins element. Creates a
 * results tree structure.
 * 
 * @param $element
 *  Element definition for the sort_bins.
 * @param $input
 *  If any form input has been processed for this element.
 * @param $form_state
 *  Current state of the whole form, if there is any.
 */
function _iwapi_element_sort_bin_value($element, $input = FALSE, $form_state = array()) {
  if ($input && is_array($input)) {
    $bins = $input['bins'];
    uasort($bins, 'drupal_sort_weight');
    
    foreach ($input['items'] as $key => $values) {
      if (!empty($values['parent_bin'])) {
        $bins[$values['parent_bin']]['children'][$key] = $values;
      }
    }    
    return array('tree' => $bins);
  }
  return isset($element['#default_value']) ? $element['#default_value'] : array(); 
}


// ==============================================
// Image Selector element
// ==============================================

/**
 * Form image_selector process function to expand the form element
 *  and setup JS variables.
 */
function _iwapi_element_image_selector_process($element) {
  require_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';

  // Set the base properties and values for the image selector.
  $ajax_id = $element['#id'] . '-ajax-wrapper';
  $element['#tree'] = TRUE;
  $element['#prefix'] = '<div id="' . $ajax_id . '" class="form-ajax-wrapper clearfix">';
  $element['#suffix'] = '</div>';
  $element['#value'] = max(0, $element['#value']);
  $element['#file'] = file_load($element['#value']);
  
  $js_settings['iwapi']['elements']['imageSelectors'][$element['#id']] = array(
    'extensions' => $element['#extensions'],
    'style' => $element['#image_style'],
  );
  
  $element['selector_value'] = array(
    '#type' => 'hidden',
    '#value' => empty($element['#file']) ? 0 : "[{$element['#file']->fid}] " . image_style_url('thumbnail', $element['#file']->uri),
    '#attributes' => array(
      'id' => $element['#id'],
      'class' => array('image-selector-value'),
    ),
  );
  
  // Determine what file types are allowed and look for files.
  $mimes = file_mimetype_mapping();
  $ext = array_fill_keys(explode(' ', $element['#extensions']), TRUE);
  $allowed = array_intersect_key($mimes['extensions'], $ext);
  $allowed = array_intersect_key($mimes['mimetypes'], array_flip($allowed));
  
  if (empty($allowed)) {
    $element['message'] = array(
      '#markup' => t('There are no acceptable file formats set. Please contact and administrator.'),
    );
    return $element;
  }
  
  // Determine the directory and retrieve valid files for options.
  $dir = trim($element['#directory'], ". \t\n\r\0\x0B");
  $scheme = file_uri_scheme($dir) ? '' : file_default_scheme() . '://';
  $element['#directory'] = $dir = preg_replace('#/$#', '', $scheme . $dir) . '/';

  // Search for permanent managed files in this directory.
  $query = db_select('file_managed', 'f')
    ->fields('f', array('fid', 'uri', 'filename'))
    ->condition('f.uri', db_like($dir) .'%', 'LIKE')
    ->condition('f.status', FILE_STATUS_PERMANENT)
    ->condition('f.filemime', $allowed)
    ->orderBy('f.filename', 'ASC');

  $opts = array();
  foreach ($query->execute() as $file) {
    $key = "[$file->fid] ". image_style_url('thumbnail', $file->uri);
    $opts[$key] = $file->filename;
  }
  if (!empty($opts)) {
    if (empty($element['#required'])) {
      $none_img = drupal_get_path('module', 'iwapi') .'/images/none.png';
      $none_style = image_style_path('thumbnail', $none_img);
       if (!file_exists($none_style)) {
        $style = image_style_load('thumbnail');
        $ret = image_style_create_derivative($style, $none_img, $none_style);
      }
      $key = '[0] '. image_style_url('thumbnail', $none_img);
      $opts = array($key => 'None') + $opts;
    }
    $element['file_select'] = array(
      '#type' => 'select',
      '#title' => 'Existing files:',
      '#options' => $opts,
    );
  }

  // add file upload
  $basename = implode('_', $element['#parents']);
  $element['upload_file'] = array(
    '#type' => 'file',
    '#name' => 'files[' . $basename . ']',
    '#prefix' => '<div><label>' . t('Upload new file:') . '</label>',
    '#size' => 24,
    '#theme_wrappers' => array(),
  );
  
  $element['upload_btn'] = array(
    '#name' => $basename . '_upload',
    '#type' => 'submit',
    '#value' => 'Upload',
    '#suffix' => '</div>',
    '#limit_validation_errors' => array($element['#parents']),
    '#validate' => array(),
    '#submit' => array('_iwapi_element_image_selector_submit'),
    '#ajax' => array(
      'callback' => '_iwapi_element_image_selector_callback',
      'wrapper' => $ajax_id,
      'effect' => 'fade',
    ),
  );
  
  // add upload progress bar if available. Disabled until I can figure out why there are AJAX parse errors.
  if ($element['#progress_indicator'] == 'bar' && $implementation = file_progress_implementation()) {
    $element['file_progress'] = array(
      '#type' => 'hidden',
      '#value' => mt_rand(),
      '#attributes' => array('class' => array('file-progress')),
    );

    // Add JavaScript settings to notify this widget the upload progress implementation to use.
    $js_settings['iwapi']['elements']['progressType'] = $implementation;
    $element['upload_btn']['#ajax']['progress'] = array(
      'type' => 'bar',
      'message' => t('Uploading image...'), 
      'path' => 'file/progress/' . $element['file_progress']['#value'],
    );
  }
  else {
    $element['upload_btn']['#ajax']['progress']['type'] = 'throbber';
  }
  // Add and finalize any settings for this file uploader
  drupal_add_js($js_settings, array('type' => 'setting'));
  return $element;
}

/**
 * Form image_selector element value callback function.
 */
function _iwapi_element_image_selector_value(&$element, $input = FALSE, $form_state = NULL) {
  if ($input) {
    // Check for a file upload matching our element
    $upload_name = implode('_', $element['#parents']);
    if (!empty($_FILES['files']['name'][$upload_name])) {
      $dest = trim($element['#directory'], ". \t\n\r\0\x0B");
      $scheme = file_uri_scheme($dest) ? '' : file_default_scheme() . '://';
      $dest = preg_replace('#/$#', '', $scheme . $dest) . '/';
      $validators = array(
        'file_validate_is_image'  => array(),
        'file_validate_extensions' => array($element['#extensions']),
      );
      
      if (file_prepare_directory($dest, FILE_CREATE_DIRECTORY) && ($file = file_save_upload($upload_name, $validators, $dest))) {
        db_update('file_managed')
          ->fields(array('status' => FILE_STATUS_PERMANENT))
          ->condition('fid', $file->fid)
          ->execute();

        return $file->fid;
      }
      else {
        watchdog('file', 'The file upload failed. %upload', array('%upload' => $upload_name));
        form_set_error($upload_name, t('The file in the !name field was unable to be uploaded.', array('!name' => $element['#title'])));
      };
    }
    else {
      if (!empty($input['file_select']) && preg_match('/^\[(\d+)\]/', $input['file_select'], $matches) > 0) {
        return intval($matches[1]); 
      }
    }
  }

  // Fall through default value if there was no input or input did not resolve to a file.
  $fid = 0;
  if (isset($element['#value']) && intval($element['#value']) > 0) {
    $fid = intval($element['#value']);
  }
  elseif (isset($element['#default_value']) && intval($element['#default_value']) > 0) {
    $fid = intval($element['#default_value']);
  }

  return $fid;
}

/**
 * Form image_selector element validate callback function.
 *  Checks if the element if required, and sets the form_state value of the element.
 */
function _iwapi_element_image_selector_validate(&$element, &$form_state) {
  if (!empty($element['#required']) && intval($element['#value']) <= 0) {
    form_error($element, t('The image %name is required, please make a selection or upload a new file.', array('%name' => $element['#title'])));
    return;
  }
  form_set_value($element, $element['#value'], $form_state);
}

/**
 * Image selector upload callback. Asks that the form is rebuilt.
 */
function _iwapi_element_image_selector_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Form image_selector upload AJAX callback.
 */
function _iwapi_element_image_selector_callback($form, &$form_state) {
  $parents = $form_state['triggering_element']['#parents'];
  array_pop($parents);
  
  $output = theme('status_messages');
  if (!empty($parents)) {
    $element = &$form;
    while ($parent = array_shift($parents)) {
      if (!isset($element[$parent])) {
        return array(
          '#type' => 'ajax',
          '#commands' => array(ajax_command_replace(NULL, $output)),
        );
      }
      $element = &$element[$parent];
    } 
    
    // Include the updated form element and the JS settings.
    $js = drupal_add_js();
    $output .= drupal_render($element);
    $settings = drupal_array_merge_deep_array($js['settings']['data']);

    return array(
      '#type' => 'ajax',
      '#commands' => array(ajax_command_replace(NULL, $output, $settings)),
    );
  }
}


// ==================================
// Misc. utilities and callbacks.
// ==================================

/**
 * Prevents a child element from overwriting values of its
 * parent element, but reporting its value location as
 * the same as its parent's. See _form_builder_handle_input_element()'s
 * last 3 lines to see why this is relevent.
 * 
 * @param array $element
 *  Reference to the element to find the value for.
 * @param string $input
 *  The current input for the element, if there is any.
 * @return array | string | NULL
 *  Returns the current value of this element, as should
 *  be set in the '#value' index of the element.
 * 
 * @see _form_builder_handle_input_element().
 */
function _iwapi_prevent_child_value(array &$element, $input = FALSE) {
  if ($input !== FALSE) {
    array_pop($element['#parents']);
  }
  
  // Get the form element value pushed.
  $callback = 'form_type_'. $element['#type'] .'_value';
  return function_exists($callback) ? form_type_textfield_value($element, $input) : NULL;
}

/**
 * Element validate callback to ensure that a text
 * field contains an internal URL.
 * 
 * @param array $element
 *  The full element definition, with the '#value'
 *  containing the current element value.
 * @param array $form_state
 *  Reference to the current state / values of the form.
 * @param array $form
 *  The form structure, and element definitions.
 */
function iwapi_element_validate_internal_uri(array $element, array &$form_state, array $form) {
  $path = drupal_get_normal_path($element['#value']);
  
  if (!empty($path)) {
    if (url_is_external($path)) {
      form_error($element, t('Path cannot be external. Please use a relative link.'));
    }
    else {
      $url = parse_url($path);
      $path = $url['path'];
  
      if (!trim($path) || !drupal_valid_path($path, TRUE)) {
        form_error($element, t("The path '@link' is either invalid or you do not have access to it.", array('@link' => $path)));
      }
      else {
        form_set_value($element, trim($path), $form_state);
      }
    }
  }
}

/**
 * Element validation callback, to load and validate a textfield
 * element to contain CSS classes.
 * 
 * @param array $element
 *  Array containing the element definition.
 * @param array $form_state
 *  Reference to the current form state.
 * @param array $form
 *  The full form definition.
 */
function iwapi_element_validate_css_classes($element, &$form_state, $form) {
  $classes = drupal_array_get_nested_value($form_state['values'], $element['#parents']);
  if (preg_match_all('/(?<=^| )([a-z\-_][\w\-_]*)(?: +|$)/i', $classes, $matches) > 0) {
    form_set_value($element, $matches[1], $form_state);
  }
  else {
    form_set_value($element, array(), $form_state);
  }
}
