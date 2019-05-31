<?php
/**
 * @file
 * Contains the definition for the search handler.
 */

class iwSearchDate extends iwSearchFieldElement {

  const RANGE_BETWEEN = 0;
  const RANGE_BEFORE  = 1;
  const RANGE_AFTER   = 2;
    
  public $formType  = '';
  public $rangeType = self::RANGE_BETWEEN;
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'date_range';
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::appendElements().
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $element['rangeType'] = array(
      '#type' => 'select',
      '#title' => t('Filter dates by'),
      '#required' => TRUE,
      '#options' => array(
        self::RANGE_BETWEEN => t('a date range'),
        self::RANGE_BEFORE  => t('before a date'),
        self::RANGE_AFTER   => t('after a date')
      ),
      '#default_value' => $this->rangeType,
    );
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::submitElements().
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
  
    $this->rangeType = $values['rangeType'];
  }
    
  // ---------------------------
  // Implement iwSearchElement
  // ---------------------------

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::filterString()
   */
  function filterString($value = NULL) {
    if (isset($value)) {
      if (!isset($value['min'])) {
        $string = isset($value['max']) ? 'Before '. format_date($value['max']) : 'Any';
      }
      elseif (!isset($value['max'])) {
        $string = 'After '. format_date($value['min']);
      }
      else {
        $string= 'Between '. format_date($value['min']) .' and '. $format_date($values['max']);
      }
      
      return '<span class="filter-item">'. $string .'</span>';
    }
  }
    
  public function attachForm(array &$form_state, $default = NULL) {    
    $element = array(
      '#theme_wrappers' => array('form_element'),
      '#attributes' => array('class' => array('inline-fields')),
        
      0 => array(
        '#type' => 'date_popup',
        '#suffix' => ' TO ',
        '#date_format' => 'm-d-Y',
        '#date_year_range' => '-3:+0',
        '#date_label_position' => 'hidden',
        '#default_value' => isset($default[0]) ? $default[0] : NULL,
        '#description' => '',
      ),
      1 => array(
        '#type' => 'date_popup',
        '#date_format' => 'm-d-Y',
        '#date_year_range' => '-3:+0',
        '#date_label_position' => 'hidden',
        '#default_value' => isset($default[1]) ? $default[1] : NULL,
      ),
    );

    $this->setElementTitle($element);
    return $element;
  }

  /**
   * Translate form values into canonical values for the query
   * builders. They will always expect the same format associated
   * with the declared value type of this element.
   *  
   * @param array $value
   *  The constructed min and max timestamps to use for filtering.
   * @see iwSearchElement::translateValue().
   */
  public function translateValue($value) {
    $value[0] = isset($value[0]) ? trim($value[0]) : '';
    $value[1] = isset($value[1]) ? trim($value[1]) : '';
       
    $pat = '/^\d{4}-\d{2}-\d{2}|^\d{2}-\d{2}-\d{4}/';
    if (!preg_match($pat, $value[0])) {
      $value[0] = '';
    }
    
    if (!preg_match($pat, $value[1])) {
      $value[1] = '';
    }
    
    if (empty($value[0]) && empty($value[1])) {
      return NULL;
    }
    
    return $value;
  }
}


class iwSearchDateValue extends iwSearchElement {

  const RANGE_BETWEEN = 0;
  const RANGE_BEFORE  = 1;
  const RANGE_AFTER   = 2;
    
  public $formType  = '';
  public $rangeType = self::RANGE_BETWEEN;
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'date_range';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getElementSchema()
   */
  function setElementSchema(array &$schema) {
    // Doesn't add to an index field
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::indexElement()
   */
  function indexElement($entity, array &$data) {
    // Doesn't add an indexable value
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::appendElements().
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $element['rangeType'] = array(
      '#type' => 'select',
      '#title' => t('Filter dates by'),
      '#required' => TRUE,
      '#options' => array(
        self::RANGE_BETWEEN => t('a date range'),
        self::RANGE_BEFORE  => t('before a date'),
        self::RANGE_AFTER   => t('after a date')
      ),
      '#default_value' => $this->rangeType,
    );
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::submitElements().
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
  
    $this->rangeType = $values['rangeType'];
  }
    
  // ---------------------------
  // Implement iwSearchElement
  // ---------------------------

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::filterString()
   */
  function filterString($value = NULL) {
    if (isset($value)) {
      if (!isset($value['min'])) {
        $string = isset($value['max']) ? 'Before '. format_date($value['max']) : 'Any';
      }
      elseif (!isset($value['max'])) {
        $string = 'After '. format_date($value['min']);
      }
      else {
        $string= 'Between '. format_date($value['min']) .' and '. $format_date($values['max']);
      }
      
      return '<span class="filter-item">'. $string .'</span>';
    }
  }
    
  public function attachForm(array &$form_state, $default = NULL) {    
    $element = array(
      '#theme_wrappers' => array('form_element'),
      '#attributes' => array('class' => array('inline-fields')),
        
      0 => array(
        '#type' => 'date_popup',
        '#suffix' => ' TO ',
        '#date_format' => 'm-d-Y',
        '#date_year_range' => '-3:+0',
        '#date_label_position' => 'hidden',
        '#default_value' => isset($default[0]) ? $default[0] : NULL,
        '#description' => '',
      ),
      1 => array(
        '#type' => 'date_popup',
        '#date_format' => 'm-d-Y',
        '#date_year_range' => '-3:+0',
        '#date_label_position' => 'hidden',
        '#default_value' => isset($default[1]) ? $default[1] : NULL,
      ),
    );

    $this->setElementTitle($element);
    return $element;
  }

  /**
   * Translate form values into canonical values for the query
   * builders. They will always expect the same format associated
   * with the declared value type of this element.
   *  
   * @param array $value
   *  The constructed min and max timestamps to use for filtering.
   * @see iwSearchElement::translateValue().
   */
  public function translateValue($value) {
    $value[0] = isset($value[0]) ? trim($value[0]) : '';
    $value[1] = isset($value[1]) ? trim($value[1]) : '';
       
    $pat = '/^\d{4}-\d{2}-\d{2}|^\d{2}-\d{2}-\d{4}/';
    if (!preg_match($pat, $value[0])) {
      $value[0] = '';
    }
    
    if (!preg_match($pat, $value[1])) {
      $value[1] = '';
    }
    
    if (empty($value[0]) && empty($value[1])) {
      return NULL;
    }
    
    return $value;
  }
}
