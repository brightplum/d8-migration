<?php
/**
 * @file
 * Template for the Donate Page.
 */
?>
<div id="cta-donate-page" class="<?php print (isset($_GET['success']) ? 'thank-you-page' : ''); ?>">
  <div id="hero">
    <div class="container">
      <div class="row">
        <div class="col-12">
          <div class="d-flex w-100 p-3 mx-auto flex-column">
            <header class="masthead mb-auto">
              <div class="inner">
                <h3 class="masthead-brand"><img src="/sites/all/themes/omega_dreams/images/cd_logo_white.png"
                                                style="width: 125px; height: auto"/></h3>
              </div>
            </header>

            <?php if (isset($_GET['success']) && $_GET['success'] == "1") : ?>
              <main role="main" class="inner cover p-0 py-md-3">
                <div class="content text-white mb-3">
                  <h1 class="cover-heading">Thank you for
                    your <?php print ($_GET['campaign']) && $_GET['campaign'] == '2' ? 'monthly' : ''; ?> support!</h1>
                  <p>You will receive a confirmation via email when we have processed your donation.
                    Contributions from readers like you keep Common Dreams strong.</p>
                </div>
              </main>
            <?php else : ?>
              <main role="main" class="inner cover p-0 py-md-5">
                <div class="content text-white mb-3">
                  <h1 class="cover-heading">YES! I support this work</h1>
                </div>
                <div class="form">
                  <?php print render($form); ?>
                </div>

                <div class="other-options mt-3 mt-md-5">
                  <h3 class="text-white text-center">OR,</h3>
                  <h4 class="text-white text-center">Use one of our other payment options</h4>
                  <div class="donation-options d-flex flex-row justify-content-center align-items-center mt-3">
                    <a href="https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4C248HGKCC76S&custom=<?php print (isset($_GET['cta-location']) ? $_GET['cta-location'] : ''); ?>" target="_blank" class="btn btn-fundraiser"><i class="paypal-icon"></i></a>
                    <a href="https://secure.actblue.com/donate/common-dreams-february-fundraiser?refcode=<?php print (isset($_GET['cta-location']) ? $_GET['cta-location'] : ''); ?>" target="_blank" class="btn btn-fundraiser"><i class="actblue-icon"></i></a>
                  </div>
                </div>
              </main>
            <?php endif; ?>
          </div>
        </div>
      </div>
    </div>
  </div>

  <?php if (!isset($_GET['success'])) : ?>
  <div id="extra-content" class="py-5">
    <div class="container">
      <div class="row justify-content-center">
        <div class="col-md-10">
          <p>Common Dreams is a qualified 501(c)(3) tax-exempt organization and donations are tax-deductible to the full
            extent allowed under the law.</p>
          <p>If you prefer to donate by phone or check: you can reach us at 207-775-0488, or<br>
            Common Dreams<br>
            PO Box 443<br>
            Portland, ME 04112, USA<br>
          </p>
        </div>
      </div>
    </div>
  </div>
  <?php else : ?>
  <div id="share-action-wrapper">
    <div class="d-flex pt-3 justify-content-center align-items-center share-title">
      <i class="fa fa-comments"></i> <span>=</span> <i class="fa fa-heart-o"></i>
    </div>
    <h2 class="text-center">Spread the Word</h2>
    <div class="d-flex justify-content-center align-items-center share-buttons mt-3">
      <div class="fb-share-button"
           data-href="https://www.commondreams.org/<?php print current_path(); ?>"
           data-layout="button"
           data-size="large">
      </div>
      <a id="twitter-share-button" data-size="large" class="twitter-share-button btn btn-large btn-primary mx-3" href="https://www.commondreams.org/<?php print current_path(); ?>"><i class="fa fa-twitter-square"></i> Twitter</a>
    </div>
    <p class="mt-5 h3 text-center">Spread the word and tell your friends and family that you support Common Dreams</p>
  </div>
  <?php endif; ?>

</div>

<!-- Load Facebook SDK for JavaScript -->
<div id="fb-root"></div>
<script>(function(d, s, id) {
    var js, fjs = d.getElementsByTagName(s)[0];
    if (d.getElementById(id)) return;
    js = d.createElement(s); js.id = id;
    js.src = "https://connect.facebook.net/en_US/sdk.js#xfbml=1&version=v3.0";
    fjs.parentNode.insertBefore(js, fjs);
  }(document, 'script', 'facebook-jssdk'));
</script>
<script>
  window.twttr = (function(d, s, id) {
    var js, fjs = d.getElementsByTagName(s)[0],
      t = window.twttr || {};
    if (d.getElementById(id)) return t;
    js = d.createElement(s);
    js.id = id;
    js.src = "https://platform.twitter.com/widgets.js";
    fjs.parentNode.insertBefore(js, fjs);

    t._e = [];
    t.ready = function(f) {
      t._e.push(f);
    };

    return t;
  }(document, "script", "twitter-wjs"));
</script>
