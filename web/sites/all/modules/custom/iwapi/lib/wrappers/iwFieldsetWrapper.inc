<?php
/**
 * @file
 * Definition for iwContentWrapper which handles and manages a fieldset type.
 */

class iwFieldsetWrapper extends iwContentWrapper {

  public $settings = array(
    'display_title' => 'visible',
    'span' => -1,
    'equal_heights' => FALSE,
    'attributes' => array('class' => array()),
    'collapsible' => TRUE,
    'collapsed' => FALSE,
  );

  /**
   * (non-PHPDOC)
   * @see iwContentWrapper::view().
   */
  function view() {
    $element = parent::view();
    
    // Use a fieldset instead of a div.
    $element['#type'] = 'fieldset';
    
    if (!empty($this->settings['collapsible'])) {
      $element['#collapsible'] = TRUE;
      $element['#collapsed']   = !empty($this->settings['collapsed']); 
    }
 
    return $element;
  }

  /**
   * (non-PHPDOC)
   * @see iwContentWrapper::transferItems().
   */
  function transferItems(array &$element, array &$dst) {
    parent::transferItems($element, $dst);
    
    // This is handled here in the cases where fieldset is being used
    // outside of a form. This will ensure proper classes and library is included
    if (!empty($this->settings['collapsible']) && empty($dst['#processed'])) {
      $dst['#attributes']['class'][] = 'collapsible';
      $dst['#attached']['library'][] = array('system', 'drupal.collapse');
      
      if ($dst['#collapsed']) {
        $dst['#attributes']['class'][] = 'collapsed';
      }
    }
  }
  

  // ===================================
  // Additional formatting utilities
  // ===================================

  /**
   * Apply the title to the wrapper content
   * depending on the settings of the content wrapper.
   *
   * @param array $wrapper
   *  Reference to the wrapper array to apply the title to.
   */
  protected function applyTitle(array &$wrapper) {
    $ctrlClass = $this->controller;
    $attr = array('class' => array($ctrlClass::CssName .'-title'));

    switch ($this->settings['display_title']) {
      case 'inline':
      case 'visible':
        $wrapper['#title'] = check_plain($this->label);
        break;
      case 'hidden':
      case 'invisible':
        break;
      default:
        drupal_set_message(t('@class: Unknown title display setting %display', array(
          '@class'   => get_class($this),
          '%display' => $this->settings['display_title'],
        )), 'warning');
    }
  }
  
  
  // ----------------------------------
  // Form configurations
  // ----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
   
    $element['collapsible'] = array(
      '#type' => 'checkbox',
      '#title' => t('Collapsible'),
      '#default_value' => $this->settings['collapsible'],
    );
    
    $element['collapsed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Initially collapsed'),
      '#default_value' => $this->settings['collapsed'],
    );
  }
}
