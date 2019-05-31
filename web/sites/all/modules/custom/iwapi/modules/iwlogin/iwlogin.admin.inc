<?php
/**
 * @file iwlogin.admin.inc
 * Contains the administration and settings forms for the iwlogin module.
 */

/**
 * Generate the settings form which allows us to set the login
 * prompt, welcome text and login form behavior. 
 * 
 * @see _iwlogin_admin_settings_form_validate();
 * @see _iwlogin_admin_settings_form_submit();
 */
function _iwlogin_admin_settings_form($form, &$form_state) {
  $login_text = variable_get(IWLOGIN_VAR_LOGIN_TEXT, array()) + array(
    'signin' => t('Sign In'),
    'site_text' => t('to [site:name] or'),
    'register' => t('Create an Account'),
  );

  // Configure the text used for the login prompts, login destination and if login
  // form should display links for password and registration.
  $form[IWLOGIN_VAR_LOGIN_TEXT] = array(
    '#type' => 'fieldset',
    '#title' => t('Login Verbage'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,

    'signin' => array(
      '#type' => 'textfield',
      '#title' => t('Login text'),
      '#description' => t('The text used for the sign in link.'),
      '#default_value' => $login_text['signin'],
    ),
    'site_text' => array(
      '#type' => 'textfield',
      '#title' => t('Site text'),
      '#description' => t('Text appearing between the login and register link. Usually naming the site.'),
      '#default_value' => $login_text['site_text'],
    ),
    'register' => array(
      '#type' => 'textfield',
      '#title' => t('Register text'),
      '#description' => t('Text used for the registration link.'),
      '#default_value' => $login_text['register'],
    ), 
  );
  
  $form['redirect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Destination after login'),
    '#tree' => FALSE,
    
    IWLOGIN_VAR_LOGIN_REDIRECT => array(
      '#type' => 'checkbox',
      '#title' => t('Redirect successful logins to destination below.'),
      '#default_value' => variable_get(IWLOGIN_VAR_LOGIN_REDIRECT, FALSE),
      '#description' => t(
        'Drupal will normally redirect to the user profile page, this will override that behavior'.
        'or any other redirects set for the user_login form. If left blank the <em>login '.
        'destination</em> value will be ignored, and successful logins will use default Drupal routing.'
      ),
    ),
    
    IWLOGIN_VAR_DESTINATION => array(
      '#type' => 'textfield',
      '#title' => t('Login Destination'),
      '#description' => t(
        'Destination to use after a successful login. Keep in mind this is only '.
        'used as a default (if there is no destination attached to the login '.
        'link). Leave this field blank if user should be returned to page '.
        'source page. The URL should not contain any query or anchors.'
      ),
      '#default_value' => variable_get(IWLOGIN_VAR_DESTINATION, ''),
    ),
  );
  
  $form[IWLOGIN_VAR_FORCE_HTTPS] = array(
    '#type' => 'checkbox',
    '#title' => t(
      'Ensure that logins, registrations and password recovery forms use SSL connections. '.
      'Note: For mixed mode HTTPS sessions, remember to set $conf[https] = TRUE in the settings.php file'
    ),
    '#default_value' => variable_get(IWLOGIN_VAR_FORCE_HTTPS, FALSE),
  );

  $form[IWLOGIN_VAR_ADDL_LOGOUT] = array(
    '#type' => 'checkbox',
    '#title' => t('Include an additional logout link with the welcome message.'),
    '#default_value' => variable_get(IWLOGIN_VAR_ADDL_LOGOUT, TRUE),
  );
  
  return system_settings_form($form);
}

/**
 * Form validation callback for iwlogin_admin_settings_form. Used to
 * validate the login behavior fields before saving.
 */
