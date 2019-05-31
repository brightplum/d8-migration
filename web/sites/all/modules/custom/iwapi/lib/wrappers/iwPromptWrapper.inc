<?php
/**
 * @file
 * Contains the wrapper which includes additional description or prompt information.
 */

class iwPromptWrapper extends iwContentWrapper {
 
  public $settings = array(
    'display_title' => 'visible',
    'span' => -1,
    'equal_heights' => FALSE,
    'attributes' => array('class' => array()),
    'promptPosition' => 'before',
    'promptStyle' => '',
    'prompt' => array(
      'value' => '',
      'format' => NULL,
    ),
  );  
  
  /**
   * non-PHPDOC
   * @see iwContentWrapper::view().
   */
  function transferItems(array &$element, array &$dst) {
    parent::transferItems($element, $dst);
    
    // Add the prompt and child content to the display element.
    $styles[] = 'content-prompt';
    if (!empty($this->settings['promptStyle'])) {
      $styles[] = $this->settings['promptStyle'];
    }
    $styles[] = 'clearfix';
    
    if (!empty($dst['container_title'])) {
      $content['container_title'] = $dst['container_title'];
    }
    
    $content['__prompt'] = array(
      '#prefix' => '<div class="'. implode(' ', $styles) .'">',
      '#suffix' => '</div>',
      '#markup' => check_markup($this->settings['prompt']['value'], $this->settings['prompt']['format']),
    );
    
    // Before or after the children?
    switch ($this->settings['promptPosition']) {
      case 'before':
        $dst = $content + $dst;
        break;

      case 'after':
        $dst += $content;
        break;
    }
  }

  /**
   * non-PHPDOC
   * @see iwContentWrapper::appendElements().
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    $element['prompt'] = array(
      '#type' => 'text_format',
      '#title' => t('Content Prompt'),
      '#rows' => 5,
      '#weight' => 5,
      '#format' => $this->settings['prompt']['format'],
      '#default_value' => $this->settings['prompt']['value'], 
    );
    
    $element['promptPosition'] = array(
      '#type' => 'select',
      '#title' => t('Position the prompt'),
      '#weight' => 5,
      '#options' => array(
        'before' => t('Before contents'),
        'after' => t('After contents'),
      ),
      '#default_value' => $this->settings['promptPosition'],
    );
    
    $element['promptStyle'] = array(
      '#type' => 'select',
      '#title' => t('Prompt style'),
      '#weight' => 5,
      '#empty_option' => t('- None -'),
      '#empty_value' => '',
      '#options' => array(
        'bolded' => t('Bold styling'),
        'italized' => t('Italized style'),
      ),
      '#default_value' => $this->settings['promptStyle'],
    );
  }
}
