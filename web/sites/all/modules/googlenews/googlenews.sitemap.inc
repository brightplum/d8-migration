<?php

/**
 * @file
 * Functionality for generating the actual sitemap file.
 */

/**
 * Generate the news feed.
 */
function googlenews_getgooglenews() {
  // Included $base_url in the cache ID to allow per domain xmlfiles.
  global $base_url;
  $cid = $base_url . '/googlenews';
  $content = '';

  // Try loading the data from the cache.
  if ($cache = cache_get($cid, 'cache')) {
    // Verify the data hasn't expired.
    if (time() < $cache->expire) {
      $content = $cache->data;
    }
  }

  // If nothing loaded from the cache, build it now.
  if (empty($content)) {
    // Load the publication name.
    $publication_name = variable_get('googlenews_publication_name', '');
    if (empty($publication_name)) {
      $publication_name = variable_get('site_name', 'Drupal');
    }
    $publication_name = check_plain($publication_name);

    // Optionally add global tags.
    $default_tags = array();
    foreach (googlenews_tag_list() as $tag => $description) {
      if ($add = variable_get('googlenews_tag_' . $tag, '')) {
        $default_tags[$tag] = $add;
      }
    }
    // Triggers hook_googlenews_default_tags_alter().
    drupal_alter('googlenews_default_tags', $default_tags);

    // Arguments used to build the node URL. Optionally support a custom base
    // URL.
    $url_args = array('absolute' => TRUE);
    $base = variable_get('googlenews_base_url', FALSE);
    if (!empty($base)) {
      $url_args['base_url'] = $base;
    }

    // Loop through each record.
    foreach (googlenews_list_nodes() as $record) {
      // Load the node.
      $node = node_load($record->nid);
      $tags = $default_tags;
      // Triggers hook_googlenews_node_tags().
      drupal_alter('googlenews_node_tags', $tags, $node);

      // Get the node's language, use the global language string if the node
      // does not have one assigned.
      $langcode = entity_language('node', $node);
      if ($langcode == LANGUAGE_NONE) {
        $langcode = $GLOBALS['language']->language;
      }

      $item = array(
        'url' => url('node/' . $node->nid, $url_args),
        'langcode' => check_plain($langcode),
        'title' => check_plain($node->title),
        'date' => gmdate(DATE_W3C, $node->created),
        'tags' => array(),
      );

      // Insert any added tags if they have values; processing tokens first.
      if (!empty($tags)) {
        foreach ($tags as $tag => $value) {
          // Some items can be output as a comma-separated list.
          if (is_array($value)) {
            $value = implode(', ', $value);
          }
          if (!empty($value)) {
            $value = check_plain(token_replace($value, array('node' => $node), array('clear' => TRUE, 'sanitize' => FALSE)));
            if (!empty($value)) {
              $item['tags'][$tag] = $value;
            }
          }
        }
      }

      // Triggers hook_googlenews_item_alter().
      drupal_alter('googlenews_item', $item, $node);

      $items[] = $item;
    }

    // Triggers hook_googlenews_urlset_alter().
    $attributes = array(
      'sitemap' => 'xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"',
      'news' => 'xmlns:news="http://www.google.com/schemas/sitemap-news/0.9"',
    );
    drupal_alter('googlenews_urlset', $attributes);

    // Compile the final XML.
    $content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    $content .= "<urlset " . implode(' ', $attributes) . ">\n";
    if (!empty($items) && is_array($items)) {
      foreach ($items as $item) {
        $content .= "  <url>\n";
        $content .= "    <loc>{$item['url']}</loc>\n";
        $content .= "    <news:news>\n";
        $content .= "      <news:publication>\n";
        $content .= "        <news:name>{$publication_name}</news:name>\n";
        $content .= "        <news:language>{$item['langcode']}</news:language>\n";
        $content .= "      </news:publication>\n";
        $content .= "      <news:title>{$item['title']}</news:title>\n";
        $content .= "      <news:publication_date>{$item['date']}</news:publication_date>\n";
        if (!empty($item['tags']) && is_array($item['tags'])) {
          foreach ($item['tags'] as $tag => $value) {
            $content .= "      <news:{$tag}>{$value}</news:{$tag}>\n";
          }
        }
        $content .= "    </news:news>\n";
        $content .= "  </url>\n";
      }
    }
    $content .= "</urlset>\n";

    // Cache the output for later.
    $timeout = time() + (intval(variable_get('googlenews_cache_timeout', 15)) * 60);
    cache_set($cid, $content, 'cache', $timeout);
  }

  drupal_add_http_header('Content-Type', 'text/xml; charset=utf-8');
  print $content;
}
