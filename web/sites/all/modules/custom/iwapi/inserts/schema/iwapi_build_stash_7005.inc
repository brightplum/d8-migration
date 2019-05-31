<?php
/**
 * @file
 * Contains the definition of prebuilt items caches.
 */


$schema['iwapi_build_stash'] = array(
  'description' => 'Storage for items that are being built (commonly in forms).',
  'fields' => array(
    'type' => array(
      'type' => 'varchar',
      'length' => 64,
      'not null' => TRUE,
      'description' => 'The type of object stored.',
    ),
    'id' => array(
      'type' => 'varchar',
      'length' => 64,
      'not null' => TRUE,
      'description' => 'The id, should be unique for the type.',
    ),
    'uid' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
      'description' => 'Who updated / generated this object.',
    ),
    'changed' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
      'description' => 'Timestamp of last recorded change.',
    ),
    'data' => array(
      'type' => 'blob',
      'size' => 'normal',
      'serialized' => TRUE,
      'description' => 'Serialized object being stashed & built.',
    ),
  ),
  'primary' => array('type', 'id'),
);
