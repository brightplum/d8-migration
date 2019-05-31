<?php
/**
 * @file iwlogin.pages.inc
 * Different ways of loading and responding to the login form in a popup box.
 */

/**
 * Return an ajax ready version of the login form.
 * 
 * @return array
 *  AJAX commands in array form.
 *  
 * @see ajax_deliver().
 * @see ajax_base_page_theme()
 */
function _iwlogin_ajax_form() {
  $state = array();
  
  iwapi_include('ajax');
  $form  = _iwapi_ajax_get_form('user_login_block', $state);

  // Check if the form submitted properly, if so redirect or reload.
  if (!($command = iwapi_ajax_form_complete($state, 'reload'))) {
    $form['#action'] = url(current_path(), array('query' => drupal_get_query_parameters()));
  
    $content = array(
      'messages' => array('#markup' => theme('status_messages')),
      'form' => $form,
    );

    // Get configurations and finally build the form.
    $config = isset($form['#iwapi_dialog']) ? $form['#iwapi_dialog'] : array();
    $command = iwapi_ajax_command_dialog('Login', $content, $config);
  }
  
  return array('#type' => 'ajax', '#commands' => array($command));
}

/**
 * Page callback to render the login dialog for use inside an iFrame.
 * 
 * @return string
 *  Rendered HTML content page. drupal_goto() might also get
 *  called to just reload the page as is if user is already logged.
 */
function _iwlogin_iframe_login_page() {
  global $is_https;
  
  if ($is_https && variable_get('https', FALSE) && user_is_logged_in()) {
    global $user;
    
    // When mixed mode is not allowed and logged in without HTTPS the SSID and
    // SID match. This means the session is really invalid and should be reset.
    if ($user->ssid == $user->sid) {
      session_destroy();
    }
  }

  iwapi_include('ajax');
  $state = array();
  $form  = _iwapi_ajax_get_form('user_login_block', $state);
  $form['#action'] = url('iwlogin/iframe/ajax', array('query' => drupal_get_query_parameters()));
  
  if (user_is_anonymous()) {
    drupal_add_library('system', 'jquery.form');
    drupal_add_library('iwapi', 'iwapi.core');
    drupal_add_library('iwapi', 'iwapi.ajax');
    drupal_add_js(drupal_get_path('module', 'iwlogin') .'/js/frame_login.js');
    
    // Render page content for display.
    $content['page'] = array(
      'page_top' => array(),
      'page_buttom' => array(),
      '#children' => '<div id="iwlogin-container">'. drupal_render($form) .'</div>',
    );
      
    // Render page content for display.
    print theme('html', $content);
    drupal_exit();
  }
  else {
    drupal_goto();
  }
}

/**
 * AJAX callback meant to populate an iFrame which is
 * already built and just needs to be re-processed.
 * 
 * @see ajax_deliver().
 * @see ajax_base_page_theme()
 */
function _iwlogin_iframe_login_ajax() {
  $state = array();

  iwapi_include('ajax');
  $form  = _iwapi_ajax_get_form('user_login_block', $state);
  
  // Override the redirect specified normal form execution.
  if (variable_get(IWLOGIN_VAR_LOGIN_REDIRECT, FALSE) && empty($state['redirect'])) {
    // Because we are in an iFrame, simply reload won't work so we need to
    // initiate a redirect, with the origin URL or hit the frontpage.
    $state['redirect'] = empty($_GET['origin']) ? '<front>' : $_GET['origin'];
  }

  if (!($command = iwapi_ajax_form_complete($state))) {
    // Relay the original destination back to redirect on success.
    $form['#action'] = url('iwlogin/iframe/ajax', array('query' => drupal_get_query_parameters()));
    
    // Rebuild the form with status messages, and proper wrapper.
    $content = array(
      'messages' => array('#markup' => theme('status_messages')),
      'form' => $form,
    );
    $command = ajax_command_replace('#iwlogin-container > *', drupal_render($content));
  }
  return array('#type' => 'ajax', '#commands' => array($command));
}
