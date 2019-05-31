<?php
/**
 * @file
 * Contains the field handler to deal with simple boolean values.
 */

class iwSearchBoolean extends iwSearchFieldElement {
  
  /**
   * non-PHPDOC
   * @see iwSearchValue::getValueType().
   */
  function getValueType() {
    return 'bool';
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::appendElements().
   */
  function appendElements(array &$elements, array &$form, array &$form_state) {
    parent::appendElements($elements, $form, $form_state);
  }
  
  
  // ---------------------------------------
  // Implement iwSearchElement
  // ---------------------------------------
  
  /**
   * non-PHPDOC
   * @see iwSearchElement::filterString().
   */
  function filterString($value = NULL) {
    if (!empty($value)) {
      return '<span class="filter-item">'. check_plain($this->label) .'</span>';
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
    $element = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($default),
    );
    
    $this->setElementTitle($element);
    return $element;
  }
  
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::translateValue()
   */
  function translateValue($value = NULL) {
    if (!empty($value)) {
      return intval((bool)$value);
    }
  }
}
