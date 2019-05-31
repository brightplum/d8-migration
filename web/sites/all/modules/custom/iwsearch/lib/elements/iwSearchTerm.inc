<?php
/**
 * @file
 * Manages Taxonomy terms for the iwsearch module.
 */

class iwSearchTerm extends iwSearchAllowedList {
  
  public $termParent = 0; // Root of the heirarchy.
  
  
  // ----------------------------------
  // Instance methods
  // ---------------------------------- 
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'taxonomy_term';
  }
  
  /**
   * Retrieve the term VID for the taxonomy
   * vocabulary to use with this field.
   */
  function getTaxonomyVid() {
    if (!isset($this->vocabId)) {
      $field   = field_info_field($this->fieldName);
      $allowed = reset($field['settings']['allowed_values']);
    
      $vocab = taxonomy_vocabulary_machine_name_load($allowed['vocabulary']);
      $this->vocabId = $vocab->vid;
    }
    
    return $this->vocabId;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchCheckboxes::getAllowedValues()
   */
  function getAllowedValues() {
    if (!isset($this->childTerms)) {
      $query = db_select('taxonomy_term_data', 't');
      $query->join('taxonomy_term_hierarchy', 'h', 'h.tid = t.tid');
      $query->fields('t', array('tid', 'name'));
      $query->condition('h.parent', $this->termParent);
      $query->condition('t.vid', $this->getTaxonomyVid());
      $query->addTag('term_access');
      $query->orderBy('t.weight');
      $query->orderBy('t.name');
      
      $this->childTerms = $query->execute()->fetchAllKeyed();
    }
    
    return $this->childTerms;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getIndexKey()
   */
  function getIndexKey() {
    // Use the taxonomy ID to search with term ID's rather
    // than is_{field_name} returned by apachesolr_index_key().
    return 'im_vid_' . $this->getTaxonomyVid();
  }
  
  
  // ----------------------------------
  // Implements iwIConfigurable
  // ----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $tree = taxonomy_get_tree($this->getTaxonomyVid());
    if (!empty($tree)) {
      $options = array('<' . t('root') . '>');
      
      if (empty($parent)) {
        $parent = array(0);
      }
      foreach ($tree as $item) {
        if (!in_array($item->tid, $exclude)) {
          $options[$item->tid] = str_repeat('-', $item->depth) . $item->name;
        }
      }
    
      $element['termParent'] = array(
        '#type' => 'select',
        '#title' => t('Term parent'),
        '#required' => TRUE,
        '#options' => $options,
        '#default_value' => $this->termParent,
      );
    }
    else {
      $element['termParent'] = array(
        '#type' => 'value',
        '#value' => 0,
      );
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
    
    $this->termParent = $values['termParent'];
  }
}
