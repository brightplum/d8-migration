<?php
/**
 * @file
 * Contains the definition for importing further documents
 */

$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'further')),
  'mappers' => array(
    new iwNestedMapper(array(
      'inputs' => array('created'),
      'result' => 'field_article_date[0][value]',
    )),

    new iwMapper(array(
      'inputs' => array('field_further_author'),
      'result' => 'field_author_name',
      'multi' => TRUE
    )),
    new iwMapper(array(
      'inputs' => array('field_further_link'),
      'result' => 'field_more_text',
      'multi' => TRUE
    )),
    new iwMapper(array(
      'inputs' => array('field_hp_headline'),
      'result' => 'field_hp_title',
      'multi' => TRUE
    )),
    new iwMapper(array(
      'inputs' => array('field_news_hp_image'),
      'result' => 'field_image',
      'multi' => TRUE
    )),
    new iwMapper(array(
      'inputs' => array('field_news_hp_image'),
      'result' => 'field_social_media_image',
      'multi' => TRUE
    )),
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
    $mapper = new iwD6MarkupMapper(array(
      'inputs' => array('body'),
      'result' => 'body',
      'imgPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl' => '/sites/default/files/',
      'linkPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'linkRepl' => '/sites/default/files/',
      'multi' => TRUE,
    )),
    new iwNestedMapper(array(
      'inputs' => array('field_further_teaser[0][value]'),
      'result' => 'body[0][summary]',
    )),
    new iwMapper(array(
      'inputs' => array('body_text_format'),
      'setData' => array('body_text_format' => 'full_html'),
      'result' => 'body[0][format]',
    )),
  ),
  'writers' => array(
    new TrackedNodeWriter(array('bundle' => 'further', 'promote' => 0)),  
    new DisqusWriter(),
  ),
);
