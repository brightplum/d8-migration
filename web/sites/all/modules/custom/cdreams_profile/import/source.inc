<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */


$def = array(
  'reader' => new iwD6NodeReader(array('type' => 'source')),
  'mappers' => array(
    new iwMapper(array(
      'inputs' => array('title'),
      'result' => 'name',
    )),
    new iwNestedMapper(array(
      'inputs' => array('field_main_source_url[0][value]'),
      'result' => 'field_src_url[0][url]',
    )),
    new iwMapper(array(
      'inputs' => array('field_copyright_info'),
      'result' => 'field_copyright',
      'multi' => TRUE,
    )),
    new iwMapper(array(
      'inputs' => array('field_source_image'),
      'result' => 'field_profile_img',
      'multi' => TRUE,
    )),
    
    // Fix links and old URLs
    $mapper = new iwD6MarkupMapper(array(
      'inputs' => array('body'),
      'result' => 'field_desc',
      'imgPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'imgRepl' => '/sites/default/files/',
      'linkPath' => '(?:https?://(?:www\.)?commondreams.org|)/sites/commondreams.org/files/',
      'linkRepl' => '/sites/default/files/',
      'multi' => TRUE,
    )),
  ),
  'writers' => array(new cdTrackedProfileWriter(array('bundle' => 'source'))),
);
