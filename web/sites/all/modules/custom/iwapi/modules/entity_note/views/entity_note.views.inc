<?php
/**
 * @file
 * Contains API hooks for views integration.
 */

/**
 * Implements HOOK_views_data().
 */
function entity_note_views_data() {
  $note_data['table'] = array(
    'group' => t('Entity note'),
    'entity type' => 'entity_note',
    'base' => array(
      'field' => 'enid',
      'title' => t('Entity notes (comments)'),
      'access query tag' => 'entity_note_access',
      'defaults' => array('field' => 'subject'),
    ),
  );
    
  // Describe each of the table fields, and how to handle them.
  $note_data['enid'] = array(
    'title' => t('Entity note ID'),
    'help' => t('The unique identifier used as the primary table key.'),
    'field' => array(
      'handler' => 'iwapi_views_handler_field_entity',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'iwapi_views_handler_argument_entity_id',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'enid',
    ),
    'filter' => array('handler' => 'views_handler_filter_numeric'),
    'sort'   => array('handler' => 'views_handler_sort'),
  );
  
  // Allow filtering by type.
  $note_data['entity_type'] = array(
    'title' => t('Note type'),
    'help' => t('The entity type this note was generated for.'),
    'filter' => array('handler' => 'entity_note_views_handler_filter_type'),
  );
  
  // The workgroup ID
  $note_data['uid'] = array(
    'title' => t('Author uid'),
    'help' => t('The user authoring the content. If you need more fields than the uid add the content: author relationship'),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t('Relate content to the user who created it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('author'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
  );
  
  $note_data['subject'] = array(
    'title' => t('Subject'),
    'help' => t('The note subject.'),
    'field' => array(
      'handler' => 'iwapi_views_handler_field_entity',
      'click sortable' => TRUE,
      'link_to_entity default' => TRUE,
    ),
    'argument' => array('handler' => 'views_handler_argument_string'),
    'filter'   => array('handler' => 'views_handler_filter_string'),
    'sort'     => array('handler' => 'views_handler_sort'),
  );
  
  // Work status based on the active status.
  $note_data['status'] = array(
    'title' => t('Published'),
    'help' => t('Whether or not the comment is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'publishing' => array(t('Published'), t('Unpublished')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
      'use equal' => TRUE, // Use status = 1 instead of status <> 0 in WHERE statement
    ),
    'sort' => array('handler' => 'views_handler_sort'),
  );

  $note_data['created'] = array(
    'title' => t('Post date'),
    'help' => t('The date the work product was posted.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array('handler' => 'views_handler_filter_date'),
    'sort'   => array('handler' => 'views_handler_sort_date'),
  );
  
  $note_data['changed'] = array(
    'title' => t('Changed'),
    'help' => t('The date this work product was last changed.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array('handler' => 'views_handler_filter_date'),
    'sort'   => array('handler' => 'views_handler_sort_date'),
  );

  return array('entity_notes' => $note_data);
}
