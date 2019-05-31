<?php
/**
 * @file
 * Contains the handlers for default views.
 */


/**
 * Implements HOOK_views_default_views().
 */
function cdreams_views_default_views() {
  static $views_files;
  
  if (!isset($views_files)) {
    $files = file_scan_directory(__DIR__, '#\.view\.inc$#', array('recurse' => FALSE));
    $views_files = array_keys($files);
  }
  
  $views = array();
  foreach ($views_files as $uri) {
    include $uri;

    $views[$view->name] = $view;
  }
  
  return $views;
}
