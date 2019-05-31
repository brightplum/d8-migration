<?php
/**
 * @file
 * Provides Drupal token hooks for CDreams profiles.
 */


/**
 * Implements HOOK_token_info().
 */
function cdreams_profile_token_info() {
  $types = array(
    'cdreams_profile' => array(
      'name' => t('Common Dreams profile'),
      'description' => t('Embeddable entities for other content.'),
      'needs-data' => 'cdreams_profile',
    ),
  );
  
  $tokens = array(
    'cdreams_profile' => array(
      'id' => array(
        'name' => t('Identifier'),
        'description' => t('The unique identifier for this product item.'),
      ),
      'name' => array(
        'name' => t('Name'),
        'description' => t('Name (title) used for this profile.'),
      ),
      'bundle' => array(
        'name' => t('Bundle'),
        'description' => t('Machine name of the bundle for this entity.'),
      ),
      'bundle_name' => array(
        'name' => t('Bundle name'),
        'description' => t('User friendly display name of the bundle.'),
      ),
      'url' => array(
        'name' => t('URL'),
        'description' => t('The URL to this product.'),
      ),
      'author' => array(
        'name' => t('Author'),
        'description' => t('User who created this profile.'),
        'type' => 'user',
      ),
      'created' => array(
        'name' => t('Creation date'),
        'description' => t('Date that this product item was created.'),
        'type' => 'date',
      ),
    ),
  );
  
  // Return the token information back 
  return array('types' => $types, 'tokens' => $tokens);
}

/**
 * Implements HOOK_tokens().
 */
function cdreams_profile_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replace = array();  if ($type !== 'cdreams_profile' || empty($data['cdreams_profile'])) {
    return array();
  }

  $replace     = array();
  $entity      = $data['cdreams_profile'];
  $entity_type = 'cdreams_profile';
  
  // Configure general options for  updates.
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);
  
  // Find all the substitutes for tokens.
  foreach ($tokens as $name => $original) {
    switch ($name) {
      case 'id':
        $replace[$original] = $entity->getId();
        break;
       
      case 'name':
        $replace[$original] = $sanitize ? check_plain($entity->getLabel()) : $entity->getLabel();
        break;
        
      case 'bundle':
        $replace[$original] = $entity->getBundle();
        break;
          
      case 'bundle_name':
        $type_info = cdreams_profile_type_load($entity->getBundle());
        $replace[$original] = $sanitize ? check_plain($type_info->label) : $type_info->label;
        break;
          
      case 'url':
        $uri = $entity->getUri();
        $replace[$original] = url($uri['path'], $url_options);
        break;
    }
  }
 
  if ($author_tokens = token_find_with_prefix($tokens, 'author')) {
    $author = user_load($entity->uid);
    $replace += token_generate('user', $author_tokens, array('user' => $author), $options);
  }

  if ($created_tokens = token_find_with_prefix($tokens, 'created')) {
    $replace += token_generate('date', $created_tokens, array('date' => $entity->created), $options);
  }

  return $replace;
}
