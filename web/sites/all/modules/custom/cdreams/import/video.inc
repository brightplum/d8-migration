<?php
/**
 * @file
 * Contains the definition for importing further documents
 */

$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'video_article')),
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
      'result' => 'field_subtitle',
      'multi' => TRUE,
    )),

    new iwMapper(array(
      'inputs' => array('field_video_thumbnail'),
      'result' => 'field_image',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_video_thumbnail'),
      'result' => 'field_social_media_image',
      'multi' => TRUE
    )),

    new iwMapper(array(
      'inputs' => array('field_main_video_sentence'),
      'result' => 'field_featured_sentence',
      'multi' => TRUE,
    )),

    new iwMapper(array(
      'inputs' => array('field_embed_video_code'),
      'result' => 'field_video_text',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('video_text_format'),
      'setData' => array('video_text_format' => 'full_html'),
      'result' => 'field_video_text[0][format]',
    )),

    new iwTextReplaceMapper(array(
      'inputs' => array('field_source_author'),
      'result' => 'field_author_name',
      'patterns' => array('#^\s*(by|-|~)\s*#i'),
      'replace' => array(''),
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
    
    // Fix links and old URLs
    $mapper = new iwD6MarkupMapper(array(
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
    
  ),
  'writers' => array(
    new TrackedNodeWriter(array('bundle' => 'video_article', 'promote' => 0)),
    new DisqusWriter(),
  ),
);
