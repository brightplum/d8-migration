<?php 

/**
 * Attempt to determine the "section" to which a given node belongs.
 */
function parsely_get_section($node) {
  $section_name = 'Uncategorized';
  $section_term = NULL;
  $section_logic = variable_get('parsely_section_logic');

  switch($section_logic) { 
  
    case 0: 
      //$section_id = variable_get('parsely_section_by_node');
      $section_by_node_type = variable_get('parsely_nodes'); 
      if (module_exists('node') && !is_null($section_by_node_type)) {
    
        $node = menu_get_object(); 
        if (in_array($node->type, $section_by_node_type)) {
          $section_name = $node->type; 
        }
      } 
    break; 
      
    case 1: 
      //$section_id = variable_get('parsely_section_vocabulary');
      $section_vocabulary_id = variable_get('parsely_section_vocabulary');
      
    /* @FIXME: code indentation */          
    if (module_exists('taxonomy') && !is_null($section_vocabulary_id)) {
    // Find the most relevant term from the designated vocab, using the
    // criterion specified in the module configuration. See
    // parsely_admin_settings() for an explanation of the supported criteria.
    $selection_criterion = variable_get('parsely_section_term_criterion', 'first');

    $section_term_query = db_select('taxonomy_index', 'ti');
    $section_term_query->join('taxonomy_term_data', 'ttd', 'ti.tid=ttd.tid');
    $section_term_query->fields('ttd', array('tid'))
      ->condition('ti.nid', $node->nid)
      ->condition('ttd.vid', $section_vocabulary_id);

    // "First term" or "Last term."
    if ($selection_criterion == 'first' || $selection_criterion == 'last') {
      $sort_direction = ($selection_criterion == 'first') ? 'ASC' : 'DESC';
      $section_term_query->orderBy('ttd.tid', $sort_direction)
        ->range(0, 1);
      $section_term_result = $section_term_query->execute();

      if ($section_term_result->rowCount() > 0) {
        $section_tid = $section_term_result->fetchField();
        $section_term = taxonomy_term_load($section_tid);
      }
    }
    // "Highest-level term."
    elseif ($selection_criterion == 'highest') {
      $all_terms_result = $section_term_query->orderBy('ttd.tid')
        ->execute();

      if ($all_terms_result->rowCount() > 0) {
        $tids = $all_terms_result->fetchCol();
        $terms = taxonomy_term_load_multiple($tids);
        // Examine each term and see how many parents it has. We could use
        // taxonomy_get_tree() to get the depths of every term in the vocab,
        // but that could be problematic for large vocabs.
        foreach ($terms as $term) {
          // Calculate depth by repeatedly calling taxonomy_get_parents(). This
          // is preferable to calling taxonomy_get_parents_all(), because that
          // would include all parents of any multi-parent terms, skewing our
          // generation count. Note that $depth will be "1" for a top-level term
          // here instead of the conventional "0."
          $t = $term;
          $depth = 0;
          while (!is_null($t)) {
            $depth++;
            $parents = taxonomy_get_parents($t->tid);
            $t = array_pop($parents);
          }
          if (!isset($highest_term) || $depth < $highest_term->depth) {
            $term->depth = $depth;
            $highest_term = $term;
          }
        }
        $section_term = $highest_term;
      }
    }
    // "Highest-level ancestor."
    elseif ($selection_criterion == 'ancestor') {
      $first_term_result = $section_term_query->orderBy('ti.created')
        ->range(0, 1)
        ->execute();

      if ($first_term_result->rowCount() > 0) {
        $first_term_tid = $first_term_result->fetchField();
        $lineage = taxonomy_get_parents_all($first_term_tid);
        $section_term = array_pop($lineage);
      }
    }
  }

  if ($section_term && property_exists($section_term, 'name')) {
    $section_name = $section_term->name;
  }
  break;
  
  }

  // Allow any module to alter the section name.
  drupal_alter('parsely_section', $section_name, $node, $section_term);

  return $section_name;
}


