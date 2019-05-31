<?php
/**
 * @file
 * Contains token based HOOKs.
 */

/**
 * Implements HOOK_token_info_alter().
 */
function views_term_alias_token_info_alter(&$info) {
  $info['tokens']['term']['term-alias'] = array(
    'name' => t('Aliased name'),
    'description' => t('Contains a name that is safe for use as a URL argument.'),
  );
}

/**
 * Implements HOOK_tokens().
 */
function views_term_alias_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $repl = array();
  
  if ($type === 'term' && !empty($data['term'])) {
    $term = $data['term'];
    
    if (isset($tokens['term-alias'])) {
      $repl[$tokens['term-alias']] = $term->alias;
    }
  }
  
  return $repl;
}