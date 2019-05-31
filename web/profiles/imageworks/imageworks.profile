<?php
/**
 * @file
 * Enables modules and site configuration for a minimal site installation.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function imageworks_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
}


/**
 * Implements HOOK_install_tasks().
 */
function imageworks_install_tasks() {
  return array(
    'content_filtering' => array(
      'display_name' => st('Content Filtering'),
    ),
  );
}
