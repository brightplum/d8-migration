<?php
/**
 * @file
 * Contains most of the form builders, and page callbacks for entity_note module.
 */


/**
 * Page callback to perform the AJAX handling of
 * additional entity adding.
 * 
 * @return array
 *  AJAX command response in a renderable array.
 */
function _entity_note_add_page(iwEntity $entity, $type) {
  global $user;
    
  $state = array();  
  iwapi_include('ajax');
  
  // Create a new note set to link with this entity.
  $note = new NoteEntity(array(
    'entity_type' => $type,
    'entity_id' => $entity->getId(),
    'uid' => $user->uid,
  ));
    
  $form = _iwapi_ajax_get_form('entity_note_edit_note', $state, $note);
  $form['#attributes']['class'][] = 'add-entity-note';
  
  if (iwapi_ajax_form_complete($state, NULL, TRUE)) {
    $display = $state['entity_note']->view('full');
    $data['comment'] = render($display);
  }
  
  $form = array(
    'msg' => array('#theme' => 'status_messages'),
    'form' => $form,
  );
  $data['form'] = render($form);
  
  // Separate the form elements and comment additions.
  return array('#type' => 'ajax', '#commands' => array(
    array('command' => 'entity_note_add') + $data),
  );
}

// ==================================
// Edit entity note
// ==================================

/**
 * Returns an editing form, either for an AJAX or a regular
 * page request depending on the value of $ajax parameter.
 * 
 * @param NoteEntity $note
 *  The entity note which is being edited.
 * @param string $ajax
 *  Boolean to indicate that the function expects
 *  to have a return an AJAX response.
 * @return array
 */
function _entity_note_edit_page(NoteEntity $note, $ajax = FALSE) {
  if ($ajax) {
    $state = array();
    
    iwapi_include('ajax');
    $form = _iwapi_ajax_get_form('entity_note_edit_note', $state, $note);
    
    if (!iwapi_ajax_form_complete($state, NULL, TRUE)) {
      $content = array(
        'msg' => array('#theme' => 'status_messages'),
        'title' => array(
          '#type' => 'html_tag',
          '#tag' => 'h3',
          '#value' => t('Edit comment'),
        ),
        'form' => $form,
      );
    }
    else {
      $content = $state['entity_note']->view('full');
    }
    
    return array('#type' => 'ajax', '#commands' => array(
      array(
        'command' => 'entity_note_edit',
        'selector' => "#entity-note-{$note->enid}",
        'html' => render($content),
      ),
    ));
  }
  else {
    return drupal_get_form('entity_note_edit_note', $note);
  }
}

/**
 * Form builder to generate an edit form for building note objects.
 */
function entity_note_edit_note($form, &$form_state, NoteEntity $note) {
  global $user;
  
  // Make sure this gets included to capture all the form submit / states.
  form_load_include($form_state, 'inc', 'entity_note', 'entity_note.pages');
  
  $form['#attributes']['class'][] = 'comment-form';
  
  // Determine the correct note item to use to build the edit form.
  if (!isset($form_state['entity_note'])) {
    $form_state['entity_note'] = $note;
  }
  else {
    $note = $form_state['entity_note'];
  }
  
  $pass_thru = array('enid', 'entity_type', 'entity_id', 'thread', 'parent', 'uid', 'created');
  foreach ($pass_thru as $val_name) {
    if (isset($note->{$val_name})) {
      $form[$val_name] = array(
        '#type' => 'value',
        '#value' => $note->{$val_name},
      );
    }
  }
  
  // Expose these for the entity $id form.
  $form['entity_type']['#type'] = 'hidden';
  $form['entity_id']['#type']   = 'hidden';
  
  // Display a little differently depending on if the user is the same as the author.
  if ($user->uid == $note->uid) {
    $form['current_user'] = array(
      '#prefix' => '<div class="form-item comment-author"><label>Your name</label> ',
      '#suffix' => '</div>',
      '#theme' => 'username',
      '#account' => user_load($note->uid),
      '#weight' => -20,
    );
  }
  
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#required' => TRUE,
    '#default_value' => $note->subject,
    '#weight' => -15,
  );
  
  // Add field base elements to the form.
  field_attach_form('entity_note', $note, $form, $form_state);
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );

  return $form;
}

/**
 * Form validate callback for the edit entity note.
 */
function entity_note_edit_note_validate($form, &$form_state) {
  entity_form_field_validate('entity_note', $form, $form_state);
}

/**
 * Form submit callback for the edit entity note.
 */
function entity_note_edit_note_submit($form, &$form_state) {
  $note = $form_state['entity_note'];
  
  try {
    entity_form_submit_build_entity('entity_note', $note, $form, $form_state);
    
    $note->save();
    
    // Redirect to the targeted entity where this note was attached.
    if ($target = iwapi_entity_object_load($note->entity_id, $note->entity_type)) {
      $uri = $target->getUri();
      $form_state['redirect'] = array('path' => $uri['path']);
    }
  }
  catch (Exception $e) {
    watchdog_exception('entity_note', $e);
    drupal_set_message(t('Unable to save your comment due to an internal error.'), 'error');
  }
}


