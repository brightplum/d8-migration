<?php

class iwMapperBoolToValue extends iwSimpleMapper {
	
  protected $boolValue;

  /**
   * (non-PHPdoc)
   * @see iwIPackable::summarize()
   */
  static function summarize(array $definition) {
    // Set display results to show.
    $definition += array('inputs' => '',	'result' => '', 'boolValue' => '');

    // Convert the information into string form, for display.
    $params = array(
      '@name'   => $definition['inputs'],
      '@result' => $definition['result'],
      '@value'  => $definition['boolValue'],
    );

    return t("<strong>Input:</strong> @name\n<strong>Map to:</strong> @result\n<strong>Value:</strong> @value", $params);
  }

  /**
   * (non-PHPdoc)
   * @see iwWriter::package()
   */
  function package() {
    $data = parent::package();
    if (isset($this->boolValue)) {
      $data['boolValue'] = $this->boolValue;
    }
	  
    return $data;
  }

  /**
   * (non-PHPdoc)
   * @see iwIPackable::unpackage()
   */
  static function unpackage(array $definition, &$context) {
    // Generate a new instances of this mapper item.
    extract($definition);
    return new static($inputs, $result,	$boolValue);
  }

  /**
   *
   * @param $inputs
   * @param $result
   */
  function __construct($inputs, $result, $value) {
    parent::__construct($inputs, $result);

    $this->boolValue = $value;
  }

  /**
   *
   * @param array $raw
   * @return value;
   * 
   * @throws iwMapperException
   */
  function translate(array $raw) {
    if (!$this->isValid()) {
      throw new iwMapperException(iwMapperException::UNBUILT_MAPPER);
    }

 	  // Check if we can tell if this value is FALSE or not.
    if (empty($raw[$this->inputs])) {
      return '';
    }
	  	
    // Check for text phrases that we will consider as empty.
    switch (strtolower($raw[$this->inputs])) {
      case 'no':
      case 'null':
      case 'false':
        return '';
    }
    return $this->boolValue;
  }


  // ===============================
  // Implement iwIConfigurable
  // ===============================

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendFormElements()
   */
  static function appendFormElements(&$form, $callbacks, &$validation_scope = NULL) {
    parent::appendFormElements($form, $callbacks, $validation_scope);
    
    $form['#definition'] += array('boolValue' => '');
    $form['boolValue'] = array(
      '#type' => 'textfield',
      '#title' => t('Replacement value'),
      '#required' => TRUE,
      '#default_value' => $form['#definition']['boolValue'],
      '#description' => t('This is the value used when the value is considered TRUE by this mapper.'),
    );
  }

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitFormElements()
   */
  static function submitFormElements(&$form, &$form_state, $trigger) {
    $form['#definition']['inputs']    = $form_state['inputs'];
    $form['#definition']['result']    = $form_state['result'];
    $form['#definition']['boolValue'] = $form_state['boolValue'];

    // Report that these settings have been updated successful and expect to be updated.
    return array('save' => TRUE);
  }
}
