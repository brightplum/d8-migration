<?php
/**
 * @file
 * Contains batch processing items for Drupal to handle large tasks.
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
function cdreams_profile_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {    
    $bundles = array();
    foreach (cdreams_profile_type_load() as $name => $bundle) {
      if ($bundle->has_page) {
        $bundles[] = $name;
      }
    }
    
    $context['sandbox']['count']   = 0;
    $context['sandbox']['current'] = 0;
    $context['sandbox']['bundles'] = $bundles;
  }
  

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $context['sandbox']['total']   = (int)_cdreams_profile_pathauto_entity_update_count($bundles);

    // If there are no nodes to update, the stop immediately.
    if ($context['sandbox']['total'] <= 0) {
      $context['finished'] = 1;
      return;
    }
  }

  // First build a query to fetch the items which need to have a alias generated.
  $query = db_select('cdreams_profiles', 'ent')
    ->addTag('pathauto_bulk_update')
    ->addMetaData('entity', 'cdreams_profile')
    ->condition('ent.profile_id', $context['sandbox']['current'], '>')
    ->condition('ent.type', $context['sandbox']['bundles'])
    ->isNull('ua.source')
    ->orderBy('ent.profile_id', 'ASC')
    ->range(0, 100);
  
  $query->leftJoin('url_alias', 'ua', "CONCAT('profile/nojs/', ent.profile_id) = ua.source");
  $query->addField('ent', 'profile_id');

  $ids = $query->execute()->fetchCol();
  if (empty($ids)) {
    $context['finished'] = 1;
    return;
  }

  // Generate the new aliases.
  module_load_include('inc', 'pathauto');  
  foreach (entity_load('cdreams_profile', $ids) as $entity) {
    if (pathauto_pattern_load_by_entity('cdreams_profile', $entity->getBundle())) {
      $uri  = $entity->getUri();
      $data = array('cdreams_profile' => $entity);
      pathauto_create_alias('cdreams_profile', 'bulkupdate', $uri['path'], $data, $entity->getBundle());
    }
    
    $context['sandbox']['current'] = $entity->getId();
  }

  $context['sandbox']['count'] += count($ids);
  if ($context['sandbox']['count'] < $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  };
}

/**
 * Get the number of items remaining for a bulk alias batch function.
 *
 * @param array $bundles
 *  Indicate filters of bundles to include.
 * @return number
 *  Number of items left to alias.
 */
function _cdreams_profile_pathauto_entity_update_count($bundles) {
  $query = db_select('cdreams_profiles', 'ent')
    ->addTag('pathauto_bulk_update')
    ->addMetaData('entity', 'cdreams_profile')
    ->condition('type', $bundles);

  $query->leftJoin('url_alias', 'ua', "CONCAT('profile/nojs/', ent.profile_id) = ua.source");
  $query->isNull('ua.source');

  return $query->countQuery()->execute()->fetchField();
}