// ==================================
// Replying to entity notes
// ==================================

/**
 * Generate a response form for a user to create a
 * reply with. This allows users to respond to other
 * notes, and works with either AJAX or regular pages
 * depending on the state of the $ajax variable.
 * 
 * @param object $entity
 *  The entity object that this note belong to.
 * @param NoteEntity $note
 *  The fully loaded entity note for which the
 *  response is being built for.
 * @param bool $ajax
 *  Boolean to indicate if we are requesting AJAX
 *  or HTML page content to be returned.
 * @return array
 *  Renderable array containing the entity note
 *  response form, either for AJAX or a page view
 *  depending on the $ajax variable.
 */
function _entity_note_reply_page(NoteEntity $note, $ajax = FALSE) {
  global $user;
  
  $reply = new NoteEntity(array(
    'entity_type' => $note->entity_type,
    'entity_id' => $note->entity_id,
    'parent' => $note->enid,
    'uid' => $user->uid,
  ));
  
  if ($ajax) {
    iwapi_include('ajax');
    
    $state = array();
    $form  = _iwapi_ajax_get_form('entity_note_edit_note', $state, $reply);
    $reply = $state['entity_note'];
    
    // Check the submissions status of the form, and build appropriate command.
    if (!iwapi_ajax_form_complete($state, NULL, TRUE)) {
      $content = array(
        'msg' => array('#theme' => 'status_messages'),
        'title' => array(
          '#type' => 'html_tag',
          '#tag' => 'h3',
          '#value' => t('Your response'),
        ),
        'form' => $form,
      );
    }
    elseif (!empty($reply->enid)) {
      $content = $reply->view('full');
    }
    else {
      $content = array(); // empty or no content.
    }
    
    $command = array(
      'command' => 'entity_note_reply',
      'selector' => "#entity-note-{$note->enid}",
      'html' => render($content),
    );
    return array('#type' => 'ajax', '#commands' => array($command));
  }
  else {
    $content = $note->view('full');
    unset($content['note_links']);
    
    return array(
      'reply_to' => $content,
      'form' => drupal_get_form('entity_note_edit_note', $reply),
    );
  }
}


// ==================================
// Deleting entity notes
// ==================================

/**
 * Page callback to get a delete confirmation form
 * for deleting an entity note.
 * 
 * @param NoteEntity $note
 *  The note object to confirm delete for.
 * @param bool $ajax
 *  Boolean to indicate if we are requesting AJAX
 *  or HTML page content to be returned.
 * @return array
 *  Renderable array, either in AJAX command form or
 *  as a processed form depending on the $ajax variable.
 */
function _entity_note_delete_page(NoteEntity $note, $ajax = FALSE) {
  if ($ajax) {
    $state = array();
    
    iwapi_include('ajax');
    $form = _iwapi_ajax_get_form('entity_note_delete_confirm', $state, $note);
    if (!$command = iwapi_ajax_form_complete($state, 'reload', TRUE)) {
      $command = iwapi_ajax_command_dialog(drupal_get_title(), $form);
    }
   
    return array('#type' => 'ajax', '#commands' => array($command));
  }
  else {
    return drupal_get_form('entity_note_delete_confirm', $note);
  }
}

/**
 * Form builder to create a confirmation form before deleting a comment.
 */
function entity_note_delete_confirm($form, &$form_state, NoteEntity $note) {
  $form_state['entity_note'] = $note;
  
  $entity = iwapi_entity_object_load($note->entity_id, $note->entity_type);
  $uri = $entity->getUri();
  
  $form['information'] = array(
    '#markup' => '<p>'. t('Are you sure you wish to delete this comment: ') .'<p>',
  );
  
  $form['preview'] = $note->view('full');
  unset($form['preview']['links']);
  
  $form['warning'] = array(
    '#markup' => '<strong>'. t('This action cannot be undone, and will remove all replies as well!') .'</strong>',
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'delete' => array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    ),
    'cancel' => array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => $uri['path'],
      '#attributes' => array('class' => array('cancel-action', 'button')),
    ),
  );
  
  return $form;
}

/**
 * Form submit callback after the entity note has been confirmed
 * for deletion.
 */
function entity_note_delete_confirm_submit($form, &$form_state) {
  $note   = $form_state['entity_note'];
  $entity = iwapi_entity_object_load($note->entity_id, $note->entity_type);
  
  try {
    $note->delete();
    
    drupal_set_message(t(
      'Comment "%name" has been successfully deleted',
      array('%name' => $note->subject)
    ));
    
    $uri = $entity->getUri();
    $form_state['redirect'] = array('path' => $uri['path']);
  }
  catch (Exception $e) {
    watchdog_exception('entity_note', $e);
    drupal_set_message(t(''));
  }
}
