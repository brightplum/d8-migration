<?php
/**
 * @file
 * Utility and additions for supporting import tasks.
 */

function cdreams_profile_clear_all_profiles($form, &$form_state) {
  if (!empty($form_state['confirmed'])) {
    $form['message'] = array(
      '#markup' => t(
        'This is the final confirmation before deletion will begin. '.
        '<strong>Are you sure you want to delete all profiles?</strong>'
      ),
    );
    
    $submit_msg = t('Confirm Delete');
  }
  else {
    $form['message'] = array(
      '#markup' => t(
        'Allows the deletion of all current user profiles. '.
        '<strong>Be very careful this process as it cannot be undone.</strong>'
      ),
    );
    
    $submit_msg = t('Delete ALL Profiles');
  }

  $form['actions'] = array(
    '#type' => 'actions',
    
    'submit' => array(
      '#type' => 'submit',
      '#value' => t($submit_msg),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => 'Cancel',
      '#href' => 'admin/config/system/cdreams-import',
      '#attributes' => array('class' => array('button', 'cancel-action')),
    )
  );
  
  return $form;
}

/**
 * Form submit callback to handle the deleting of profiles.
 * Tests if the delete has been confirmed before proceeding.
 */
function cdreams_profile_clear_all_profiles_submit($form, &$form_state) {
  if (empty($form_state['confirmed'])) {
    $form_state['confirmed'] = TRUE;
    $form_state['rebuild'] = TRUE;
  }
  else {
    $batch = array(
      'operations' => array(
        array('_cdreams_profile_batch_delete_all_profiles', array()),
      ),
      'finished' => '_cdreams_profile_batch_finished',
      'progress_message' => t('Processed @current out of @total.'),
      'title' => t('Deleting profile content'),
      'file' => drupal_get_path('module', 'cdreams_profile') .'/pages/import.inc',
    );
    
    batch_set($batch);
  }
}

/**
 * Batch callback which handles the deleting of profiles. 
 */
function _cdreams_profile_batch_delete_all_profiles(&$context) {
  $query = db_select('cdreams_profiles', 'p')
    ->fields('p', array('profile_id'));
    
  if (!isset($context['sandbox']['pos'])) {
    $context['sandbox']['pos']   = 0;
    $context['sandbox']['count'] = 0;
    $context['sandbox']['max']   = $query->countQuery()->execute()->fetchField();
  }
  
  $ids = $query->condition('profile_id', $context['sandbox']['pos'], '>')
    ->orderBy('profile_id', 'ASC')
    ->range(0, 100)
    ->execute()->fetchCol();
      
  if (!empty($ids)) {
    foreach (entity_load('cdreams_profile', $ids) as $id => $profile) {
      $profile->delete();
        
      $context['sandbox']['pos'] = $id;
      ++$context['sandbox']['count'];
    }
    
    // Check for completion, and report the progress.
    $context['finished'] = ($context['sandbox']['count'] >= $context['sandbox']['max']) 
      ? 1 : ($context['sandbox']['count']/ $context['sandbox']['max']);
    
    $context['message']  = t('%count profiles purged', array('%count' => $context['sandbox']['count']));
  }
}

/**
 * Batch completion summary and final checks.
 */
function _cdreams_profile_batch_finished($success, $results, $operations) {
  if ($success) {
    // Here we do something meaningful with the results.
    $message = count($results) .' deleted.';
    $message .= theme('item_list', $results);  // D6 syntax
    drupal_set_message($message);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
    drupal_set_message($message, 'error');
  }
}