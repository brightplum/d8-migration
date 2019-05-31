<?php
/**
 * @file
 * Support handling bundles for as an exposed search parameter.
 */


class iwSearchBundles extends iwSearchElement {

  public $indexKey  = 'bundle';
  public $formType  = 'radio';
  public $emptyText = 'Any';
  public $multi     = TRUE;
  
  
  /**
   * (non-PHPdoc)
   * @see iwSearchCheckboxes::getValueType()
   */
  function getValueType() {
    return 'bundle';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchCheckboxes::getAllowedValues()
   */
  function getAllowedValues() {
    if (!isset($this->allowedBundles)) {
      $type = $this->provider->getEntityType();
      $info = entity_get_info($type);
      
      $bundles = array_intersect_key($info['bundles'], $this->provider->getBundles());
      $this->allowedBundles = _iwapi_extract_field($bundles);
    }
    
    return $this->allowedBundles;
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getLabelOptions()
   */
  function getLabelOptions() {
    $options = parent::getLabelOptions();
    
    // Checkboxes don't support embedding of the label.
    unset($options[self::LABEL_INSIDE]);
    return $options;
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
   * @see iwSearchElement::setElementSchema()
   */
  function setElementSchema(array &$schema) {
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::indexElement()
   */
  function indexElement($entity, array &$data) {
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::filterString().
   */
  function filterString($value = NULL) {
    if (!empty($value)) {
      $allowed = $this->getAllowedValues();
      $head = '<span class="filter-item">';
      $tail = '</span>';
      
      if ($this->multi) {
        $names = array_intersect_key($allowed, $value);
        $names = array_map('check_plain', $names);

        return $head . implode($tail .' or '. $head, $names) . $tail;
      }
      elseif (isset($allowed[$value])) {
        return $head . check_plain($value) . $tail;
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
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
        $element['#empty_option'] = $this->emptyText;
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
  function translateValue($value = NULL) {
    if ($this->multi || !is_array($value)) {
      $results = is_array($value) ? array_filter($value) : $value;
 
      if (!empty($results)) {
        return $results;
      }
    }
  }
}
