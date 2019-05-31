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
class iwapi_views_handler_filter_entity_bundle extends views_handler_filter_in_operator {

  function get_value_options() {
    $table_data = views_fetch_data($this->view->base_table);
    $this->entityType = $table_data['table']['entity type'];

    if (!isset($this->value_options)) {
      $info = entity_get_info($this->entityType);
      $options = _iwapi_extract_field($info['bundles']);
      asort($options);

      $this->value_title = t('@entity types', array('@entity' => $info['label']));
      $this->value_options = $options;
    }
  }
}
