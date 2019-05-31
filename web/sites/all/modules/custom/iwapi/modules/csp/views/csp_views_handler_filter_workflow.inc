<?php

class csp_views_handler_filter_workflow extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('CSP Workflow');

      $this->value_options = db_select('csp_flows', 'fl')
        ->fields('fl', array('flow', 'label'))
        ->orderBy('label', 'ASC')
        ->execute()->fetchAllKeyed();

      return $this->value_options;
    }
  }
}