function _iwlogin_admin_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  if (!empty($values[IWLOGIN_VAR_DESTINATION])) {
    $dest = drupal_get_normal_path($values[IWLOGIN_VAR_DESTINATION]);
    $element = $form['redirect'][IWLOGIN_VAR_DESTINATION];

    // Prevent improper use of this login URL value.
    if (url_is_external($dest)) {
      form_error($element, t('The login destination cannot be an external URL.'));
    }
    else {
      $parts = parse_url($dest);
      if (!empty($parts['query']) || !empty($parts['fragment'])) {
        drupal_set_message(t('The query and/or anchor component of the URL has been removed.'), 'warning');
      }
      // Ensure that we are only using the path component of the URL.
      form_set_value($element, $parts['path'], $form_state);
    }
  }
}


// =========================================
// User logged in links and verbage
// =========================================

/**
 * Form builder; callback to define the configuration form
 * which controls how additional user links are displayed.
 */
 function _iwlogin_user_links_form($form, &$form_state) {  
  $form['user_links'] = array(
    '#type' => 'table_orderable',
    '#prefix' => '<div id="authenticated-user-links-table">',
    '#suffix' => '</div>',
    '#header' => array(t('Text'), t('Path'), t('Actions')),
    '#empty' => t('No additional authenticated user links have been specified.'),
  );

  $form['update_btn'] = array(
    '#type' => 'submit',
    '#value' => t('Update Changes'),
    '#validate' => array(),
    '#submit' => array('_iwlogin_admin_update_user_links_submit'),
  );
  $form['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('user'),
  );
  
  // Manage the list of links that should appear for authenticated users.
  $links = variable_get(IWLOGIN_VAR_USER_LINKS, array());
  $links += array('logout' => array('text' => 'Sign out', 'path' => 'user/logout'));
  
  foreach ($links as $idx => $link) {
    if ($idx === 'logout') {
      $form['user_links'][$idx] = array(
        'text' => array(
          '#type' => 'textfield',
          '#default_value' => $link['text']
        ),
        'path' => array('#markup' => 'user/logout'),
        'remove' => array('#markup' => '<div class="empty"></div>'),
      );
    }
    else {
      $form['user_links'][$idx] = array(
        'text' => array('#markup' => check_plain($link['text'])),
        'path' => array('#markup' => check_plain($link['path'])),
        'remove' => array(
          '#type' => 'submit',
          '#name' => "addl_link_{$idx}",
          '#value' => t('Remove'),
          '#validate' => array(),
          '#submit' => array('_iwlogin_admin_remove_user_link_submit'),
          '#limit_validation_errors' => array(),
        ),
      );
    }
  }
  
  $form['user_links']['add_user_link'] = array(
    'text' => array(
      '#type' => 'textfield',
      '#prefix' => '<label>'. t('Add new link') .'</label>',
      '#attributes' => array('placeholder' => t('Link text')),
    ),
    'path' => array(
      '#type' => 'textfield',
      '#prefix' => '<div class="row-label-placeholder">&nbsp;</div>',
      '#attributes' => array('placeholder' => t('Link URL')),
    ),
    'add_link_btn' => array(
      '#type'     => 'submit',
      '#prefix'   => '<div class="row-label-placeholder">&nbsp;</div>',
      '#value'    => t('Add New Link'),
      '#submit'   => array('_iwlogin_admin_add_user_link_submit'),
      '#validate' => array('_iwlogin_admin_add_user_link_validate'),
    ),
  );
  return $form;
}

/**
 * _iwlogin_admin_settings_form validation callback function.
 * 
 * @see _iwlogin_admin_settings_form_submit()
 */
function _iwlogin_admin_add_user_link_validate($form, &$form_state) {
  $values = $form_state['values']['user_links']['add_user_link'];
  
  // Make sure that there is some title text specified.
  if (empty($values['text'])) {
    form_set_error('user_links', t('Label cannot be empty.'));
  }
  
  // Make sure the specified link URL is a valid one.
  if (empty($values['path']) || !parse_url($values['path'])) {
    form_set_error('user_links][add_user_link][path', t('Please specify a valid url.'));
  }
}


// =========================================
// Form submit callbacks
// =========================================

