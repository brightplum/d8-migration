<?php
/**
 * @file
 * Contains the field elements handler used to handle numbers.
 */


class iwSearchNumber extends iwSearchFieldElement {

  public $valPrefix;
  public $valSuffix;
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'number';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::formatSummary()
   */
  function formatSummary() {
    $decorators = array();
    if (!empty($this->valPrefix)) {
      $decorators['prefix'] = check_plain($this->valPrefix);
    }
    
    if (!empty($this->valSuffix)) {
      $decorators['suffix'] = check_plain($this->valSuffix);
    }
    
    
    $summary = parent::formatSummary();
    $summary['#properties'] += $decorators;
    return $summary;
  }
  
  // -----------------------------------
  // Implements iwIConfigurable
  // -----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);

    $element['valPrefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Value prefix'),
      '#size' => 10,
      '#default_value' => $this->valPrefix,
    );
    
    $element['valSuffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Value suffix'),
      '#size' => 10,
      '#default_value' => $this->valSuffix,
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
  
    $this->valPrefix  = $values['valPrefix'];
    $this->valSuffix  = $values['valSuffix'];
  }
  
  
  // -----------------------------------
  // Implements iwSearchElement
  // -----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::filterString()
   */
  function filterString($value = NULL) {
    if (isset($value)) {
      $text = check_plain("{$this->valPrefix}$value{$this->valSuffix}");
      return '<span class="filter-item">'. $text .'</span>';
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
    $element = array(
      '#type' => 'textfield',
      '#size' => 15,
      '#field_prefix' => $this->valPrefix,
      '#field_suffix' => $this->valSuffix,
      '#default_value' => $default,
      '#element_validate' => array('element_validate_number'),
    );
    
    $this->setElementTitle($element);
    return $element;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::translateValue()
   */
  function translateValue($value) {
    if (is_string($value) && strlen($value) == 0) {
      return NULL;
    }
    
    $field = field_info_field($this->fieldName);
    return ($field['type'] === 'number_integer') ? intval($value) : doubleval($value);
  }
}
