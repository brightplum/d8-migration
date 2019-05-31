
<div <?php print $attributes; ?>>
  <?php
    print render($title_prefix);
    print render($title_suffix);
  ?>
  
  <div<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>
</div>