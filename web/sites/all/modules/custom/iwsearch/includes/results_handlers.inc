<?php
/**
 * @file
 * Functionality to assist with managing search results.
 */

function _iwsearch_get_results_handlers() {
  $handlers = &drupal_static(__FUNCTION__);

  if (!isset($handlers)) {
    $handlers = module_invoke_all('iwsearch_results_handler') + array(
      'iwEntityViewResults'    => array('label' => t('Entity view modes')),
      'iwSolrSnippetResults'   => array('label' => t('Solr default results format')),
      'iwTabularSearchResults' => array('label' => t('Table results')),
    );

    // allow altering any of the results properties by other modules.
    drupal_alter('iwsearch_results_handler', $handlers);
  }
  return $handlers;
}
