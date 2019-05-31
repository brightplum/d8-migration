<?php

/**
 * @file
 * Default view template to display a item in an RSS feed.
 *
 * @ingroup views_templates
 */
?>
  <item>
    <title><?php print decode_entities($title); ?></title>
    <link><?php print $link . '?cd-origin=rss'; ?></link>
    <description><?php print $description; ?></description>
    <?php print $item_elements; ?>
  </item>
