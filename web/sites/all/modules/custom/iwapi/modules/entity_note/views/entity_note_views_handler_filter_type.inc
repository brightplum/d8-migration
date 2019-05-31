<?php

/**
 * @file
 * Definition of iwapi_views_handler_filter_entity_bundle.
 */

/**
 * Filter by node type.
 *
 * @ingroup views_filter_handlers
 */
class entity_note_views_handler_filter_type extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
        
      $query = db_select('entity_notes', 'notes')
        ->orderBy('entity_type', 'ASC')
        ->distinct();
      
      $query->addField('notes', 'entity_type');
      $names = $query->execute()->fetchCol();
      $names = array_combine($names, $names);
      
      $entities = entity_get_info();
      $entities = array_intersect_key($entities, $names);

      $this->value_title   = t('Note types');
      $this->value_options = _iwapi_extract_field($entities) + $names;
    }
  }
}
