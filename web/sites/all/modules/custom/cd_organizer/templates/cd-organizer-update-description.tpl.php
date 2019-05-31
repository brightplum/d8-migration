<?php
/**
 * @file
 * Template file for the Content Description.
 */
?>
<p>The following content will be updated:</p>
<ul>
  <?php foreach ($nodes as $node) : ?>
    <li><?php print l($node->title, 'node/' . $node->nid, ['attributes' => ['target' => '_blank']]); ?></li>
  <?php endforeach; ?>
</ul>
