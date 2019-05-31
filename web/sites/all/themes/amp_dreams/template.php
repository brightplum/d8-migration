<?php

/**
 * Preprocess all templates.
 */
function amp_dreams_preprocess(&$vars, $hook) {
  $vars['ampsubtheme_path_file'] = DRUPAL_ROOT . '/' . drupal_get_path('theme', 'amp_dreams');
}

/**
 * Implements hook_preprocess_HOOK() for HTML document templates.
 *
 * Example of a preprocess hook for a subtheme that could be used to change
 * variables in templates in order to support custom styling of AMP pages.
 */
function amp_dreams_preprocess_html(&$variables) {

}

function amp_dreams_preprocess_node(&$variables) {
  if (isset($variables['content']['body'][0]['#markup'])) {
    // Replace <img> tags with <amp-img> variant.
    $variables['content']['body'][0]['#markup'] = str_replace('<img ', '<amp-img layout="responsive" height="300" width="300"  ', $variables['content']['body'][0]['#markup']);
  }
  // Share links.
  if (preg_match('/headline|newswire|views_article/', $variables['type'])) {
    $content_url = urlencode(url('node/' . $variables['nid'], ['absolute' => TRUE]));
    $share = [
      '#markup' => '<div class="share"><a class="share_link share_link--left" href="https://www.facebook.com/sharer/sharer.php?u=' . $content_url . '" rel="nofollow" target="_blank"><i class="fa fa-facebook" aria-hidden="true"></i></a><a class="share_link share-link--right" href="https://twitter.com/intent/tweet?url=' . $content_url . '&amp;text=' . urlencode($variables['title']) . '&amp;via=commondreams" rel="nofollow" target="_blank"><i class="fa fa-twitter" aria-hidden="true"></i></a></div>'
    ];
    $variables['content']['body'][0] = [
      'share' => $share,
      'body' => $variables['content']['body'][0]
    ];
  }
}

/**
 * Implements hook_preprocess_THEME_NAME().
 */
function amp_dreams_preprocess_image(&$variables) {
  $variables['attributes']['layout'] = 'responsive';
}

/**
 * Implements hook_preprocess_THEME_NAME().
 */
function amp_dreams_preprocess_cta_content_block_one(&$variables) {
  // Replace <img> tags with <amp-img> variant.
  $variables['content'] = str_replace('<img ', '<amp-img layout="responsive" height="300" width="300"  ', $variables['content']);
}
