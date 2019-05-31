<?php
/**
 * @file
 * Contains the views API HOOKs
 */

/**
 * Implements HOOK_views_data()
 */
function cdreams_profile_views_data() {    
  $profiles_table['table'] = array(
    'group'       => t('Profile Types'),
    'entity type' => 'cdreams_profile',
    'base'        => array(
      'field' => 'profile_id',
      'title' => t('Profiles'),
      'weight' => -10,
      'access query tag' => 'cdreams_profile_access',
      'defaults' => array('field' => 'name'),
    ),
  );

  $profiles_table['profile_id'] = array(
    'title' => t('Profile ID'),
    'help' => t('The profile ID.'),
    'field' => array(
      'handler' => 'iwapi_views_handler_field_entity',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'iwapi_views_handler_argument_entity_id',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'filter' => array('handler' => 'views_handler_filter_numeric'),
    'sort' => array('handler' => 'views_handler_sort'),
  );

  // title
  // This definition has more items in it than it needs to as an example.
  $profiles_table['name'] = array(
    'title' => t('Name'), // The item it appears as on the UI,
    'help' => t('The name of the profile item.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'name',
      'group' => t('Profile Types'),
      'handler' => 'iwapi_views_handler_field_entity',
      'click sortable' => TRUE,
      'link_to_entity default' => TRUE,
     ),
    'sort'     => array('handler' => 'views_handler_sort'),
    'filter'   => array('handler' => 'views_handler_filter_string'),
    'argument' => array('handler' => 'views_handler_argument_string'),
  );

  // created field
  $profiles_table['created'] = array(
    'title' => t('Created date'), // The item it appears as on the UI,
    'help' => t('The date the profile was created.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort'   => array('handler' => 'views_handler_sort_date'),
    'filter' => array('handler' => 'views_handler_filter_date'),
  );

  // changed field
  $profiles_table['changed'] = array(
    'title' => t('Updated date'), // The item it appears as on the UI,
    'help' => t('The date the profile was last updated.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort'   => array('handler' => 'views_handler_sort_date'),
    'filter' => array('handler' => 'views_handler_filter_date'),
  );

  // Content type
  $profiles_table['type'] = array(
    'title' => t('Type'), // The item it appears as on the UI,
    'help' => t('The profile type (for example, "Author", "Source", etc).'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'cdreams_profile_views_handler_field_entity_bundle',
      'click sortable' => TRUE,
    ),
    'sort'   => array('handler' => 'views_handler_sort'),
    'filter' => array('handler' => 'iwapi_views_handler_filter_entity_bundle'),
  );

  // published status
  $profiles_table['status'] = array(
    'title' => t('Published'),
    'help' => t('Whether or not the profile is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'published-notpublished' => array(t('Published'), t('Not published')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
      'use equal' => TRUE, // Use status = 1 instead of status <> 0 in WHERE statment
    ),
    'sort' => array('handler' => 'views_handler_sort'),
  );
  
  // uid field
  $profiles_table['uid'] = array(
    'title' => t('Author uid'),
    'help' => t('The user authoring the profile. If you need more fields than the uid add the content: author relationship'),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t('Relate profile to the user who created it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('author'),
    ),
    'field'    => array('handler' => 'views_handler_field_user'),
    'filter'   => array('handler' => 'views_handler_filter_user_name',),
    'argument' => array('handler' => 'views_handler_argument_numeric'),
  );

  return array('cdreams_profiles' => $profiles_table);
}
