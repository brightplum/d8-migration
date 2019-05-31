<?php
/**
 * @file includes/ajax.inc
 * Includes the ajax commands and utilities for the iwapi module.
 */

/**
 * Create a AJAX command which should tell JS to reload the page.
 * 
 * @return array
 *  AJAX command indicating that the page should be reloaded.
 */
function iwapi_ajax_command_reload() {
  return array('command' => 'page_reload');
}

/**
 * Create a AJAX command which should tell JS to redirect the
 * webpage to another location.
 * 
 * @param $url
 *  URL that the AJAX command should request that we redirect to.
 *  URL can either be a string or a URI array with keys "path" and
 *  "options" and should only contain a relative path.
 * @param $delay
 *  Amount of time in seconds to delay the redirect command.
 * @return array
 *  AJAX command indicating that the page should redirect.
 */
function iwapi_ajax_command_redirect($raw, $delay = 0) {
  if (!is_array($raw)) {
    $uri = url_is_external($raw) ? array('path' => '<front>') : drupal_parse_url($raw);
  }
  else {
    $uri['path'] = empty($raw['path']) ? '<front>' : $raw['path'];
    if (!empty($raw['options']) && is_array($raw['options'])) {
      $uri += $raw['options'];
    }
  }
  $uri['absolute'] = TRUE;

  return array(
    'command' => 'url_redirect',
    'url' => url($uri['path'], $uri),
    'delay' => intval($delay),
  );
}

/**
 * Generate a new AJAX command which will either trigger
 * or populate a ui.dialog with contents.
 * 
 * @param $title
 *  Dialog title that should be displayed in the dialog header.
 *  An empty or NULL value means to keep the current title.
 * @param $content
 *  String or renderable array with the dialog contents.
 * @param array $settings
 *  The settings that will get passed to the ui.Dialog and tell
 *  it how to display, appear.
 * @param array
 *  AJAX command to display a modual dialog, and the content,
 *  title and settings that should be used to display.
 */
function iwapi_ajax_command_dialog($title, $content, array $settings = array()) {
  // Transform render arrays into regular HTML for display.
  $content = is_array($content) ? render($content) : $content;
  if (!empty($title)) {
    $settings['title'] = $title;
  }
  
   // Return the AJAX command with the dialog settings.
  return array(
    'command'  => 'dialog_modal',
    'content'  => $content,
    'settings' => $settings,
  );
}

/**
 * Generate an AJAX command to request that a
 * dialog just close, if there is one open.
 * 
 * @return array
 *  AJAX command to request that a dialog is dismissed.
 */
function iwapi_ajax_command_dismiss_dialog() {
  return array('command' => 'dismiss_dialog');
}

/**
 * Generate an AJAX command push a 
 * 
 * @return array
 *  AJAX command to push an update element table row.
 */
function iwapi_ajax_command_update_tablerow($table, $row_id) {
  if (isset($table[$row_id])) { 
    $row = $table[$row_id];
    $ts  = !empty($table['#header']) ? array() : tablesort_init($table['#header']);  
    
    return array(
      'command' => 'update_tablerow',
      'selector' => '#'. $table['#id'] .'-row-'. $row_id,
      'data' => _iwapi_render_element_row($row_id, $row, $table, $ts),
    );
  }
}

/**
 * Generate an AJAX command to request to delete
 * the parent row from the table. Called from a dialog,
 * it will delete the table from the row that triggered
 * the dialog, otherwise it'll work with the AJAX element.
 * 
 * @return array
 *  AJAX command to request that a table row gets removed.
 */
function iwapi_ajax_command_delete_tablerow() {
  return array('command' => 'delete_tablerow');
}


// ==============================
// Form AJAX functionality
// ==============================

/**
 * Generate a renderable version of the form that
 * can be used in AJAX or IFrame.
 *
 * @param \string $form_id
 *  The ID of the form to retrieve.
 * @param array $form_state
 *  The requested state of the form
 * @return array
 *  Return a renderable array of the form contents
 */
function _iwapi_ajax_get_form($form_id, array &$form_state) {
  // Add the necessary form state values for an AJAX request.
  $form_state += array(
    'ajax' => TRUE,
    're_render' => FALSE,
    'no_redirect' => TRUE,
    'build_info' => array(),
  );

  // If no arguments were supplied already, check for them now.
  if (!isset($form_state['build_info']['args'])) {
    $args = func_get_args();
    array_shift($args);  // shift out the $form_id.
    array_shift($args);  // shift out the $form_state.
    $form_state['build_info']['args'] = $args;
  }
  return drupal_build_form($form_id, $form_state);
}

/**
 * Determines if the form should be displayed, or if it has been
 * executed already, what commands if any should be run as a result.
 * 
 * @param $form_state
 *  Reference to the current $form_state variable. This should
 *  have already gone through drupal_build_form(), and contain
 *  the form processed values.
 * @param $action
 *  String name of the action which should be preformed as the
 *  default AJAX action when the form is submitted. This action
 *  occurs when there is not redirect or destination. It is also
 *  prefixed by "iwapi_ajax_command_{$action}" to for the name
 *  of the function to format the resulting AJAX command. 
 * @return <mixed>
 *  Returns an array of AJAX commands based on the the result of the form.
 *  FALSE, if the form should be reposted and no new command is present.
 */
