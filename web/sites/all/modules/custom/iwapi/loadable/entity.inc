<?php
/**
 * @file includes/entities.inc
 * Contains handlers and general functionality for handling entities and fields.
 */


/**
 * Implements HOOK_entity_attacher_info().
 */
function iwapi_entity_attacher_info() {
  $info = array(
    'field' => array(
      'label'   => t('Fields'),
      'handler' => 'iwFieldAttacher',
    ),
    'field_extras' => array(
      'label'   => t('Extra Fields'),
      'handler' => 'iwFieldExtraAttacher',
    ),
  );
  
  // Add the field group attacher if the module exists.
  if (module_exists('field_group')) {
    $info['fieldgroup'] = array(
      'label' => t('Field Groups'),
      'handler' => 'iwFieldGroupAttacher',
    );
  }
  
  return $info;
}

/**
 * Return a list of entity attachers available, for the
 * current bootstrap 
 */
function _iwapi_get_entity_attacher_info() {
  // Depending on when this function is called, different set of classes maybe available.
  // if called without a full bootstrap, fallback to only core classes being available.
  if (drupal_bootstrap() === DRUPAL_BOOTSTRAP_FULL) {
    static $attachers;
  
    if (!isset($attachers)) {  
      $attachers = module_invoke_all('entity_attacher_info') + iwapi_entity_attacher_info();
      drupal_alter('entity_attacher_info', $attachers);
    }
    return $attachers;
  }
  else {
    // Since this is a temporary and incomplete listing of what's
    // available, we do not check or save to the Drupal cache
    return iwapi_entity_attacher_info();
  }
}


/**
 * Helper function to add the default entity info for iwEntity
 * class types. This function is generally used for 
 * HOOK_entity_info() functions to make it easier to apply defaults.
 * 
 * @param string $entity_class
 *  String name of the class object used to 
 * @param array $info
 *  Reference to the entity information definition. The
 *  method will populate the info with defined by the entity
 *  class. Key, default URI and label callbacks.
 */
function _iwapi_populate_entity_info($entity_class, array &$info) {
  // First ensure that the entity class exists and is of valid type
  if (class_exists($entity_class) && is_subclass_of($entity_class, 'iwEntity')) {  
    // Try to append the defaults incase these are still blank
    $info += array(
      'entity class'     => $entity_class,
      'controller class' => 'iwEntityController',
      'uri callback'     => 'iwapi_entity_uri',
      'label callback'   => 'iwapi_entity_label',
      'access callback'  => '_iwapi_entity_access',
      'view callback'    => '_iwapi_entity_view',
    );

    // Use the tables and entity keys defined by the handling class.
    $info['base table']  = $entity_class::getTable();
    $info['entity keys'] = $entity_class::getKeys();
  }
  else {
    $msg = t('PHP class of type @class does not exists or is not a sub-type of iwEntity', array('@class' => $entity_class));
    throw new InvalidArgumentException($msg);
  }
}

/**
 * Run through all our entity attacher classes and see what items
 * from $template can be applied to it. This function does not
 * actually create the entity type or bundle, it only tries to apply
 * template items to one that already exists.
 * 
 * @param $entity_info
 *  Array which contains the entity type and bundle to attach
 *  the template properties to.
 * @param $template
 *  The template which contains the information we want to
 *  apply to the entity.
 * @param $force
 *  Whether or not we want to re-write information if it already
 *  exists for this entity.
 */
function _iwapi_attach_entity_template($type, $bundle, array $template, $force = FALSE) {
  // Reset the fields info and ensure that we get fresh definitions.
  field_info_cache_clear();

  $attachers = _iwapi_get_entity_attacher_info();
  $attachers = array_intersect_key($attachers, $template);
  
  foreach ($attachers as $key => $info) {
    try {
      $attacher = new $info['handler']($type, $bundle);
      $attacher->attach($template[$key], $force);
    }
    catch (iwException $e) {
      $e->logToMessages();
      throw $e;
    }
    catch (Exception $e) {
      watchdog_exception('entity_attacher', $e);
      throw $e;
    }
  }
}

/**
 * Scan a folder for applicable entity template files
 * and apply them to all entities they belong to.
 * 
 * @param string $uri
 *  The URI or base directory where the template file
 *  live. This directory will be searched for valid templates.
 */
function _iwapi_install_entity_templates_dir($uri, $create = FALSE) {
  $uri   = rtrim($uri, '/');
  $files = file_scan_directory($uri, '/\.template\.inc$/i', array(
    'recurse' => FALSE,
    'key' => 'name',
  ));
  
  foreach ($files as $name => $file) {
    $isMatch = preg_match('#^([a-z,0-9,_]+)--([a-z,0-9,_]+)\.template#', $name, $matches);
    
    if ($isMatch && is_readable($file->uri) && ($entity_info = entity_get_info($matches[1]))) {
      include $file->uri;
      
      if (!key_exists($matches[2], $entity_info['bundles'])) {
        if ($matches[1] == 'node' && $create && !empty($template['bundle_info'])) {
          node_type_save((object)$template['bundle_info']);
        }
        else {
          drupal_set_message(t(
            'Unable to install %type: %bundle because bundle is not available and cannot be created.',
            array('%type' => $matches[1], '%bundle' => $matches[2])
          ), 'warning');

          continue;
        }
      }

      _iwapi_attach_entity_template($matches[1], $matches[2], $template, TRUE);
    }
  }
}

/**
 * Generate the entity template data from the current form values
 * found in the $form and $form_state variables.
 *
 * @param $form
 *  Current form build definition.
 * @param $form_state
 *  Reference to the values and state values of the form.
 * @return array
 *  Returns an array with the entity template extracted
 *  from the form state values.
 */
function _iwapi_extract_entity_template_data($form, &$form_state) {
  $template = array('bundle_info' => array());
  $values   = $form_state['values'];
  
  $type   = $values['entity_type'];
  $bundle = $values['entity_bundle'];
  
  // Save bundle info for node
  // TODO: Allow other entity types to save / restore bundle settings.
  if ($type == 'node' && $nodeType = node_type_get_type($bundle)) {
    $template['bundle_info'] = (array)$nodeType;
  }

  // Iterate through the registered entity attachers and try to run them.
  foreach (_iwapi_get_entity_attacher_info() as $attachTo => $info) {
    try {
      $attacher = new $info['handler']($type, $bundle);

      // Check for requirements and valid data before attaching.
      if ($attacher instanceof iwIConfigurable) {
        $template[$attachTo] = array();
        $element = $form['bundles'][$attachTo];
        $data    = isset($values[$attachTo]) ? $values[$attachTo] : array();
        
        $attacher->submitElements($element, $data, $form_state);
        $template[$attachTo] = $attacher->export();
      }
      else {
        $template[$attachTo] = $attacher->export();
      }
    }
    catch (iwException $e) {
      $e->logToMessages();
    }
  }
  return $template;
}
