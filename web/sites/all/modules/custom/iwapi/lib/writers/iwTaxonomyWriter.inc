<?php
/**
 * @file iwWriterNode.inc
 * Writes simple node data, and creates node items.
 */

/**
 * Base node writer, which writes node data.
 */
class iwTaxonomyWriter extends iwEntityWriter {
  const ENTITY_TYPE = 'taxonomy_term';
  
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
      'mode' => array('label' => t('Write mode')),
      'vocab' => array('label' => t('Vocabulary')),
      'status'  => array('label' => t('Import node as published')),
      'promote' => array('label' => t('Import node as promoted')),
    );
  }


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
    $this->required['vid'] = 'vid';
    
    $this->allowed['tid']  = 'tid';
    $this->allowed['name'] = 'name';
    $this->allowed['description'] ='description';
  }

  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
   */
  function save(array &$data) {
    $tid = !empty($data['tid']) ? intval($data['tid']) : 0;

    if ($tid > 0 ) {
      if (iwWriter::ALLOW_UPDATE & $this->mode) {
        $term = taxonomy_term_load($tid);
      }

      if (empty($term)) {
        drupal_set_message(t(
          'Term ID @tid was provided but unable to update the term: @msg', array(
            '@tid' => $tid,
            '@msg' => (iwWriter::ALLOW_UPDATE & $this->mode) ? 'Term not available' : 'Updates not allowed',
          )
        ));
        return FALSE;
      }
    }
    else if ($this->mode & iwWriter::ALLOW_INSERT) {
      $term = new stdClass();

      if ($error = $this->validate($data)) {
        drupal_set_message($error, 'error');
        return FALSE;
      }
    }
    else {
      drupal_set_message(t(
        'Unable to save new Term: @title',
        array('@title' => $data['name'])
      ));
      return FALSE;
    }

    // Apply all the data fields.
    $pass_thru = array('vid', 'name', 'description');
    foreach ($pass_thru as $extra) {
      if (!empty($this->{$extra})) {
        $term->{$extra} = $this->{$extra};
      }
      elseif (!empty($data[$extra])) {
        $term->{$extra} = $data[$extra];
      }
    }
    $this->applyFields($term, $data);

    taxonomy_term_save($term); // save the taxonomy term.
  }
}
