<?php
/**
 * @file
 * Template file for the CTA Slide Down From Top.
 */
?>

<div class="cta-handle">
  <a id="cta-handle-open" href="#"><i class="fa fa-chevron-circle-down"></i> Please Help</a>
  <a id="cta-handle-close" href="#" style="display: none;"><i class="fa fa-chevron-circle-up"></i> Close</a>
</div>

<div class="cta-content banner">
  <?php print $content; ?>
</div>
<div class="slim-display banner">
  <div id="slim-display-content">
    <div class="cta-handle">
      <a id="cta-handle-open" href="#"><i class="fa fa-chevron-circle-up"></i> Help Today</a>
      <a id="cta-handle-close" href="#" style="display: none;"><i class="fa fa-chevron-circle-down"></i> Close</a>
    </div>
    <p><strong>Please Support Our Work</strong> -- Our Winter campaign ends TONIGHT and we can't fall short.
    </p>
  </div>
  <div id="slim-display-content-mobile">
     <p><strong>Please Support Our Work</strong> -- Our Winter campaign ends TONIGHT and we can't fall short.
    </p>
    <a class="btn" href="/donate?campaign=winter-campaign-2019&cta-location=mobileslide">DONATE</a>
    <a id="cta-handle-mobile-close" href="#"><i class="fa fa-close"></i></a>
  </div>
  <div id="super-slim-display-content-mobile">
    <a class="btn" href="/donate?campaign=winter-campaign-2019&cta-location=mobileslide3" target="_blank">DONATE</a>
  </div>
</div>
