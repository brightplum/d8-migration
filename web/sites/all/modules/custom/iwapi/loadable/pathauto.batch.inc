<?php
/**
 * @file includes/pathauto.batches.inc
 * Path auto batch helper functions for uniformly generating path aliases for iwEntity based entities.
 */


/**
 * Generate aliases for the provided entity type, with
 * entity ID greater than $current_id.
 *
 * @param string $entity_type
 *  Type of entity to find aliases for.
 * @param string $url_base
 *  The base url path string.
 * @param int $current_id
 *  Reference to the current position of the aliasing.
 * @return number
 *  Count of items that have been updated this round.
 */
function _iwapi_pathauto_entity_bulk_update_aliases($entity_type, $url_base, &$current_id) {
  // First build a query to fetch the items which need to have a alias generated.
  $entity_info = entity_get_info($entity_type);
  $keys = $entity_info['entity keys'];

  $query = db_select($entity_info['base table'], 'ent');
  $query->leftJoin('url_alias', 'ua', "CONCAT('{$url_base}', ent.{$keys['id']}) = ua.source");
  $query->addField('ent', $keys['id']);
  $query->isNull('ua.source');
  $query->condition("ent.{$keys['id']}", $current_id, '>');
  $query->orderBy("ent.{$keys['id']}");
  $query->addTag('pathauto_bulk_update');
  $query->addMetaData('entity', $entity_type);

  $query->range(0, 25);
  $ids = $query->execute()->fetchCol();

  // Generate the new aliases.
  module_load_include('inc', 'pathauto');
  $entities = entity_load($entity_type, $ids);
  foreach ($entities as $entity) {
    if (pathauto_pattern_load_by_entity($entity_type, $entity->getBundle())) {
      $uri = $entity->getUri();
      $data = array($entity_type => $entity);
      pathauto_create_alias($entity_type, 'bulkupdate', $uri['path'], $data, $entity->getBundle());
    }
  }
  $current_id = max($ids);

  return count($ids);
}

/**
 * Get the number of items remaining for a bulk alias batch function.
 *
 * @param string $entity_type
 *  Type of entity to find aliases for.
 * @param string $url_base
 *  The base url path string.
 * @return number
 *  Number of items left to alias.
 */
function _iwapi_pathauto_entity_update_count($entity_type, $url_base) {
  // First build a query to fetch the items which need to have a alias generated.
  $entity_info = entity_get_info($entity_type);
  $keys = $entity_info['entity keys'];

  $query = db_select($entity_info['base table'], 'ent');
  $query->addTag('pathauto_bulk_update');
  $query->addMetaData('entity', $entity_type);
  $query->leftJoin('url_alias', 'ua', "CONCAT('{$url_base}', ent.{$keys['id']}) = ua.source");
  $query->isNull('ua.source');

  return $query->countQuery()->execute()->fetchField();
}
