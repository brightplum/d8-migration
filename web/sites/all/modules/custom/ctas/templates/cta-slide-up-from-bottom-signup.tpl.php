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
  <div id="slim-display-content-mobile">
   <div class="content">
      <p><strong>Never Miss a Beat.</strong> Get our best delivered to your inbox.</p>
     <a id="cta-handle-mobile-close" href="#"><i class="fa fa-close"></i></a>
    </div>
    <div class="form">
      <form name='custom_form' action='https://info.commondreams.org/acton/eform/33231/0004/d-ext-0001' enctype="multipart/form-data" accept-charset="UTF-8">
        <input type="text" class="formFieldText formFieldLarge" id="form_0004_fld_0" name="Email" placeholder='Your email' required>
        <input id="form_0004_ao_submit_input" type="submit" name="Submit" value="Sign Up" onClick="doSubmit(document.getElementById('form_0004'))">
<input id="form_0004_fld_2-1" type="hidden" name="Subscription Choice" value="Weekly Newsletter" checked="checked" />
        <input id="form_0004_fld_2-0" type="hidden" name="Subscription Choice" value="Daily Newsletter" checked="checked" />
      </form>
    </div>

  </div>
</div>
