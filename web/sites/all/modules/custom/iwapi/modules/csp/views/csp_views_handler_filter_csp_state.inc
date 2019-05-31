<?php

/**
 * Basic textfield filter to handle string filtering commands
 * including equality, like, not like, etc.
 *
 * @ingroup views_filter_handlers
 */
class csp_views_handler_filter_csp_state extends views_handler_filter {
	
  function init(&$view, &$options) {
    parent::init($view, $options);
    
    // An array of workflows ID's for workflows that should be included.
    if (!empty($this->definition['csp_flows'])) {
      $this->options['csp_flows'] = $this->definition['csp_flows'];
    }
  }
  
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $options = $this->operator_options('short');
    $output = '';
    if(!empty($options[$this->operator])) {
      $output = check_plain($options[$this->operator]);
    }
    if (in_array($this->operator, $this->operator_values(1))) {
      $output .= ' ' . check_plain($this->value);
    }

    return $output;
  }

  
  // ----------------------------
  // Options handling
  // ----------------------------
  
  function option_definition() {
    $options = parent::option_definition();
    
    $options['csp_flows'] = array('default' => array());
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $workflows = CspController::LoadAll();
    $form['csp_flows'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Use ONLY these workflows'),
      '#options' => _iwapi_extract_field($workflows),
      '#default_value' => $this->options['csp_flows'],
    );
  }
  
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);
    
    $form_state['values']['csp_flows'] = array_filter($form_state['values']['csp_flows']);
    $this->options['csp_flows'] = $form_state['values']['csp_flows'];
  }
  
  
  // ----------------------------
  // Operator handling
  // ----------------------------

  /**
   * This kind of construct makes it relatively easy for a child class
   * to add or remove functionality by overriding this function and
   * adding/removing items from this array.
   */
  function operators() {
    $operators = array(
      '=' => array(
        'title' => t('Is equal to'),
        'short' => t('='),
        'method' => 'op_equal',
        'values' => 1,
      ),
      '!=' => array(
        'title' => t('Is not equal to'),
        'short' => t('!='),
        'method' => 'op_equal',
        'values' => 1,
      ),
      'empty' => array(
        'title' => t('has no state'),
        'short' => t('empty'),
        'method' => 'op_empty',
        'values' => 0,
      ),
      'not empty' => array(
        'title' => t('has a state set'),
        'short' => t('not empty'),
        'method' => 'op_empty',
        'values' => 0,
      ),
    );
    return $operators;
  }

  function operator_values($values = 1) {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      if (isset($info['values']) && $info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }
  
  /**
   * Build strings from the operators() for 'select' options
   */
  function operator_options($which = 'title') {
    $operators = $this->operators();
    array_walk($operators, '_iwapi_extract_array_field', $which);
    
    return $operators;
  }

  function op_equal($field) {
    $values = is_array($this->value) ? $this->value : array($this->value);
    
    // Organize the states by their respective workflows.
    // This will generate a cleaner where clause.
    $byFlow = array();
    foreach ($values as $value) {
      if (preg_match('/(\d+)\[([a-z,0-9,_]+)\]/i', $value, $matches)) {
        $byFlow[$matches[1]][] = $matches[2];
      }
    }

    // Add the condition to the filter.
    $flow_field = "$this->table_alias.flow";
    foreach($byFlow as $flow_id => $states) {
      $cond = ($this->operator === '!=')
        ? db_or()->condition($field, $states, 'NOT IN')
        : db_and()->condition($field, $states, 'IN');

      $cond->condition($flow_field, $flow_id, $this->operator);
      $this->query->add_where($this->options['group'], $cond);
    }
  }

  function op_empty($field) {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }
    
    $this->query->add_where($this->options['group'], $field, NULL, $operator);
  }

  
  // --------------------------
  // Set state value
  // --------------------------
  
  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {    
    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator_id'])) {
        $which = in_array($this->operator, $this->operator_values(1)) ? 'value' : 'none';
      }
      else {
        $source = 'edit-' . drupal_html_id($this->options['expose']['operator_id']);
      }
    }

    if ($which == 'all' || $which == 'value') {
      $state_opts = array();
      
      if (count($this->options['csp_flows']) === 1) {
        $workflow = CspController::Load(reset($this->options['csp_flows']));
        
        foreach ($workflow->states as $state => $info) {
          $state_opts["{$workflow->flow}[{$state}]"] = $info['label'];
        }
      }
      else {
        $workflows = CspController::LoadMultiple($this->options['csp_flows']);
        
        foreach ($workflows as $flow => $workflow) {
          foreach ($workflow->states as $state => $info) {
            $state_opts["{$workflow->flow}[{$state}]"] = "$workflow->label: $info[label]";
          }
        }
      }
      
      $form['value'] = array(
        '#type' => 'select',
        '#title' => t('Value'),
        '#options' => $state_opts,
        '#default_value' => $this->value,
        '#weight' => -15,
      );

      if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }

      if ($which == 'all') {
        $form['value'] += array(
          '#dependency' => array($source => $this->operator_values(1)),
        );
      }
    }

    if (!isset($form['value'])) {
      // Ensure there is something in the 'value'.
      $form['value'] = array(
        '#type' => 'value',
        '#value' => NULL,
      );
    }
  }

  /**
   * Add this filter to the query.
   *
   * Due to the nature of fapi, the value and the operator have an unintended
   * level of indirection. You will find them in $this->operator
   * and $this->value respectively.
   */
  function query() {
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";

    $info = $this->operators();    
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }
}
