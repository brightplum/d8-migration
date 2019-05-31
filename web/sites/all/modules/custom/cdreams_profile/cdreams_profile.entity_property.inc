<?php
/**
 * @file
 * Contains the entity properties HOOKs and callbacks.
 */


/**
 * Implements HOOK_iwapi_entity_properties()
 */
function cdreams_profile_iwapi_entity_properties() {
  $properties['cdreams_profile'] = array(
    'name' => array(
      'label' => t('Profile name'),
      'handler' => 'iwLabelProperty',
    // Implicit because searches already handles titles
    // 'index' => 'string', 
    ),
    'type' => array(
      'label' => t('Profile type'),
      'handler' => 'iwBundleProperty',
    // Implicit because most searches already handle the bundle / type
    // 'index' => 'string',
    ),
    'author' => array(
      'label' => t('Author name'),
      'handler' => 'iwAuthorProperty',
      'index' => 'string',
    ),
    'created' => array(
      'label' => t('Creation date'),
      'handler' => 'iwTimestampProperty',
      'index' => 'date',
    ),
    'changed' => array(
      'label' => t('Modification date'),
      'handler' => 'iwTimestampProperty',
    ),
    'content_link' => array(
      'label' => t('Link to content'),
      'handler' => 'iwContentLinkProperty',
    ),
  );
  
  return $properties;
}
