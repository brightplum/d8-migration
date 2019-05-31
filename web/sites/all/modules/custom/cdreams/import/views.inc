<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */


$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'views_article')),
  'mappers' => array(
    new iwNestedMapper(array(
      'inputs' => array('created'),
      'result' => 'field_article_date[0][value]',
    )),

    new iwMapper(array(
      'inputs' => array('field_subtitle'),
      'result' => 'field_subhead',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_hp_headline'),
      'result' => 'field_hp_title',
      'multi' => TRUE,
    )),
    
    // Author profiles and overrides
    new iwMapper(array(
      'inputs' => array('field_op_author'),
      'result' => 'field_author_profile',
      'multi' => TRUE,
    )),
    new iwTextReplaceMapper(array(
      'inputs' => array('field_source_author'),
      'result' => 'field_author_name',
      'patterns' => array('#^\s*(by|-|~)\s*#i'),
      'replace' => array(''),
      'multi' => TRUE,
    )),
    
    // Fix links and old URLs
    new iwD6MarkupMapper(array(
      'inputs' => array('field_author_bio'),
      'result' => 'field_author_bio',
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
    
    // Source profiles and overrides
    new iwMapper(array(
      'inputs' => array('field_op_source'),
      'result' => 'field_src_profile',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_source_url'),
      'result' => 'field_src_url',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_copyright_info'),
      'result' => 'field_copyright',
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
  ),
  'writers' => array(
    new TrackedNodeWriter(array('bundle' => 'views_article', 'promote' => 0)),
    new DisqusWriter(),
  ),
);
