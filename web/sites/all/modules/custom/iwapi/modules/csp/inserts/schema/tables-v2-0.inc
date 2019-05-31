<?php
/**
 * @file
 * Table definitions for the initial 2.0 version of CSP.
 */

$schema['csp_flows'] = array(
  'description' => 'Contains definitions CSP workflows.',
  'fields' => array(
    'flow' => array(
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'description' => 'Unique identifier for this workflow.',
    ),
    'name' => array(
      'type' => 'varchar',
      'length' => 32,
      'not null' => TRUE,
      'description' => 'Machine name for consistent identification.',
    ),
    'label' => array(
      'type' => 'varchar',
      'length' => 255,
      'not null' => TRUE,
      'default' => '',
      'description' => 'Friendly display name for this workflow.',
    ),
    'publishes' => array(
      'type' => 'int',
      'size' => 'tiny',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
    ),
    'handler' => array(
      'type' => 'varchar',
      'length' => 128,
      'not null' => TRUE,
      'default' => 'CspFlow',
    ),
    'data' => array(
      'type' => 'blob',
      'serialize' => TRUE,
      'description' => 'Contains serialized state management information.',
    ),
  ),
  'primary key' => array('flow'),
  'unique keys' => array('name' => array('name')),
);

// List the administrators to mail / maintain.
$schema['csp_admins'] = array(
  'description' => 'Associates users to workflows they maintain.',
  'fields' => array(
    'flow' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ),
    'uid' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ),
  ),
  'primary key' => array('flow', 'uid'),
  'foreign keys' => array(
    'workflow' => array(
      'table' => 'csp_flows',
      'columns' => array('flow' => 'flow'),
    ),
    'admin' => array(
      'table' => 'users',
      'columns' => array('uid' => 'uid'),
    ),
  ),
);

// Main content relation table
$schema['csp_entities'] = array(
  'description' => 'Contains the entity CSP state',
  'fields' => array(
    'state_id' => array(
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ),
    'entity_type' => array(
      'type' => 'varchar',
      'length' => 64,
      'not null' => TRUE,
      'description' => 'The type of entity that this status belongs to.',
    ),
    'entity_id' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'description' => 'Reference to ID of the entity the state belongs to.',
    ),
    'flow' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'description' => 'Reference to the {csp_flows}.flow table.',
    ),
    'rev' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'description' => 'Reference to the revision of the entity if there is one.',
    ),
    'state' => array(
      'type' => 'varchar',
      'length' => 32,
      'not null' => TRUE,
      'description' => 'The current CSP state of this entity.',
    ),
    'changed' => array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
    ),
  ),
  'primary key' => array('state_id'),
  'unique keys' => array(
    'entity' => array('entity_type', 'entity_id', 'flow', 'rev'),
  ),
  'indexes' => array(
    'entity_state' => array('flow', 'state'),
    'change_date'  => array('changed'),
  ),
  'foreign keys' => array(
    'flows' => array(
      'table' => 'csp_flows',
      'columns' => array('flow' => 'flow'),
    ),
  ),
);
