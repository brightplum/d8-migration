<?php

/**
 * Implements hook_preprocess_page().
 */
function omega_dreams_preprocess_page(&$variables) {
  // Load the header js but not on the imce page.
  if (arg(0) != 'imce' && arg(0) != 'donate') {
    $theme_path = path_to_theme('omega_dreams');
    drupal_add_js($theme_path . '/js/waypoints/jquery.waypoints.min.js');
    drupal_add_js($theme_path . '/js/header.behaviors.js');
  }
}
