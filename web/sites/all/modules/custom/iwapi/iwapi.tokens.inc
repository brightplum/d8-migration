<?php
/**
 * @file iwapi.tokens.inc
 * Generate entity tokens provided by iwapi.
 * 
 * Additionally, I hope to one day get rid of this if it is possible
 * to handle this outsite of my own custom module. At this point there
 * are too many inconsistencies with Token handling for Drupal, and
 * a lack of general entity tokens, despite the generic entity functions
 * like entity_uri() or entity_label().
 */


/**
 * Implements HOOK_token_info().
 */
function iwapi_token_info() {
  return array(
    'types' => array(
      'entity_object' => array(
        'name' => t('Generic Entity Info'),
        'description' => t('General entity data, which can be found on ANY entity object.'),
        'needs-data' => 'entity_object',
      ),
    ),
    'tokens' => array(
      'entity_object' => array(
        'entity_type' => array(
          'name' => t('Entity type'),
          'description' => t('The type of entity being represented here (node, user, term, etc...).'),
        ),
        'id' => array(
          'name' => t('Entity ID'),
          'description' => t('Unique identifier for this entity.'),
        ),
        'bundle' => array(
          'name' => t('Bundle'),
          'description' => t('Machine name of the bundle of this entity.'),
        ),
        'entity_name' => array(
          'name' => t('Entity name'),
          'description' => t('User friendly display name of the entity type.'),
        ),
        'bundle_name' => array(
          'name' => t('Bundle name'),
          'description' => t('User friendly display name for the entity bundle.'),
        ),
        'label' => array(
          'name' => t('Entity label'),
          'description' => t('The label or title used for this entity object.'),
        ),
        'url' => array(
          'name' => t('Entity URL'),
          'description' => t('The URL used to view the entity.'),
        ),
      ),
    ),
  );
}

/**
 * Implements HOOK_tokens().
 */
function iwapi_tokens($type, $tokens, array $data = array(), array $options = array()) {

  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);
  
  // Start building the replacements for tokens.
  $repl = array();

  if ($type == 'entity_object' && !empty($data['entity_object'])) {
    $entity      = $data['entity_object']['entity'];
    $entity_type = $data['entity_object']['entity_type'];
    
    $entity_info = entity_get_info($entity_type);
    list($idVal, , $bundle) = entity_extract_ids($entity_type, $entity);
    
    // Iterate tokens and build replacements for items available.
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'entity_type':
          $repl[$original] = $sanitize ? check_plain($entity_type) : $entity_type;
          break;
          
        case 'id':
          $repl[$original] = $sanitize ? check_plain($idVal) : $idVal;
          break;
          
        case 'bundle':
          $repl[$original] = $bundle; // We can assume only safe characters were used.
          break;
    
        case 'entity_name':
          $repl[$original] = $sanitize ? check_plain($entity_info['label']) : $entity_info['label'];
          break;
          
        case 'bundle_name':
          $bundle_info = $entity_info['bundles'][$bundle];
          $repl[$original] = $sanitize ? check_plain($bundle_info['label']) : $bundle_info['label'];
          break;
        
        case 'label':
          $label = entity_label($entity_type, $entity);
          $repl[$original] = $sanitize ? check_plain($label) : $label;
          break;
 
        case 'url':
          $uri = entity_uri($entity_type, $entity);
          $repl[$original] = url($uri['path'], array('absolute' => TRUE));
          break;
      }
    }
  }
  return $repl;
}
