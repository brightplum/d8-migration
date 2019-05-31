<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */


$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'organization')),
  'mappers' => array(
    new iwMapper(array(
      'inputs' => array('title'),
      'result' => 'name',
    )),
    new iwMapper(array(
      'inputs' => array('field_contact_info'),
      'result' => 'field_contact_info',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_organization_image_sm'),
      'result' => 'field_profile_img',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('term_organization_category'),
      'result' => 'field_org_categories',
      'multi' => TRUE,
    )),
    
    // Links
    new iwMapper(array(
      'inputs' => array('field_home_page_url'),
      'result' => 'field_org_website',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_pressmedia_center_link'),
      'result' => 'field_pressmedia_link',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_action_page_link'),
      'result' => 'field_action_link',
      'multi' => TRUE,
    )),
      
    // Fix links and old URLs
    new iwD6MarkupMapper(array(
      'inputs' => array('field_org_description'),
      'result' => 'field_desc',
      'imgPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl' => '/sites/default/files/',
      'multi' => TRUE,
    )),
  ),
  'writers' => array(new cdTrackedProfileWriter(array('bundle' => 'organization'))),
);
