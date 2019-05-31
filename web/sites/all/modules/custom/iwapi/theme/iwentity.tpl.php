
<div<?php print $attributes?>>
  <?php print render($title_prefix);?>
  <?php if (!$page && empty($title_in_content)): ?>
  <h2<?php print $title_attributes; ?>>
    <?php if (!(empty($entity_uri['path']) || empty($title))): ?>
    <a href="<?php print url($entity_uri['path']); ?>"><?php print $title; ?></a>
    <?php else: ?>
    <?php print $title; ?>
    <?php endif; ?>
  </h2>
  <?php endif; ?>
  <?php print render($title_suffix);?>

  <div<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>
</div>