<?php
/**
 * @file
 * Contains the core theme preprocess, process and functions for iwapi module.
 */


// =================================
// Theme functions
// =================================

/**
 * Implements THEME_popup_help().
 */
function theme_popup_help($vars) {
  $element = $vars['element'];

  $output = '';
  if (!empty($element['#markup'])) {
    $output = '<a class="help-tooltip-label" href="#">'. $element['#tooltip_label'] .'</a>';
    $output .= '<div class="help-content">'. $element['#markup'] .'</div>';
  }
  return $output;
}

/**
 * Implements HOOK_sort_bin_element().
 */
function theme_sort_bin_element($vars) {
  $element = &$vars['element'];

  if (!empty($element['weight'])) {
    $element['#attributes']['class'][] = 'sortable';
  }
  $element['#attributes']['class'][] = 'clearfix';
  
  $output = '<div'. drupal_attributes($element['#attributes']) .'>';
  $output .= '<div class="handle-title">';
  if (!empty($element['#title'])) {
    $output .= t($element['#title']);
  }
  $output .= '</div>';

  // Append child elements and close the containing div.
  return $output . $element['#children'] .'</div>';
}

/**
 * Implements THEME_table_elements().
 */
function theme_table_elements(array $vars) {
  $table      = $vars['table'];
  $attributes = isset($table['#attributes']) ? $table['#attributes'] : array();
  
  $header = '';
  
  // Format the table header:
  if (!empty($table['#header'])) {
    $ts = tablesort_init($table['#header']);
    
    // Render the header cells
    foreach ($table['#header'] as $cell) {      
      $cell = tablesort_header($cell, $table['#header'], $ts);
      $header .= _theme_table_cell($cell, TRUE);
    }
    
    // Add sticky headers, if applicable.
    if (!isset($table['#sticky']) || $table['#sticky']) {
      drupal_add_js('misc/tableheader.js');
      $attributes['class'][] = 'sticky-enabled';
    }
  }
  else {
    $ts = array();
    $table['#header'] = array();
  }
  
  $rows  = '';
  $count = 0;
  $sort_col = _iwapi_get_header_sort_col($table['#header'], $ts);
  
  // Render all the available rows
  foreach ($table as $key => $row) {
    if ($key[0] !== '#') {
      $row['#attributes']['class'][] = (++$count & 1) ? 'even':'odd';
      $rows .= _iwapi_render_element_row($key, $row, $table, $sort_col);
    }
  }
  
  // Handle the case of empty data.
  if (empty($rows) && !empty($table['#empty'])) {
    $count = 0;
    foreach ($table['#header'] as $cell) {
      $count += isset($cell['colspan']) ? $cell['colspan'] : 1;
    }
    $rows = '<tr><td colspan="'. max($count, 1) .'" class="empty message">'.$table['#empty'] ."</td></tr>\n";
  }
  
  // Finally put it all together and send it off.
  $html = empty($table['#title']) ? '' : '<label>' . check_plain($table['#title']) . "</label>\n";
  $html .= '<table' . drupal_attributes($attributes) . ">\n";

  $html .= empty($rows) ? "<tr>{$header}</tr>\n"
    : "<thead><tr>{$header}</tr></thead>\n<tbody>\n{$rows}</tbody>\n";
  
  $html .= "</table>\n";
  
  // Add the description if there was one.
  if (!empty($table['#description'])) {
    $html .= '<div class="description">'. $table['#description'] ."</div>\n";
  }
  
  return '<div class="form-item form-type-table">'. $html .'</div>';
}


// =================================
// preprocess functions
// =================================

/**
 * Implements TEMPLATE_PREPROCESS_iwentity().
 */
function template_preprocess_iwentity(&$vars) {
  $content = $vars['elements'];
  $entity  = $content['#entity'];
  $type    = $entity->entityType();
  
  $vars['entity_type'] = $type;
  $vars['bundle'] = $entity->getBundle();
  
  // Add the general variables
  $entity->preprocess($content, $vars);
  
  $vars['attributes_array']['class'][] = strtr("$vars[bundle]", '_', '-');
  
  // Add some common CSS classes for entities.
  $vars['title_attributes_array']['class'][] = 'entity-title';
  
  $vars['content_attributes_array']['class'][] = 'content';
  $vars['content_attributes_array']['class'][] = 'clearfix';

  // Add some theme suggestions to allow customization by entity type view mode or bundle.
  $vars['theme_hook_suggestions'][] = 'iwentity__'. $type;
  $vars['theme_hook_suggestions'][] = 'iwentity__'. $type .'__'. $content['#view_mode'];
  $vars['theme_hook_suggestions'][] = 'iwentity__'. $type .'__'. $entity->getBundle();
}

/**
 * Implements TEMPLATE_preprocess_package_summary().
 */
function theme_package_summary($vars) {
  $html = '<div class="package-summary clearfix">';
  
  foreach ($vars['properties'] as $label => $value) {
    $html .= '<div class="property-row">';
    $html .= '<span class="property-label">'. t($label) .':</span> ';
    $html .= '<em class="property-value">'. render($value) .'</em>';
    $html .= '</div>';
  }
  $summary = &$vars['package'];
  
  return $html .'</div>';
}