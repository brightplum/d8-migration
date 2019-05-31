<?php
/**
 * @file
 * Contains a result handler that pulls result information from
 * Solr instead of rendering entity views.
 */

class iwSolrSnippetResults extends iwSearchResults {

  function additionalFields() {
    return array( 'label', 'teaser', 'content', 'bundle', 'path', 'url', 'ds_created', 'tos_name');
  }
  
  function highlightFields() {
    return array('content');
  }


  function formatResults(array $results) {    
    $formatted  = array(
      '#theme' => 'search_results',
      '#module' => 'iwsearch',
    );
    
    $entityType = $this->provider->getEntityType();
    
    $hl_fl = $this->highlightFields();
    foreach ($results['entities'] as $doc) {
      $extra = array();
      $snippets = array();

      // Find the nicest available snippet.
      foreach ($hl_fl as $hl_param) {
        if (isset($results['highlight']->{$doc->id}->$hl_param)) {
          // Merge arrays preserving keys.
          foreach ($results['highlight']->{$doc->id}->$hl_param as $value) {
            $snippets[$hl_param][] = $value;
          }
        }
      }

      // If there's no snippet at this point, add the teaser.
      if (!$snippets && isset($doc->teaser)) {
        $snippets[] = truncate_utf8($doc->teaser, 256, TRUE);
      }

      $hook = 'apachesolr_search_snippets__' . $entityType;
      $bundle = !empty($doc->bundle) ? $doc->bundle : NULL;
      if ($bundle) {
        $hook .= '__' . $bundle;
      }
      $snippet = theme($hook, array('doc' => $doc, 'snippets' => $snippets));

      // Normalize common dates so that we can use Drupal's normal date and
      // time handling.
      $doc->created = strtotime($doc->ds_created);
      $doc->name = $doc->tos_name;

      // Set all expected fields from fl to NULL if they are missing so
      // as to prevent Notice: Undefined property.
      $fl = array_merge($fl, array('path', 'label', 'score'));
      foreach ($fl as $field) {
        if (!isset($doc->{$field})) {
          $doc->{$field} = NULL;
        }
      }

      // Define our url options. They depend on the document language.
      $url_options = array('absolute' => TRUE);
      if (isset($doc->ss_language) && isset($languages[$doc->ss_language])) {
        $url_options['language'] = $languages[$doc->ss_language];
      }
      
      $result = array(
        // link is a required field, so handle it centrally.
        'link' => url($doc->path, $url_options),
        // template_preprocess_search_result() runs check_plain() on the title
        // again.  Decode to correct the display.
        'title' => htmlspecialchars_decode($doc->label, ENT_QUOTES),
        // These values are not required by the search module but are provided
        // to give entity callbacks and themers more flexibility.
        'score' => $doc->score,
        'snippets' => $snippets,
        'snippet' => $snippet,
        'fields' => (array)$doc,
        'entity_type' => entityType,
        'bundle' => $bundle,
      );

      // Call entity-type-specific callbacks for extra handling.
      $result_callback = apachesolr_entity_get_callback($entityType, 'result callback', $bundle);
      if (is_callable($result_callback)) {
        $result_callback($doc, $result, $extra);
      }

      $result['extra'] = $extra;

      $formatted['#results'][] = $result;
    }

    $formatted['#sorted'] = TRUE;
    return $formatted;
  }
}
