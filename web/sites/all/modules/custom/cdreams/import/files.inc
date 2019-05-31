<?php
/**
 * @file
 * Contains the import definition for transferring files from D6 to D7.
 */

$def = array(
  'reader' => new iwD6FileReader(),
  'mappers' => array(
    new iwD6FileMapper(array(
      'inputs' => array('filepath'),
      'result' => 'filepath',
      'modifyPath' => array('regex' => '#^/?sites/commondreams.org/files/#'),
    )),
    new iwD6FileMapper(array(
      'inputs' => array('filepath'),
      'result' => 'src',
      'modifyPath' => array(
        'regex' => '#^/?sites/commondreams.org/files/#',
        'prefix' => 'old_files/',
      ),
    )),
  ),
  'writers' => array(
    new TrackedFileWriter(array(
      'destination' => 'public://',
      'fileType' => 'images',
    )),
  ),  
);
