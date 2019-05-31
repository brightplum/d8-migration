<?php
/**
 * @file
 * Contains the inline field content wrapper handler.
 */

class iwInlineFieldWrapper extends iwContentWrapper {

  /**
   * (non-PHPdoc)
   * @see iwContentWrapper::view()
   */
  function view() {
    $element = parent::view();
    
    $element['#attributes']['class'][] = 'inline-fields';
    return $element;
  }
  
  function transferItems(array &$element, array &$dst) {
    $dst['#sorted'] = TRUE;
    
    if (!empty($this->settings['separator']['prefix'])) {
      $dst[] = array(
        '#markup' => check_plain($this->settings['separator']['prefix']),
        '#prefix' => '<div class="grouping-prefix">',
        '#suffix' => '</div>',
      );
    }
    
    // add our spacing and separators between fields.
    if (!empty($this->settings['separator']['between'])) {      
      $child = reset($this->children);
      
      // Transfer the first available item without a separator
      while ($child = current($this->children)) {
        if (isset($element[$child])) {
          $dst[$child] = $element[$child];
          unset($element[$child]);
          break;
        }
        
        next($this->children);
      }
      
      // Remaining items should have a separator before them.
      while ($child = next($this->children)) {
        if (isset($element[$child])) {
          $dst[] = array(
            '#markup' => check_plain($this->settings['separator']['between']),
            '#prefix' => '<div class="grouping-spacer">',
            '#suffix' => '</div>',
          );
          
          $dst[$child] = $element[$child];
          unset($element[$child]);
        }
      }
    }
    else {
      parent::transferItems($element, $dst);
    }
   
    if (!empty($this->settings['separator']['suffix'])) {
      $dst[] = array(
        '#markup' => check_plain($this->settings['separator']['suffix']),
        '#prefix' => '<div class="grouping-suffix">',
        '#suffix' => '</div>',
      );
    }
  }


  // ===================================
  // Implements iwIConfigurable
  // ===================================
  
  /**
   * (non-PHPdoc)
   * @see iwContentWrapper::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    parent::appendElements($element, $form, $form_state);
    
    if (!isset($this->settings['separator'])) {
      $this->settings['separator'] = array();
    }
    
    // Set the defaults for the separator settings.
    $this->settings['separator'] += array(
      'prefix'  => NULL,
      'between' => NULL,
      'suffix'  => NULL,
    );

    $element['separator'] = array(
      '#type' => 'fieldset',
      '#title' => t('Group Formatting'),
      '#weight' => 75,
      '#tree' => TRUE,
      
      'prefix' => array(
        '#type' => 'textfield',
        '#title' => t('Group prefix'),
        '#size' => 10,
        '#default_value' => $this->settings['separator']['prefix'],
      ),
      
      'between' => array(
        '#type' => 'textfield',
        '#title' => t('Separator'),
        '#size' => 10,
        '#default_value' => $this->settings['separator']['between'],
      ),
      
      'suffix' => array(
        '#type' => 'textfield',
        '#title' => t('Group suffix'),
        '#size' => 10,
        '#default_value' => $this->settings['separator']['suffix'],
      ),
    );
  }
}
