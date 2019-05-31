<?php
/**
 * @file csp.tokens.inc
 *  Describe and create tokens for various uses.
 */

/**
 * Implements Drupal HOOK_token_info()
 */
function csp_token_info() {
  $tokens['types']['csp'] = array(
    'name' => t('CSP Workflow'),
    'description' => t('Workflow used to manage publishing.'),
    'needs-data' => 'csp',
  );
  
  $tokens['tokens']['csp'] = array(
    'flow' => array(
      'name' => t('Workflow ID'),
      'description' => t('Unique key for the workflow'),
    ),
    'label' => array(
      'name' => t('Workflow label'),
      'description' => t('User friendly name of the workflow'),
    ),
    'name' => array(
      'name' => t('Workflow name'),
      'description' => t('Machine name of workflow'),
    ),
    'admin-names' => array(
      'type' => 'user',
      'name' => t('Administrators'),
      'description' => t('The administrators names comma separated.'),
    ),
    'mail' => array(
      'name' => t('Adminstrator Emails'),
      'description' => t('Comma separated email list of workflow administrators.'),
    ),
  );
}

/**
 * Add CSP workflow tokens to entities that use a workflow.
 * 
 * @param array $data
 *  The current entity token data.
 */
function csp_token_info_alter(array &$data) {
  $types = array('node' => 'node') + _iwapi_get_api_entity_types();
  
  CspController::LoadAll();
  foreach ($types as $type) {
    $bundles = variable_get("csp_{$type}_bundles", NULL);
    
    if (empty($bundles) || empty($data['types'][$type])) {
      continue;
    }

    $active  = array();
    foreach ($bundles as $flows) {
      $active += array_flip($flows);
    }
    
    // Add information about active workflows to the entity.
    foreach (array_keys($active) as $id) {
      $workflow = CspController::Load($id);
      
      // Add the workflow information field.
      $data['tokens'][$type]["csp-flow-{$workflow->name}"] = array(
        'type' => 'csp',
        'name' => t('CSP Workflow'),
        'description' => t('Workflow information for @label', array('@label' => $workflow->label)),
      );
      
      // Add a state token
      $data['tokens'][$type]["csp-state-{$workflow->name}"] = array(
        'name' => t('Workflow State'),
        'description' => t('Entity state for the @label workflow', array('@label' => $workflow->label)),
      );
    }
  }
}

/**
 * Implements HOOK_tokens()
 */
function csp_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $sanitize = !empty($options['sanitize']);
  
  $values = array();
  $types = array('node' => 'node') + _iwapi_get_api_entity_types();
  
  if ($type == 'csp' && isset($data['csp'])) {
    $flow = $data['csp'];
    
    foreach ($tokens as $name => $orig) {
      switch ($name) {
        case 'flow':
          $values[$orig] = $flow->flow;
          break;
        case 'label':
          $values[$orig] = $sanitize ? check_plain($flow->label) : $flow->label;
          break;
        case 'name':
          $values[$orig] = $flow->name;
          break;
        case 'admin-names':
          $admins = _csp_get_admins_field($flow, 'name');
          $values[$orig] = $sanitize ? check_plain(implode(',', $admins)) : implode(',', $admins);
          break;
        case 'mail':
          $mail = _csp_get_admins_field($flow, 'mail');
          $values[$orig] = implode(',', $mail);
          break;
      }
    }
  }
  elseif (array_key_exists($type, $types) && !empty($data[$type])) {
    $entity = $data[$type];
    
    $workflows = CspController::EntityWorkflows($entity, $type);
    CspController::LoadEntityStates($entity, $type);
      
    foreach ($workflows as $id => $workflow) {
      if (isset($tokens["csp-state-{$workflow->name}"])) {
        $orig  = $tokens["csp-state-{$workflow->name}"];
        $state = $entity->csp[$id];
        $values[$orig] = $workflow->states[$state]['label'];
 
        if ($sanitize) {
          $values[$orig] = check_plain($values[$orig]);
        }
      }
      elseif ($csp_tokens = token_find_with_prefix($tokens, "csp-flow-{$workflow->name}")) {
        $values += token_generate('csp', $csp_tokens, array('csp' => $workflow), $options);
      }
    }
  }

  return $values;
}