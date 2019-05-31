<?php


/**
 * Implements TEMPLATE_preprocess_cdreams_related_content().
 */
function template_preprocess_iwsearch_related_content(&$vars) {
  $vars['classes_array'][] = 'clearfix';
  $vars['content_attributes_array']['class'][] = 'clearfix related-content-wrapper';
  
  $element = $vars['element'];
  $vars['entities'] = array();
  
  foreach ($element['#entities'] as $id => $entity) {
    if ($entity['type'] == 'node') {
      $vars['entities'][$id] = array(
        'attributes' => array('class' => array('related-item')),
        'entity' => $entity,
      );
    }
    else {
      $vars['entities'][$id] = array(
        'attributes' => array('class' => array('related-item')),
        'entity' => $entity['entity'],
      );
    }

  }
  
  // Add the first and last CSS class names.
  if (reset($vars['entities'])) {
    $key = key($vars['entities']);
    $vars['entities'][$key]['attributes']['class'][] = 'alpha';
    
    end($vars['entities']);
    $key = key($vars['entities']);
    $vars['entities'][$key]['attributes']['class'][] = 'omega';
  }
}
