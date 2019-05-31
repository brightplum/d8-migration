<?php
/**
 * @file
 * Contains information about additional entity properties aside from fields.
 */


$default_properties = array(
  'node' => array(
    'title' => array(
      'label' => t('Node title'),
      'handler' => 'iwLabelProperty',
    // Implicit because searches already handles titles
    // 'index' => 'string', 
    ),
    'type' => array(
      'label' => t('Node type'),
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
  ),
);
