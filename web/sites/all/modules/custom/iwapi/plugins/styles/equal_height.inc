<?php

/**
 * @file
 * Definition of the 'default' panel style.
 */

// Plugin definition
$plugin = array(
  'title' => t('Equal Height'),
  'description' => t('Renders a panel to be equal with its siblings.'),
  'render region' => 'panel_equal_heights_style_render_region',
);

/**
 * Render callback.
 *
 * @ingroup themeable
 */
function theme_panel_equal_heights_style_render_region($vars) {
  static $inc_js;
  
  if (!isset($inc_js)) {
    $inc_js = TRUE;
    drupal_add_js(drupal_get_path('module', 'iwapi') .'/js/equal-heights.js');
  }
  
  $output = '<div class="region equal-heights region-' . $vars['region_id'] . '">';
  $output .= implode('<div class="panel-separator"></div>', $vars['panes']);
  $output .= '</div>';
  return $output;
}

