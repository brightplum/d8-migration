<?php
/**
 * @file
 * Contains the basic theme functions, and display callbacks.
 */


/**
 * Implements TEMPLATE_preprocess_discourse_comments().
 */
function template_preprocess_discourse_comments(&$vars) {
  drupal_add_css(drupal_get_path('module', 'discourse_comments') .'/css/discourse-comments.css');

  $comments  = $vars['comments'];

  if (empty($comments['#topic_url'])) {
    $protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";
    $server = variable_get(DISCOURSE_COMMENTS_VAR_SERVER, array());
    $uri = $protocol . $server['host'] . "/t/{$comments['#topic_slug']}/{$comments['#topic_id']}";
    $comments['#topic_url'] = check_url($uri);
  }

  // Determine the correct summary text
//  if ($comments['#count'] > 0) {
//    $vars['count'] = t('%posts of %count Comments', array(
//      '%posts' => count($comments['#posts']),
//      '%count' => $comments['#count'],
//    ));
//  }
//  else {
//    $vars['count'] = t('No comments yet, be the first!');
//  }

//  $vars['posts'] = array();
  $vars['discourse_url'] = $comments['#topic_url'];

  $parity = 0;
//  foreach ($comments['#posts'] as $post) {
//    $vars['posts'][] = theme('discourse_comment', array(
//      'post' => $post,
//      'topic_url' => $topic_url,
//      'parity' => (++$parity)%2 == 0,
//    ));
//  }
}

/**
 * Implements TEMPLATE_preprocess_discourse_comment().
 */
function template_preprocess_discourse_comment(&$vars) {
  $protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";
  $server = variable_get(DISCOURSE_COMMENTS_VAR_SERVER, array());
  $post   = $vars['post'];

  $timestamp = strtotime($post->created_at);
  $vars['date'] = (REQUEST_TIME - $timestamp) > 23 * 3600
    ? date('M j Y', $timestamp) : ceil((REQUEST_TIME - $timestamp)/3600) .' hours ago';



  $vars['username']   = check_plain($post->username);
  $vars['user_url']   = check_url($protocol . "$server[host]/users/$post->username/activity");
  $vars['avatar_url'] = check_url(preg_replace('#\{size\}#', '45', $post->avatar_template));
  $vars['post_url']   = $vars['topic_url'] .'/'. intval($post->post_number);

  if (!empty($post->name)) {
    $vars['subject'] = check_plain($post->name);
  }

  // Trust that cooked is really cooked right?
  $vars['content'] = $post->cooked;

  $vars['classes_array'][] = $vars['parity'] ? 'even' : 'odd';
  $vars['classes_array'][] = 'clearfix';

  $vars['content_attributes_array']['class'][] = 'post-body';
}
