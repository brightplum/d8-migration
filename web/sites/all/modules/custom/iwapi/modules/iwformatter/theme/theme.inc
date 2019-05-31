<?php
/**
 * @file theme/theme.inc
 * Contains the process and preprocess functions relating Drupal theming.
 */


// ================================
// Field formatters
// ================================

/**
 * Implements TEMPLATE_preprocess_iwformatter_field_page().
 */
function template_preprocess_iwformatter_field_page(&$vars) {
  $element = &$vars['field_data'];
  
  $entity_title = $element['#entity_title'];
  $uri = entity_uri($element['#entity_type'], $element['#entity']);
  
  $vars['title_suffix']['return_link'] = array(
    '#type' => 'link',
    '#title' => "View {$entity_title} page",
    '#href' => $uri['path'],
    '#options' => $uri['options'],
    '#prefix' => '<div class="return-link">',
    '#suffix' => '</div>',
  );
  
  // Render the single value, without the labeling.
  $vars['content'] = field_view_value(
    $element['#entity_type'],
    $element['#entity'], 
    $element['#field']['field_name'],
    $element['#item'],
    array('label' => 'hidden')
  );
  
  $vars['content_attributes_array']['class'][] = 'field-content';
  $vars['content_attributes_array']['class'][] = 'clearfix';
}


// ================================
// Fieldgroup
// ================================

/**
 * Implements TEMPLATE_preprocess_iwformatter_fieldgroup().
 */
function template_preprocess_iwformatter_fieldgroup(&$vars) {
  $element    = $vars['element'];
  $fieldgroup = $element['#fieldgroup'];
}
