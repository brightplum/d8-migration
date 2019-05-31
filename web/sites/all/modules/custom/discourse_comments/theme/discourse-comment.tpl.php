
<div<?php print $attributes; ?>>    
  <a class="author-icon-link" href="<?php print $user_url; ?>" target="_blank">
    <img class="author-avatar" src="<?php print $avatar_url; ?>" />
  </a>

  <div<?php print $content_attributes; ?>>
   <div class="post-author">
      <a href="<?php print $user_url; ?>" target="_blank">
        <span class="author-name"><?php print $username; ?></span>
      </a>
       <div class="post-date"><?php print $date; ?></div>
    </div>
  
    <?php print $content; ?>
    <?php print l('View / Reply', $post_url, array(
      'attributes'=> array('target' => '_blank', 'class' => array('post-link'))
    )); ?>
  </div>
  
</div>
