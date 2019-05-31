<?php
/**
 * @file elements/iwKeywordSearchElement.inc
 * Contains the keyword search element.
 */

/**
 * The keyword search element is special, in that it uses 
 * an existing index (I.E. Solr or {search_dataset} table).
 * 
 * Thus it doesn't implement or handle much other than attachForm()
 * and default display configurations.
 */
class iwSearchKeyword extends iwSearchElement {

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'keywords';
  }
   
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::setFieldSchema()
   */
  function setElementSchema(array &$schema) {
    // Keyword searches uses an existing index.
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::indexElement()
   */
  function indexElement($entity, array &$data) {
    // Keyword searches use an existing index.
  }


  // ================================
  // Search form
  // ================================
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
    $element = array(
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => $default,
    );
    
    // Construct the title based on display settings.
    $this->setElementTitle($element);
    return $element;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::translateValue()
   */
  function translateValue($value) {
    return empty($value) ? NULL : $value;    
  }
}
