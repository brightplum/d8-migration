<?php


class csp_views_handler_field_workflow extends views_handler_field {
  
  function render($values) {
    $value    = $this->get_value($values);
    $workflow = CspController::Load(intval($value));
    
    // Get the label of this workflow.
    return $this->sanitize_value($workflow->label);
  }
}
