<?php

/**
 * A handler to run a field through check_markup, using a companion
 * format field.
 *
 * - format: (REQUIRED) Either a string format id to use for this field or an
 *           array('field' => {$field}) where $field is the field in this table
 *           used to control the format such as the 'format' field in the node,
 *           which goes with the 'body' field.
 *
 * @ingroup views_field_handlers
 */
class csp_views_handler_field_csp_state extends views_handler_field {

  /**
   * Constructor; calls to base object constructor.
   */
  function construct() {
    parent::construct();

    $this->additional_fields['flow'] = 'flow';
  }

  function render($values) {
    $value = $this->get_value($values);
    $workflow = CspController::Load($this->get_value($values, 'flow'));
    
    if ($workflow) {
      $state = isset($workflow->states[$value])
        ? $workflow->states[$value] : reset($workflow->states);
      
      return $this->sanitize_value($state['label']);
    }
    
    return t('Unknown state');
  }
}
