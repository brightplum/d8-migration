<?php
/**
 * @file
 * Include specialized functionality altering queries with CSP control.
 */


/**
 * Alters a query to filter query to ensure proper
 * handling of CSP states and permissions.
 * 
 * @param QueryAlterableInterface $query
 *  The query to be altered to filter on workflow settings.
 * @param string $entity_type
 *  The machine name of the entity type being worked on.
 */
function _csp_query_entity_access_alter(QueryAlterableInterface $query, $entity_type) {
  global $user;

  // Read meta-data from query, if provided.
  if (!$account = $query->getMetaData('account')) {
    $account = $user;
  }
  if (!$op = $query->getMetaData('op')) {
    $op = 'view';
  }

  // Administrators have no additional filters from this module
  if (user_access(CSP_PERM_ADMIN, $account)) {
    return;
  }
  
  $cspInfo  = CspController::GetEntityVariable($entity_type);
  if (empty($cspInfo)) {
    return;
  }
  
  $flow_ids = call_user_func_array('array_merge', $cspInfo);
  if (empty($flow_ids)) {
    return;
  }  
  
  $entity_info = entity_get_info($entity_type);
  $keys        = $entity_info['entity keys'];
  $statusKey   = empty($keys['status']) ? 'status' : $keys['status'];

  $baseAlias = '';
  if ($query instanceof SelectQuery) {
    foreach ($query->getTables() as $alias => $table_info) {
      if ($table_info['table'] == $entity_info['base table']) {
        $baseAlias = $alias;
        break;
      }
    }
  }
  
  /*
  // TODO: Alter query to respect workflow states and permissions.
  // CSP only actively denies / filters, it should never add.
  $props = array(
    "{$baseAlias}.{$keys[bundle]}" => array(),
    "{$baseAlias}.{$statusKey}" => array(),
  );
  
  // TODO: We need to recursively descend and find 
  // bundle / or publish state, and add filters accordingly.
  //_csp_find_query_conditions($query->conditions(), $props);
  */
}

/**
 * TODO: Alter this function to change the query as needed
 * to filter only items that need CSP altered permissioning.
 */
function _csp_find_query_conditions($conditions, array &$fields) {  
  unset($conditions['#conjunction']); // How do we handle different conjunctions?
  
  foreach ($conditions as $cond) {
    if ($cond['field'] instanceof DatabaseCondition) {
      _csp_find_query_conditions($cond['field']->conditions(), $fields);
    }
    elseif (key_exists($cond['field'], $fields)) {

      // TODO: How do we combine the fields information into the reporting array.
      if (is_array($cond['value'])) {
        $fields[$cond['field']] = array_merge($fields[$cond['field']], $cond['value']);
      }
      else {
        $fields[$cond['field']][] = $cond['value'];
      }
    }
  }
}
