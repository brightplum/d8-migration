<?php
/**
 * @file
 * Integrate the xmlsitemap module.
 */

/**
 * Implements hook_entity_info_alter().
 */
function cdreams_profile_entity_info_alter(array &$entity_info) {
  $entity_info['profile']['label'] = t('Profile');
  $entity_info['profile']['bundle label'] = t('Profile Type');
  $entity_info['profile']['xmlsitemap'] = array(
    'process callback' => 'cdreams_profile_xmlsitemap_process_profile_links',
  );
}

/**
 * Implements hook_cron().
 *
 * Process old profiles not found in the {xmlsitemap} table.
 */
function cdreams_profile_cron() {
  cdreams_profile_xmlsitemap_index_links(xmlsitemap_var('batch_limit'));
}

/**
 * Implements hook_xmlsitemap_index_links().
 */
function cdreams_profile_xmlsitemap_index_links($limit) {
  if ($types = cdreams_profile_type_load()) {
    $types = array_keys($types);
    foreach ($types as $key => $type) {
      if ($type == 'source') {
        unset($types[$key]);
      }
    }
    $pids = db_query_range("SELECT p.profile_id FROM {cdreams_profiles} p LEFT JOIN {xmlsitemap} x ON x.type = 'profile' AND p.profile_id = x.id WHERE x.id IS NULL AND p.type IN (:types) ORDER BY p.profile_id DESC", 0, $limit, array(':types' => $types))->fetchCol();
    cdreams_profile_xmlsitemap_process_profile_links($pids);
  }
}

/**
 * Process profile sitemap links.
 *
 * @param $nids
 *   An array of profile IDs.
 */
function cdreams_profile_xmlsitemap_process_profile_links(array $pids) {
  $profiles = entity_load('cdreams_profile', $pids);
  foreach ($profiles as $profile) {
    if ($profile->type != 'source') {
      $link = cdreams_profile_create_link($profile);
      xmlsitemap_link_save($link, array($link['type'] => $profile));
    }
  }
}

/**
 * Implements hook_entity_insert().
 */
/**
 * Implements hook_entity_insert().
 */
function cdreams_profile_entity_insert($entity, $type) {
  if ($type == 'cdreams_profile') {
    if ($entity->type != 'source') {
      cdreams_profile_entity_update($entity, $type);
    }
  }
}

/**
 * Implements hook_entity_update().
 */
/**
 * Implements hook_entity_update().
 */
function cdreams_profile_entity_update($entity, $type) {
  if ($type == 'cdreams_profile') {
    if ($entity->type != 'source') {
      $link = cdreams_profile_create_link($entity);
      xmlsitemap_link_save($link, array($link['type'] => $entity));
    }
  }
}

/**
 * Implements hook_profile_delete().
 */
/**
 * Implements hook_entity_delete().
 */
function cdreams_profile_entity_delete($entity, $type) {
  if ($type == 'cdreams_profile') {
    xmlsitemap_link_delete('profile', $entity->nid);
  }
}

/**
 * Create a sitemap link from a profile.
 *
 * The link will be saved as $profile->xmlsitemap.
 *
 * @param $profile
 *   A profile object.
 */
function cdreams_profile_create_link($profile) {
  if (!isset($profile->xmlsitemap) || !is_array($profile->xmlsitemap)) {
    $profile->xmlsitemap = array();
    if ($profile->profile_id && $link = xmlsitemap_link_load('profile', $profile->profile_id)) {
      $profile->xmlsitemap = $link;
    }
  }

  $settings = xmlsitemap_link_bundle_load('profile', $profile->type);
  $uri = entity_uri('cdreams_profile', $profile);
  $path = drupal_get_path_alias($uri['path']);

  $profile->xmlsitemap += array(
    'type' => 'profile',
    'id' => $profile->profile_id,
    'subtype' => $profile->type,
    'status' => 1,
    'status_default' => 1,
    'status_override' => 0,
    'priority' => $settings['priority'],
    'priority_default' => $settings['priority'],
    'priority_override' => 0,
  );

  // Always recalculate changefreq and changecount.
  $timestamps = [];
  $profile->xmlsitemap['changefreq'] = 0;
  $profile->xmlsitemap['changecount'] = 0;

  // The following values must always be checked because they are volatile.
  $profile->xmlsitemap['loc'] = $path;
  $profile->xmlsitemap['lastmod'] = count($timestamps) ? max($timestamps) : 0;
  $profile->xmlsitemap['access'] = 1;
  $profile->xmlsitemap['language'] = isset($profile->language) ? $profile->language : LANGUAGE_NONE;

  return $profile->xmlsitemap;
}