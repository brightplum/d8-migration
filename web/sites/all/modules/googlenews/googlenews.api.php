<?php

/**
 * @file
 * Hooks provided by the Google News sitemap module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Implements hook_googlenews_urlset_alter().
 *
 * Allows customization of the attributes used in the <urlset> tag.
 */
function hook_googlenews_urlset_alter(&$attributes) {
}

/**
 * Extend the default Google News sitemap schema with additional elements.
 */
function hook_googlenews_default_tags_alter(&$tags) {
}

/**
 * Modify the values used to render the sitemap entry for a specific node.
 *
 * @param array $tags
 *   The configured tag values, including tokens before rendering. This allows
 *   simple modifications to the tags based on the node.
 * @param object $node
 *   The loaded node object being processed for this news entry. Changes made
 *   in this hook will affect token processing and the small number of hardcoded
 *   sitemap items pulled from the node, such as title and publication date, but
 *   will not affect the saved node.
 */
function hook_googlenews_node_tags(&$tags, $node) {
  // Integration with Publication Date module.
  $node->created = $node->published_at;

  // Content of type "blog" should always be reported as a Blog.
  if ($node->type == 'blog') {
    $tags['genres'][] = 'Blog';
    // Eliminate duplicates in case the blog was correctly marked.
    $tags['genres'] = array_unique($tags['genres']);
  }
}

/**
 * Modify the final values for the sitemap entry for a specific node.
 *
 * @param array $item
 *   The finished sitemap entry for this node. Allows for a final modification
 *   prior to output.
 * @param object $node
 *   The loaded node object being processed for this news entry. Changes should
 *   not be made to this object.
 */
function hook_googlenews_item_alter(&$item, $node) {
  // Add the AMP suffix to all URLs.
  $item['url'] .= '?amp';
}

/**
 * @} End of "addtogroup hooks".
 */
