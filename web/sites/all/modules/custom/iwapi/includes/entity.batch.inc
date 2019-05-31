<?php
/**
 * @file includes/batches.inc
 * Includes batch functions for entity maintenance.
 */

/**
 * Batch function to delete all student work products
 * of a given product type.
 */
function _iwapi_batch_clear_entity_bundle($entity_type, $bundle, &$context) {
  $entity_info = entity_get_info($entity_type);
  $idKey     = $entity_info['entity keys']['id'];
  $bundleKey = $entity_info['entity keys']['bundle'];
  
  // Start building the query to use for the count query + to get all ids'
  $query = db_select($entity_info['base table'], 'base')
    ->fields('base', array($idKey))
    ->condition("base.{$bundleKey}", $bundle);
  
  // If first time around, gather some statistics, and set progress.
  if (empty($context['sandbox'])) {
    $context['sandbox'] = array(
      'progress' => 0,
      'current'  => 0,
      'max'      => $query->countQuery()->execute()->fetchField(),
    );
    $context['sandbox']['max'];
  }
  $context['message'] = t('Deleting %label ...', array('%label' => $entity_info['label']));
  
  $ids = $query
    ->condition("base.{$idKey}", $context['sandbox']['current'], '>')
    ->orderBy($idKey)
    ->range(0, 50)
    ->execute()->fetchCol();
  
  // Iterate and delete the entity.
  foreach (entity_load($entity_type, $ids) as $entity) {
    $entity->delete();
    $context['sandbox']['current'] = $entity->{$idKey};
    ++$context['sandbox']['progress'];
    
    // Report the message
    $context['results'][] = $entity->{$idKey} .': '. entity_label($entity_type, $entity);
  }
  
  // Update the progress.
  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * The clear entity bundle content completion callback. Reports the results
 * of the entity clearing processing.
 */
function _iwapi_batch_clear_entity_bundle_finished($success, $results, $operations) {
  // The 'success' parameter means no fatal PHP errors were detected. All
  // other error management should be handled using 'results'.
  if ($success) {
    $message = format_plural(count($results), 'One entity deleted.', '@count entities deleted.');
  }
  else {
    $message = t('Finished with an error.');
    
    // Providing data for the redirected page is done through $_SESSION.
    /* TODO: Figure out what to do with this stuff, should we try to 
     * report what was actually deleted? 
    foreach ($results as $result) {
      $items[] = t('Deleted entity %title.', array('%title' => $result));
    }
    $_SESSION['iwapi_clear_entity_bundle_results'] = $items;
    */
  }
  drupal_set_message($message);
}

/**
 * Deletes a series of entities that match criteria
 * passed in by $data. Usually used with queue or batch
 * requests, and will iterate through up to 400 per run.
 * 
 * @param array $data
 *  Contains the criteria and information about what entities
 *  should be deleted during the queued processing.
 */
function _iwapi_queue_entity_clear(array $data) {
  $data += array('joins' => array());
  extract($data);

  if (!($query = _iwapi_build_entity_query($entity_type, $conditions, $joins))) {
    return;
  }
  $query->range(0, 50);
  
  $info    = entity_get_info($entity_type);
  $idKey   = $info['entity keys']['id'];
  $count   = 0;
  $current = 0;
    
  // Loop at least once, and keep going until there are no entities left.
  while (TRUE) {
    $cloned = clone $query;
    $cloned->condition("base.$idKey", $current, '>');
    $ids = $cloned->execute()->fetchCol();
      
    // If not ID's loaded, then we are done
    if (empty($ids)) {
      break;
    }
    
    $entities = entity_load($entity_type, $ids);
    $entity   = reset($entities);
    
    if (method_exists($entity, 'delete')) {
      foreach ($entities as $entity) {
        $entity->delete();    
        $current = $entity->{$idKey};
        ++$count;
      }
    }
    elseif (function_exists($func = $entity_type .'_delete_multiple')) {
      $func(array_keys($entities));
      $last    = end($entities);
      $current = $last->{$idKey};
      $count   += count($entities);
    }
    elseif (function_exists($func = $entity_type .'_delete')) {
      foreach ($entities as $entity) {
        $func($entity->{$idKey});
        $current = $entity->{$idKey};
        ++$count;
      }
    }
  
    // If we've done this many items in one go, re-queue
    // and give someone else a turn. We're also threatened
    // of expiring our run time, and being interupted.
    if ($count >= 300) {
      $queue = DrupalQueue::get('iwapi_entity_clear', TRUE);
      $queue->createItem($data);
      break;
    }
  }
}

/**
 * Generate the base query that will find all entities matching a
 * certain criteria.
 * 
 * @param array $conditions
 *  Array of conditions to use when trying to delete content
 *  of a provided data type.
 * @param array $joins
 *  Additional joins that may be needed in order to help filter
 *  the entities that need to be deleted.
 */
function _iwapi_build_entity_query($entity_type, array $conditions, array $joins = array()) {
  if ($info = entity_get_info($entity_type)) {
    $query = db_select($info['base table'], 'base');
    
    // normalize entity identifier name
    $query->addField('base', $info['entity keys']['id'], 'entity_id');
    
    // Add additional tables if any are needed for the query.
    foreach ($joins as $table => $join) {
      $join += array(
        'type' => 'inner',
        'alias' => $table,
        'args' => array(),
      );
      
      // Add additional tables to the process query.
      $query->addJoin($join['type'], $table, $join['alias'], $join['condition'], $join['args']);
    }
    
    // Add the conditions for 
    foreach ($conditions as $cond) {
      switch ($cond['field']) {
        case 'bundle':
          $field = 'base.'. $info['entity keys']['bundle'];
          $value = $cond['value'];
          $op    = NULL;
          break;
        default:
          $cond += array('alias' => 'base');
          $field = "{$cond['alias']}.{$cond['field']}";
          $value = $cond['value'];
          $op    = isset($cond['op']) ? $cond['op'] : NULL;
      }
      $query->condition($field, $value, $op);
    }
    return $query->orderBy('entity_id');
  }
  return FALSE;
}
