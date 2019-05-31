<?php
/**
 * @file
 */

$def = array(
  'reader' => new iwD6NodeAliasReader(array(
    'types' => array('further', 'newswire', 'headline', 'views_article', 'video_article'),
  )),
  'mappers' => array(
    new iwMapper(array(
      'inputs' => array('dst'),
      'result' => 'source',
    )),
    new iwMapper(array(
      'inputs' => array('nid'),
      'result' => 'redirect',
      'procs'  => array('_cdreams_import_prefix_nid_uri'),
    )),
  ),
  'writers' => array(
    new iwRedirectWriter(array(
      'skipMissing' => TRUE,
      'skipExisting' => TRUE,
    )),
  ),
);