/**
 * Form submit callback for _iwlogin_admin_settings_form().
 * - This function is used to remove user links.
 */
function _iwlogin_admin_remove_user_link_submit(&$form, &$form_state) {
  // Determine if we are removing a particular link, and then figure out which one.
  $button = $form_state['clicked_button'];
  if ($button['#value'] === t('Remove')) {
    $links = variable_get(IWLOGIN_VAR_USER_LINKS, array());
    array_pop($button['#parents']);
    unset($links[array_pop($button['#parents'])]);
    variable_set(IWLOGIN_VAR_USER_LINKS, array_filter($links));
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Form submit callback for _iwlogin_admin_settings_form().
 * - This function is used to add new user links.
 */
function _iwlogin_admin_add_user_link_submit($form, &$form_state) {
  $values = &$form_state['values']['user_links'];

  // Parse any URL into constituent parts for better storage.
  if ($url = parse_url($values['add_user_link']['path'])) {
    $old = variable_get(IWLOGIN_VAR_USER_LINKS, array());
    if (!empty($url['query'])) {
      $query = $url['query'];
      parse_str($query, $url['query']);
    }
    
    $links = array();
    $link = array('text' => $values['add_user_link']['text']) + $url;
    foreach($values as $delta => $value) {
      $links[] = ($delta === 'add_user_link') ? $link : $old[$delta];
    }  
    variable_set(IWLOGIN_VAR_USER_LINKS, $links);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit callback for _iwlogin_admin_settings_form().
 * This function is used to add new user links.
 */
function _iwlogin_admin_update_user_links_submit($form, &$form_state) {
  $values = &$form_state['values']['user_links'];
  unset($values['add_user_link']);
    
  // Rebuild the links in the order they appeared.
  $old = variable_get(IWLOGIN_VAR_USER_LINKS, array());
  $old['logout']['text'] = $values['logout']['text'];
  $old['logout']['path'] = 'user/logout';
  
  $links = array();
  foreach ($values as $index => $value) {
    if ($index !== 'logout') {
      $links[] = $old[$index];
    }
    else {
      $links['logout'] = $old['logout'];
    }
  }
  variable_set(IWLOGIN_VAR_USER_LINKS, $links);
  $form_state['rebuild'] = TRUE;
}


// ===============================
// User tab labels
// ===============================

/**
 * Form builder to generate the form which allows
 * altering of user profile tab labels.
 */
function iwlogin_user_tab_label_form($form, &$form_state) {
  $labels = variable_get(IWLOGIN_VAR_USER_TABS, array()) + array(
    'edit' => '',
    'view' => '',
  );
  
  $form[IWLOGIN_VAR_USER_TABS] = array(
    '#type' => 'fieldset',
    '#title' => t('User profile tab labels'),
    '#tree' => TRUE,
    '#description' => t(
      'Leaving any of the label fields blank, will keep the current '.
      'default values and not attempt to alter the labels at all.'
    ),
    
    'view' => array(
      '#type' => 'textfield',
      '#title' => t('View tab label'),
      '#default_value' => $labels['view'],
    ),
    'edit' => array(
      '#type' => 'textfield',
      '#title' => t('Edit tab label'),
      '#default_value' => $labels['edit'],
    ),
  );
  
  $form['tokens'] = array(
    '#theme' => 'token_tree',
    '#dialog' => TRUE,
    '#token_types' => array('user'),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save Changes'),
    ),
  );

  return $form;
}

/**
 * 
 */
function iwlogin_user_tab_label_form_submit($form, &$form_state) {
  $rebuild = FALSE;
  $values  = $form_state['values'][IWLOGIN_VAR_USER_TABS];
  
  foreach ($values as $tab => $label) {
    if (!isset($default[$tab]) || $default[$tab] !== $label) {
      $rebuild = TRUE;
      break;
    }
  }
  
  variable_set(IWLOGIN_VAR_USER_TABS, $values);
  $form_state['redirect'] = 'user';
  
  if ($rebuild) {
    menu_rebuild();
  }
}