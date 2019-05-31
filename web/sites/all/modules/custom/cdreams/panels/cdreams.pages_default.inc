<?php
/**
 * @file
 * Contains the default hooks for the page manager module.
 */


/**
 * Implements HOOK_default_page_manager_pages()
 */
function cdreams_default_page_manager_pages() {
  static $page_files;
  
  if (!isset($page_files)) {
    $files = file_scan_directory(__DIR__, '#\.panel\.inc$#', array('recurse' => FALSE));  
    $page_files = array_keys($files);
  }
  
  $pages = array();
  foreach ($page_files as $uri) {
    include $uri;
    $pages[$page->name] = $page;
  }

  return $pages;
}

/**
 * Implements HOOK_default_page_manager_handlers()
 */
function cdreams_default_page_manager_handlers() {
  static $handler_files;
  
  if (!isset($handler_files)) {
    $files = file_scan_directory(__DIR__, '#\.handler\.inc$#', array('recurse' => FALSE));  
    $handler_files = array_keys($files);
  }
  
  $handlers = array();
  foreach ($handler_files as $uri) {
    include $uri;
    $handlers[$handler->name] = $handler;
  }

  return $handlers;
}
