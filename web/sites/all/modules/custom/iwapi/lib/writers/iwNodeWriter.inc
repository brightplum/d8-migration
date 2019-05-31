<?php
/**
 * @file iwWriterNode.inc
 * Writes simple node data, and creates node items.
 */

/**
 * Base node writer, which writes node data.
 */
class iwNodeWriter extends iwEntityWriter {
  const ENTITY_TYPE = 'node';
  
  /**
   * Report back the customizable writer settings
   * which are available.
   * 
   * @return array
   *  Return the configurable package properties for the
   *  node writer.
   */
  static function getPackageProps() {
    return array(
      'bundle'  => array('label' => t('Entity bundle')),
      'status'  => array('label' => t('Import node as published')),
      'promote' => array('label' => t('Import node as promoted')),
    );
  }
  
  
  // ----------------------------------
  // Instance variables
  // ----------------------------------
  
  protected $status   = NULL;
  protected $promoted = NULL;

  
  // ----------------------------------
  // Instance methods
  // ----------------------------------

  /**
   * Create a node writer that is compatible with a specific node type.
   *
   * @param $nodeType
   *  String representing the node type that we attempt to build
   *  a writer for.
   * @param $mode
   *  The writing mode used by this writer. Determines how existing
   *  entries are updated or inserted.
   *
   * @throws iwException
   */
  function __construct(array $values = array()) {
    parent::__construct($values);
    
    $bundle_info = node_type_get_type($this->bundle);
    $this->required['type'] = 'type';
    
    // If this node reports that it has a title, require it.
    if ($bundle_info->has_title) {
      $this->allowed['title']  = 'title';
      $this->required['title'] = 'title';
    }
    
    $this->allowed['nid']     = 'nid';
    $this->allowed['status']  = 'status';
    $this->allowed['promote'] = 'promote';
    $this->allowed['sticky']  = 'sticky';
    $this->allowed['created'] = 'created';
    $this->allowed['changed'] = 'changed';
  }

  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    $nid = !empty($data['nid']) ? intval($data['nid']) : 0;

    if ($nid > 0 ) {
      if (iwWriter::ALLOW_UPDATE & $this->mode) {
        $node = node_load($nid);
      }

      if (empty($node)) {
        drupal_set_message(t(
          'Node ID @nid was provided but unable to update the node: @msg', array(
            '@nid' => $nid,
            '@msg' => (iwWriter::ALLOW_UPDATE & $this->mode) ? 'Updates not allowed' : 'Node not available',
          )
        ));
        return FALSE;
      }
    }
    elseif ($this->mode & iwWriter::ALLOW_INSERT) {
      $node           = new stdClass();
      $node->language = LANGUAGE_NONE;
    
      if (!empty($this->bundle)) {
        $data['type'] = $this->bundle;
      }
      
      if ($error = $this->validate($data)) {
        drupal_set_message($error, 'error');
        return FALSE;
      }
    }
    else {
      drupal_set_message(t(
        'Unable to save new node: @title',
        array('@title' => $data['title'])
      ));
      return FALSE;
    }
    
    $this->saveNode($node, $data);
    $data['nid'] = $node->nid;
  }

  
  // --------------------------
  // Helper / Utilities
  // --------------------------
  
  /**
   * 
   * @param type $node
   */
  function saveNode($node, $data) {
    // ----------------------------
    // Apply all the data fields.
    $pass_thru = array(
      'title', 'type', 'status', 'promote',
      'sticky', 'created', 'changed',
    );
    
    foreach ($pass_thru as $extra) {
      if (isset($this->{$extra})) {
        $node->{$extra} = $this->{$extra};
      }
      elseif (isset($data[$extra])) {
        $node->{$extra} = $data[$extra];
      }
    }
    
    $node->title = trim($node->title);
    $this->applyFields($node, $data);
  
    node_object_prepare($node);
    node_save($node);
    
    if (!empty($node->nid) && !(empty($data['created']) && empty($data['changed']))) {
      $allowed = array_fill_keys(array('created', 'changed'), TRUE);

      db_update('node')
        ->fields(array_intersect_key($data, $allowed))
        ->condition('nid', $node->nid)
        ->execute();
    }
  }
}
