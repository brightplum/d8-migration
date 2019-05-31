<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */


$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'author')),
  'mappers' => array(
    new iwMapper(array(
      'inputs' => array('title'),
      'result' => 'name',
    )),
    new iwMapper(array(
      'inputs' => array('field_author_photo'),
      'result' => 'field_profile_img',
      'multi' => TRUE,
    )),
    
    // Fix links and old URLs
    $mapper = new iwD6MarkupMapper(array(
      'inputs'   => array('body'),
      'result'   => 'field_desc',
      'imgPath'  => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl'  => '/sites/default/files/',
      'linkPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'linkRepl' => '/sites/default/files/',
      'multi'    => TRUE,
    )),
  ),
  'writers' => array(new cdTrackedProfileWriter(array('bundle' => 'author'))),
);
