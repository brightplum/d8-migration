<?php


class csp_views_handler_filter_entity_type extends views_handler_filter_in_operator {
  
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Entity types');
      
      $types = array('node' => 'node') + _iwapi_get_api_entity_types();
      $include = array_intersect_key(entity_get_info(), $types);
      
      $options = _iwapi_extract_field($include);
      asort($options);
      
      $this->value_options = $options;
    }
  }
}
