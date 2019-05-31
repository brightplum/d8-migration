<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */


$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'newswire')),
  'mappers' => array(
    new iwNestedMapper(array(
      'inputs' => array('created'),
      'result' => 'field_article_date[0][value]',
    )),

    new iwMapper(array(
      'inputs' => array('field_hp_headline'),
      'result' => 'field_hp_title',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_subtitle'),
      'result' => 'field_subhead',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_subtitle_2'),
      'result' => 'field_subhead2',
      'multi' => TRUE,
    )),
    
    new iwMapper(array(
      'inputs' => array('field_location'),
      'result' => 'field_location',
      'multi' => TRUE,
    )),
    
    // Source profile and override
    new iwMapper(array(
      'inputs' => array('field_organization'),
      'result' => 'field_organization',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_source_url'),
      'result' => 'field_src_url',
      'multi' => TRUE,
    )),
    new iwD6MarkupMapper(array(
      'inputs' => array('field_organization_info'),
      'result' => 'field_desc',
      'imgPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl' => '/sites/default/files/',
      'linkPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'linkRepl' => '/sites/default/files/',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_contact_person'),
      'result' => 'field_contact_info',
      'multi' => TRUE,
    )),
    
    // Taxonomy
    new iwMapper(array(
      'inputs' => array('term_menu_categories'),
      'result' => 'field_menu_categories',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('term_topic'),
      'result' => 'field_topics',
      'multi' => TRUE,
    )),
    
    // Fix links and old URLs
    new iwD6MarkupMapper(array(
      'inputs' => array('body'),
      'result' => 'body',
      'imgPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl' => '/sites/default/files/',
      'linkPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'linkRepl' => '/sites/default/files/',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('body_text_format'),
      'setData' => array('body_text_format' => 'full_html'),
      'result' => 'body[0][format]',
    )),
  ),
  'writers' => array(
    new TrackedNodeWriter(array('bundle' => 'newswire', 'promote' => 0)),
    new DisqusWriter(),
  ),
);
