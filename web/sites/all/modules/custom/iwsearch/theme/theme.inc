<?php
/**
 * @file
 * Contains preprocessing and theme functions for iwsearch items.
 */


/**
 * Implements TEMPLATE_preprocess_iwsearch_results().
 */
function template_preprocess_iwsearch_results(&$vars) {
  $element  = &$vars['element'];
  $provider = $element['#provider'];
  
  $vars['array_classes'][] = 'search-results';
  $vars['array_classes'][] = strtr($provider->getName(), '_', '-') .'-results';
  
  $vars['summary'] = empty($element['#summary']) 
    ? t('All @label', array('@label' => $provider->label))
    : implode(' AND ', $element['#summary']);

  $vars['count']   = $element['#count'];
  $vars['content'] = $element['#results'];
  
  // Handle the pager if there is one.
  if (!empty($element['#has_pager'])) {
    pager_default_initialize($vars['count'], $element['#page_size']);

    $vars['pager'] = theme('pager');
  }
  else {
    $vars['pager'] = '';
  }
  
  $vars['theme_hook_suggestions'][] = 'iwsearch_results';
  $vars['theme_hook_suggestions'][] = 'iwsearch_results__' . $provider->getName();
}
