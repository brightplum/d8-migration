<?php
/**
 * @file
 * Contains the administrative form definitions for entity notes module.
 */


/**
 * Form builder to create the overall form for managing
 * the different entity notes settings.
 */
function entity_note_admin_settings($form, &$form_state) {
  $types = array();
  
  foreach (_entity_note_get_supported_types() as $type) {
    $info = entity_get_info($type);

    $types[$type] = check_plain($info['label']);
  }
  
  $form['msg'] = array(
    '#markup' => theme('item_list', array(
      'title' => t('Supported note entity types'),
      'items' => $types,
    )),
  );
  
  return $form;
}
