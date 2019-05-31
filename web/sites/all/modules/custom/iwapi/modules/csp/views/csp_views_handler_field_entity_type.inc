<?php


class csp_views_handler_field_entity_type extends views_handler_field {

  function render($values) {
    $type = $this->get_value($values);
    
    $info = entity_get_info($type);
    return empty($info) ? t('Unknown') : $this->sanitize_value($info['label']);
  }

}