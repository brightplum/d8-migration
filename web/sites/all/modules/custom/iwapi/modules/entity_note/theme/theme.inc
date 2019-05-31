<?php
/**
 * @file
 * Contains the preprocess and theme functions for the entity note module.
 */


/**
 * Implements TEMPLATE_preprocess_entity_note().
 */
function template_preprocess_entity_note(&$vars) {
  $element = $vars['element'];
  $note = $element['#entity'];
  
  $note->preprocess($element, $vars);
  
  $vars['content_attributes_array']['class'][] = 'comment-content';
  
  $vars['attribution'] = t('Submitted by !username on !posted', array(
    '!username' => theme('username', array('account' => user_load($note->uid))),
    '!posted' => format_date($note->created, 'medium'),
  ));
  
  if (isset($note->replies)) {
    $vars['children'] = $note->replies + array(
      '#theme_wrappers' => array('container'),
      '#attributes' => array('class' => array('indented')),
    );
  }
}
