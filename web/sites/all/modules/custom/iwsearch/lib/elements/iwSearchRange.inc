<?php
/**
 * @file
 * A search field elements that can be set as a range.
 */

class iwSearchRange extends iwSearchFieldElement {
  
  public $valPrefix = '';
  public $valSuffix = '';
  
  public $usePresets = FALSE;
  public $rangeSets  = array();
  
  
  // ----------------------------------
  // Instance methods
  // ----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getValueType()
   */
  function getValueType() {
    return 'value_range';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getLabelOptions()
   */
  function getLabelOptions() {
    $options = parent::getLabelOptions();
    unset($options[parent::LABEL_INSIDE]);
  
    // Checkboxes don't support embedding of the label.
    return $options;
  }
  
  /**
   * Transform the $rangeSets into their string / display versions.
   * 
   * @return array
   *  Array of string values, representing each of the display ranges.
   */
  protected function builtRanges() {    
    if (!isset($this->rangeStrings)) {
      foreach ($this->rangeSets as $index => $range) {
        if (!isset($range[0])) {
          $this->rangeStrings[$index] = isset($range[1])
            ? "{$this->valPrefix}{$range[1]}{$this->valSuffix} or less" : 'Any';
        }
        elseif (!isset($range[1])) {
          $this->rangeStrings[$index] = "{$this->valPrefix}{$range[0]}{$this->valSuffix} or more";
        }
        else {
          $this->rangeStrings[$index] = "{$this->valPrefix}{$range[0]}{$this->valSuffix} ".
            "to {$this->valPrefix}{$range[1]}{$this->valSuffix}";
        }
      }
    }
    
    return $this->rangeStrings;
  }
  
  // ---------------------------------
  // Implements iwIPackage
  // ---------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::formatSummary()
   */
  function formatSummary() {
    $addl = array();
    if (!empty($this->valPrefix)) {
      $addl['prefix'] = check_plain($this->valPrefix); 
    }
    
    if (!empty($this->valSuffix)) {
      $addl['suffix'] = check_plain($this->valSuffix);
    } 
    
    $addl['Set ranges'] = $this->usePresets ? 'Yes' : 'No';
    
    $summary = parent::formatSummary();
    $summary['#properties'] += $addl;
    return $summary;
  }
  
  // ---------------------------------
  // Implements iwIConfigurable
  // ---------------------------------
  
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
    
    $element['usePresets'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use preset range values'),
      '#default_value' => $this->usePresets,
    );

    $element['rangeSets'] = array(
      '#type' => 'table_elements',
      '#title' => t('Range presets'),
      '#header' => array(t('Min'), t('Max'), t('Actions')),
      '#empty' => t('There are currently no preset values'),
      '#description' => t(
        'When enabled users are only able to use the predefined range sets. '.
        'If disabled, users will be given open ended text fields to enter values.'
      ),
    );
    
    if (!isset($form_state['rangeSets'])) {
      $form_state['rangeSets'] = $this->rangeSets;
    }
    
    $ranges = $form_state['rangeSets'];
    foreach ($ranges as $index => $range) {
      $element['rangeSets'][] = array(
        0 => array('#markup' => isset($range[0]) ? $range[0] : '*'),
        1 => array('#markup' => isset($range[1]) ? $range[1] : '*'),
        'delete' => array(
          '#type' => 'submit',
          '#name' => 'preset-ranges-' . $index,
          '#value' => t('Remove'),
          '#validate' => $form['#validate'],
          '#submit' => $form['#submit'],
          '#limit_validation_errors' => array(
            array_merge($element['#parents'], array('rangeSets', '__add_range')),
          ),
        ),
      );
    }
    
    $element['rangeSets']['__add_range'] = array(
      0 => array(
        '#type' => 'textfield',
        '#size' => 10,
        '#element_validate' => array('element_validate_number'),
      ),
      1 => array(
        '#type' => 'textfield',
        '#size' => 10,
        '#element_validate' => array('element_validate_number'),
      ),
      'actions' => array(
        'add' => array(
          '#type' => 'submit',
          '#value' => t('Add'),
          '#validate' => $form['#validate'],
          '#submit' => $form['#submit'],
          '#limit_validation_errors' => array(
            array_merge($element['#parents'], array('rangeSets', '__add_range')),
          ),
        ),
      ),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::validateElements()
   */
  function validateElements(array $element, array &$value, array &$form_state) {
    $addRange = $value['rangeSets']['__add_range'];
    if (empty($addRange[0]) || $addRange[0] === '*') {
      return;
    }
    elseif (empty($addRange[1]) || $addRange[1] === '*') {
      return;
    }
    elseif (intval($addRange[0]) > intval($addRange[1])) {
      form_error($element['rangeSets']['__add_range'], t(
        'The minimum number is set to be greater than the maximum.'
      ));
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    $trigger = $form_state['triggering_element'];
    if ($trigger['#value'] == t('Remove')) {
      if (preg_match('/^preset-ranges-(\d+)$/', $trigger['#name'], $matches)) {
        unset($form_state['rangeSets'][$matches[1]]);  
      }
      
      $form_state['rebuild'] = TRUE;
      return;
    }
    
    switch ($form_state['values']['op']) {
      case t('Add'):
        $newRanges = array();
        $oldRanges = $form_state['rangeSets'];
        
        $addRange = $values['rangeSets']['__add_range'];
        $addRange = array(
          0 => empty($addRange[0]) || $addRange == '*' ? NULL : intval($addRange[0]),
          1 => empty($addRange[1]) || $addRange == '*' ? NULL : intval($addRange[1]),
        );
        
        if (!empty($oldRanges)) {
          $range = reset($oldRanges);
          do {
            if($range[0] > $addRange[0] || ($range[0] == $addRange[0] && $range[1] > $addRange[1])) {
              break;
            }

            $newRanges[] = $range;
          } while ($range = next($oldRanges));
          
          // Add the new range in this position.
          $newRanges[] = $addRange;
          
          // Add the remaining range items left over.
          if ($range) {
            do {
              $newRanges[] = $range;
            } while ($range = next($oldRanges));
          }
        }
        else {
          $newRanges[] = $addRange;
        }
        
        // Store and rebuild the table updates / changes.
        $form_state['rangeSets'] = $newRanges;
        $form_state['rebuild'] = TRUE;
        break;

      default:
        parent::submitElements($element, $values, $form_state);
        
        $this->valPrefix  = $values['valPrefix'];
        $this->valSuffix  = $values['valSuffix'];
        $this->usePresets = (bool)$values['usePresets'];
        $this->rangeSets  = $form_state['rangeSets'];
    }
  }


  // -------------------------------
  // Handle search form
  // -------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchElement::filterString()
   */
  function filterString($value = NULL) {
    if (isset($value)) {
      if (!isset($value[0])) {
        $string = isset($value[1]) ? "Under {$this->valPrefix}{$value[1]}{$this->valSuffix}" : 'Any';
      }
      elseif (!isset($value[1])) {
        $string = "Over {$this->valPrefix}{$value[0]}{$this->valSuffix}";
      }
      else {
        $string= "{$this->valPrefix}{$value[0]}{$this->valSuffix} to {$this->valPrefix}{$value[1]}{$this->valSuffix}";
      }
      
      return '<span class="filter-item">'. check_plain($string) .'</span>';
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::attachForm()
   */
  function attachForm(array &$form_state, $default = NULL) {
    if ($this->usePresets) {      
      $element = array(
        '#type' => 'radios',
        '#options' => $this->builtRanges(),
        '#default_value' => $default,
      );
    }
    else {
      $element = array(
        '#theme_wrappers' => array('form_element'),
        '#attributes' => array('class' => array('inline-fields')),
        
        0 => array(
          '#type' => 'textfield',
          '#size' => 5,
          '#suffix' => ' TO ',
          '#field_prefix' => $this->valPrefix,
          '#field_suffix' => $this->valSuffix,
          '#element_validate' => array('element_validate_number'),
          '#attributes' => array('class' => array('numeric-entry')),
          '#default_value' => isset($default['min']) ? $default['min'] : NULL,
        ),
        1 => array(
          '#type' => 'textfield',
          '#size' => 5,
          '#field_prefix' => $this->valPrefix,
          '#field_suffix' => $this->valSuffix,
          '#element_validate' => array('element_validate_number'),
          '#attributes' => array('class' => array('numeric-entry')),
          '#default_value' => isset($default['max']) ? $default['max'] : NULL,
        ),
      );
    }

    $this->setElementTitle($element);
    return $element;
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchElement::getConditions()
   */
  function translateValue($value) {
    if (is_array($value)) {
      if (empty($value[0]) && empty($value[1])) {
        return NULL;
      }
    }
    elseif ($this->usePresets && isset($this->rangeSets[$value])) {
      return $this->rangeSets[$value];
    }
    
    return NULL;
  }
}
