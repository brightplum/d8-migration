<?php
/**
 * @file theme/theme.inc
 * Theme functions and preprocessors for the menu_builder module.
 */

/**
 * Implements THEME_menu_builder_column(). 
 */
function theme_menu_builder_column($vars) {
  $element = $vars['element'];
  
  $element['#attributes']['class'][] = 'menu-column';
  $attributes = drupal_attributes($element['#attributes']);
  
  return "<div{$attributes}>{$element['#children']}</div>";
}
