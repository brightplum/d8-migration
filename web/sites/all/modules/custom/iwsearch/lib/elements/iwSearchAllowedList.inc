<?php
/**
 * @file
 * Contains the search
 */


class iwSearchAllowedList extends iwSearchFieldElement {

  public $formType       = 'radio';
  public $emptyText      = 'Any';
  public $multi          = TRUE;


  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'values';
  }
  
  /**
   * Find the available options listing for this field
   * type. Generally this is just an array of allowed
   * values found in the field settings.
   */
  protected function getAllowedValues() {
    $field = field_info_field($this->fieldName);
    return $field['settings']['allowed_values'];
  }
  
  /**
   * non-PHPDOC
   * @see iwIPackage::formatSummary()
   */
  function formatSummary() {
    if ($this->formType == 'select') {
      $addl['Form type'] = 'Selectbox';
    }
    elseif ($this->multi) {
      $addl['Form type'] = 'Checkboxes';
    }
    else {
      $addl['Form type'] = 'Radio buttons';
    }
    
    $addl = array();
    if ($this->multi) {
      $addl['Multiple values'] = t('Yes');
    }
  
    $summary = parent::formatSummary();
    $summary['#properties'] += $addl;
    return $summary;
  }   
 
  // ------------------------------------
  // Implement iwIConfigurable
  // ------------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $element['formType'] = array(
      '#type' => 'select',
      '#title' => t('Element type'),
      '#options' => array(
        'radio'  => t('radio / checkbox'),
        'select' => t('selectbox'),
      ),
      '#default_value' => $this->formType,
    );
    
    $element['emptyText'] = array(
      '#type' => 'textfield',
      '#title' => t('No selection text'),
      '#size' => 32,
      '#default_value' => $this->emptyText,
    );
    
    $element['multi'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow multiple values'),
      '#default_value' => $this->multi,
    );
    
    return $element;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
    
    $this->multi     = $values['multi'];
    $this->formType  = $values['formType'];
    $this->emptyText = $values['emptyText'];
  }
  
  
  // ------------------------------------
  // Implement iwSearchElement
  // ------------------------------------

  /**
   * (non-PHPdoc)
   * @see iwSearchCheckboxes::filterString()
   */
  function filterString($value = NULL) {
    if (!empty($value)) {
      $allowed = $this->getAllowedValues();
      $head = '<span class="filter-item">';
      $tail = '</span>';
      
      if ($this->multi) {
        $names = array_intersect_key($allowed, array_flip($value));
        $names = array_map('check_plain', $names);

        return $head . implode($tail .' or '. $head, $names) . $tail;
      }
      elseif (isset($allowed[$value])) {
        return $head . check_plain($allowed[$value]) . $tail;
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
    $iwapi_path = drupal_get_path('module', 'iwapi');
      
    if ($this->multi && empty($default)) {
      $default = array();
    }

    $element = array(
      '#options' => $this->getAllowedValues(),
      '#default_value' => $default,
    );

    switch ($this->formType) {
      case 'select':
        $element['#type'] = 'select';
        $element['#multiple'] = $this->multi;
        
        if (!empty($this->emptyText)) {
          $element['#empty_option'] = $this->emptyText;
          $element['#attached']['js'][] = $iwapi_path .'/js/general.js';
          $element['#attributes']['class'][] = 'has-empty';
        }
        elseif ($this->labelDisplay == static::LABEL_INSIDE) {
          $element['#empty_option'] = $this->label;
          $element['#attached']['js'][] = $iwapi_path .'/js/general.js';
          $element['#attributes']['class'][] = 'has-empty';
        }
        break;
      case 'radio':
        if ($this->multi) {
          $element['#type'] = 'checkboxes';   
        }
        else {
          $element['#type'] = 'radios';

          if (!empty($this->emptyText)) {
            $element['#options'] = array('' => $this->emptyText) + $element['#options'];
          }
        }
        break;
    }
  
    $this->setElementTitle($element);
    return $element;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::translateValue()
   */
  function translateValue($value) {
    $field = field_info_field($this->fieldName);
    switch ($field['type']) {
      case 'number_decimal':
      case 'number_float':   case 'list_float':
        $callback = 'doubleval';
        break;
      case 'number_integer': case 'list_integer':
        $callback = 'intval';
        break;
      default:
        $callback = 'trim';
    }
   
    if ($this->multi && is_array($value)) {   
      $results = array_filter($value);
      $results = array_map($callback, $results);
    }
    elseif (!$this->multi && !is_array($value)){
      $results = $callback($value);
    }

    if (!empty($results)) {
      return $results;
    }
  }
}