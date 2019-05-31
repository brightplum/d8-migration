<?php

/**
 * @file
 * Contains the 'taxonomy term' argument validator plugin.
 */

/**
 * Validate whether an argument is an acceptable node.
 */
class iwapi_plugin_argument_validate_term_alias extends views_plugin_argument_validate {
  function init(&$view, &$argument, $options) {
    parent::init($view, $argument, $options);

    // Convert legacy vids option to machine name vocabularies.
    if (!empty($this->options['vids'])) {
      $vocabularies = taxonomy_get_vocabularies();
  
      foreach ($this->options['vids'] as $vid) {
        if (isset($vocabularies[$vid], $vocabularies[$vid]->machine_name)) {
          $this->options['vocabularies'][$vid] = $vid;
        }
      }
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['vocabularies'] = array('default' => array());
    $options['type'] = array('default' => 'tid');
    $options['transform'] = array('default' => FALSE, 'bool' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $vocabularies = taxonomy_get_vocabularies();
    $options = array();
    foreach ($vocabularies as $voc) {
      $options[$voc->vid] = check_plain($voc->name);
    }

    $form['vocabularies'] = array(
      '#type' => 'checkboxes',
      '#prefix' => '<div id="edit-options-validate-argument-vocabulary-wrapper">',
      '#suffix' => '</div>',
      '#title' => t('Vocabularies'),
      '#options' => $options,
      '#default_value' => $this->options['vocabularies'],
      '#description' => t('If you wish to validate for specific vocabularies, check them; if none are checked, all terms will pass.'),
    );
  }

  function options_submit(&$form, &$form_state, &$options = array()) {
    // Filter unselected items so we don't unnecessarily store giant arrays.
    $options['vocabularies'] = array_filter($options['vocabularies']);
  }

  function convert_options(&$options) {
    if (!isset($options['vocabularies']) && !empty($this->argument->options['validate_argument_vocabulary'])) {
      $options['vocabularies'] = $this->argument->options['validate_argument_vocabulary'];
    }
  }

  function validate_argument($argument) {
    $vocabularies = array_filter($this->options['vocabularies']);
  
    $query = db_select('taxonomy_term_data', 'td')
      ->fields('td', array('tid'))
      ->condition('td.alias', strtolower($argument));
    
    if (!empty($vocabularies)) {
      $query->condition('td.vid', $vocabularies);
    }   

    $tid = $query->execute()->fetchField();
    if ($term = taxonomy_term_load($tid)) {
      $this->argument->argument = $term->tid;
      $this->argument->validated_title = check_plain(entity_label('taxonomy_term', $term));
      return TRUE;
    }
    return FALSE;
  }

  function process_summary_arguments(&$args) {
    $vocabularies = array_filter($this->options['vocabularies']);

    $arg_keys = array_flip($args);

    $query = db_select('taxonomy_term_data', 'td');
    $query->condition('tid', $args);
    $query->addField('td', 'tid', 'tid');

    if (!empty($vocabularies)) {
      $query->leftJoin('taxonomy_vocabulary', 'tv', 'td.vid = tv.vid');
      $query->condition('tv.machine_name', $vocabularies);
    }
    $query->addField('td', 'name', 'name');

    foreach ($query->execute()->fetchAllKeyed() as $tid => $term) {
      $args[$arg_keys[$tid]] = $term;
    }
  }
}