function iwapi_ajax_form_complete(array &$form_state, $action = NULL, $suppress_redirect = FALSE) {
  if (!empty($form_state['executed']) && empty($form_state['rebuild'])) {
    // Redirects are handled and checked first.
    if (!$suppress_redirect) {
      if (isset($_GET['destination'])) {
        return iwapi_ajax_command_redirect($_GET['destination']);
      }
      elseif (!empty($form_state['redirect'])) {
        if (is_array($form_state['redirect'])) {
          $uri = array_combine(array('path', 'options'), $form_state['redirect']);
          return iwapi_ajax_command_redirect($uri);
        }
        return iwapi_ajax_command_redirect($form_state['redirect']);
      }
    }
    
    // If the preference is to reload the page, this will handle displaying
    // status messages as well, so we can simply do a page reload.
    if (isset($action) && function_exists('iwapi_ajax_command_'. $action)) {
      $callback = 'iwapi_ajax_command_'. $action;
      return $callback();
    }

    // Otherwise, if there are messages display them, or close dialog if no messages.
    return iwapi_ajax_command_dismiss_dialog();
  }
  return FALSE;
}


// =================================
// Reusable AJAX callbacks
// =================================

/**
 * Form callback which can execute a single AJAX 
 * parameterless command. Prevents the need for
 * simple wrappers of very simple AJAX calls.
 */
function _iwapi_ajax_single_command_callback($form, $form_state) {
  $trigger = $form_state['triggering_element'];
  $command = $trigger['#ajax']['ref_command'];

  if (function_exists($command)) {
    return array('#type' => 'ajax', '#commands' => array($command()));
  }
}


/**
 * Regenerate the row which contains the triggering
 * element. 
 * 
 * @param array $form
 *  The structure and renderable elements of the form.
 * @param array $form_state
 *  The current status and values of the form.
 * @return array
 *  Either an array of AJAX commands, or NULL if
 *  no table or proper table element could be found.
 */
function _iwapi_ajax_refresh_row_callback(array $form, array $form_state) {
  $table    = NULL;
  $ref_item = &$form;
  $trigger  = $form_state['triggering_element'];
  $table_types = array('table_orderable', 'table_elements');
  
  // We will drilldown through the trigger array parents, for the closest
  // table type. Once found, it can be used to regenerate the table values.
  $current = &$form;
  foreach ($trigger['#array_parents'] as $index => $name) {
    if (isset($current[$name])) {
      if (!empty($current[$name]['#type']) && in_array($current[$name]['#type'], $table_types)) {
        $table = $current[$name];
        $row_name = $trigger['#array_parents'][$index + 1];
      }
    }
 
    $current = &$current[$name]; // go to the next table item.
  }

  // If we found an available table and row name, then we can rebuild the row.
  if (isset($table) && !empty($row_name)) {
    return array(
      '#type' => 'ajax', 
      '#commands' => array(iwapi_ajax_command_update_tablerow($table, $row_name)),
    );
  }
}

/**
 * Using the #ajax properties array, we add an additional field
 * to indicate which form element needs to get returned, or
 * what commands should trigger. This will only run the values
 * found in the $form_state['triggering_element'].
 * 
 * @param $form
 *  Current form structure, with form elements.
 * @param $form_state
 *  The submitted status of the form.
 * @return array
 *  AJAX commands or form element to be processed by AJAX.
 */
function _iwapi_ajax_configurable_callback($form, $form_state) {
  $trigger = $form_state['triggering_element'];
  $target  = $trigger['#ajax']['ref_element'];

  // Clear the invalid selection error caused by the swapping of options.
  drupal_get_messages('error');
  form_clear_error();
  
  $content = _iwapi_resolve_element_ref($trigger, $target, $form);
  return $content;
}

/**
 * Determine the path to the render element by
 * checking the target reference type, and value.
 * 
 * @param array $element
 *  The element to reference from.
 * @param array $target
 *  Information about the target and how to find
 *  the items that need to get returned.
 * @param array $form
 *  The form from which to pull the elements from.
 * @return array | null
 *  Either a renderable array of the form element
 *  to render for a single target, an AJAX command
 *  array for multiple targets, or NULL of the 
 *  correct targets could not be resolved.
 */
function _iwapi_resolve_element_ref(array $element, array $target, array &$form) {
  // Resolve the path based on the target type.
  switch ($target['type']) {
    case 'multiple':
      $commands = array();

      foreach ($target['value'] as $wrapper => $value) {
        $item = _iwapi_resolve_element_ref($element, $value, $form);
        if (!empty($item)) {
          $commands[] = ajax_command_replace('#' . $wrapper, render($item));
        }
      }
      $commands[] = ajax_command_append('body', theme('status_messages', array()));
      return array('#type' => 'ajax', '#commands' => $commands);
      
    case 'root':
      $parents = $target['value'];
      break;
  
    case 'parent':
      $parents = $element['#array_parents'];
      array_pop($parents);
      break;
 
    case 'child':
      $parents = array_merge($element['#array_parents'], $target['value']);
      break;
  
    case 'sibling':
      $parents = $element['#array_parents'];
      array_pop($parents);

      if ($element['#type'] == 'radio') {
        array_pop($parents);
      }

      $parents = array_merge($parents, $target['value']);
      break;
  }
  
  // If a path to the form elements was found, try to
  // return the values / elements found at that location.
  if (!empty($parents)) {
    return array(drupal_array_get_nested_value($form, $parents));
  }
}
