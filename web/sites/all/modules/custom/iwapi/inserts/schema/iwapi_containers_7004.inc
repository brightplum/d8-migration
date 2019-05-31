<?php

$schema['iwapi_containers'] = array(
  'description' => 'Maintains format container settings.',
  'fields' => array(
    'scope' => array(
      'type' => 'varchar',
      'length' => 128,
      'not null' => TRUE,
      'description' => 'The scope that this container setting applies to.',
    ),
    'name' => array(
      'type' => 'varchar',
      'length' => 64,
      'not null' => TRUE,
    ),
    'label' => array(
      'type' => 'varchar',
      'length' => 255,
      'not null' => TRUE,
      'default' => '',
    ),
    'class_name' => array(
      'type' => 'varchar',
      'length' => 32,
      'not null' => TRUE,
      'description' => 'The container type, may dictate handler class also.',
    ),
    'weight' => array(
      'type' => 'int',
      'not null' => TRUE,
      'default' => 0,
    ),
    'data' => array(
      'type' => 'blob',
      'serialize' => TRUE,
      'description' => 'Serialize settings for this container.',
    ),
  ),
  'primary key' => array('scope', 'name'),
);
