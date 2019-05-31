<?php
/**
 * @file
 * Contains the definition for the headline article import.
 */

// --------------------------------------
// TODO: Change this to menu categories.
$def = array(
  'reader' => new iwD6TermReader(array('vocab' => 'Menu Categories')),
  'mappers' => array(),
  'writers' => array(new TrackedTermWriter()),
);
