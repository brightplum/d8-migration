<div<?php print $attributes; ?>>
  <?php 
    print render($title_prefix); 
    print '<h3 class="comment-title">'. $title .'</h3>'; 
    print render($title_suffix);
  ?>
  <div class="comment-info">
    <p class="submitted"><?php print $attribution; ?></p>
  </div>
    
  <div<?php print $content_attributes; ?>>
    <?php print render($content); ?>
  </div>
</div>

<?php if (!empty($children)) {
  print render($children);  
} ?>
