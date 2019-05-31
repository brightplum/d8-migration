<?php
/**
 * @file
 * Template file from the CTA Modal Popup.
 */
?>
<style>
#bio_ep {
  border: 3px solid #FFCC00;
  border-radius: 6px;
}
#bio_ep .content {
  padding: 15px;
}
#bio_ep_close {
  display: none;
}
#bio_ep #close-modal {
    display: block;
    position: relative;
    left: auto;
    float: right;
    background: transparent;
    color: #000;
    margin: 10px;
}
</style>
<div id="bio_ep_bg"></div>
<div id="bio_ep">
  <a href="#" id="close-modal">X</a>
  <div class="content">
    <?php print $content; ?>
  </div>
</div>
