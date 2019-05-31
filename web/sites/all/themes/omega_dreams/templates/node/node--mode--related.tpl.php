
<article<?php print $attributes; ?>>
  <?php if (!empty($title_prefix) || !empty($title_suffix)): ?>
    <header>
      <?php print render($title_prefix); ?>
      <?php print render($title_suffix); ?>
    </header>
  <?php endif; ?>

  

  <div<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
    
  </div>

  <?php print render($content['links']); ?>
  <?php print render($content['comments']); ?>
</article>
