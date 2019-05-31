<?php
/**
 * @file
 * Contains a node writer which tracks information about imported nodes.
 */


class TrackedNodeWriter extends iwNodeWriter {
  
  function __construct(array $values = array()) {
    parent::__construct($values);
    
    $this->allowed['orig_nid'] = 'orig_nid';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    if (!empty($this->bundle)) {
      $data['type'] = $this->bundle;
    }
    
    if ($error = $this->validate($data)) {
      drupal_set_message($error, 'error');
      return FALSE;
    }

    // Prepare and save the node.
    $node           = new stdClass();
    $node->language = LANGUAGE_NONE;
    
    $this->saveNode($node, $data);
    $data['nid'] = $node->nid;
    
    // Save a lookup between the old and new node id.
    db_merge('d6_importer_lookup')
      ->key(array('old' => $data['orig_nid'], 'data_type' => 'node'))
      ->fields(array(
        'data_type' => 'node',
        'old'  => $data['orig_nid'],
        'entity_type' => 'node',
        'id'   => $node->nid,
      ))->execute();
  }
}

