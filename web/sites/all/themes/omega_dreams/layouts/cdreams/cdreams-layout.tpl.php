<?php global $conf; ?>
<?php global $hide_page_title; ?>
<?php if ($page['over_branding']) : ?>
  <div class="l-over-branding">
    <?php print render($page['over_branding']); ?>
  </div>
<?php endif; ?>

<?php if ($page['navigation']) : ?>
  <div id="navigation" class="slide-menu">
<!--    <div id="close-slide-menu-button"><a href="#"><i class="fa fa-window-close-o"></i></a></div>-->
    <div class="slide-menu-container">
      <?php print render($page['navigation']); ?>
    </div>
  </div>
<?php endif; ?>

<div<?php print $attributes; ?>>
  <div id="header-container" class="fundraiser">
    <div id="header-top">
      <div class="l-header">
        <div class="col header-left">
          <div id="mobile-menu-wrapper">
            <a id="slide-menu-button" href="#">
              <span class="slide-menu-lines"></span>
              <span class="label">Sections</span>
            </a>
          </div>

          <div id="search-icon">
            <a id="search-toggle" href="#"><i class="fa fa-search"></i> </a>
          </div>
        </div>

        <div class="col header-middle">
          <div itemprop="publisher" itemscope itemtype="https://schema.org/Organization">
            <meta itemprop="name" content="Common Dreams">
            <meta itemprop="url" content="https://www.commondreams.org/sites/default/files/cd_stacked_white_600.png">
            <?php if ($logo): ?>
              <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo">
                  <meta itemprop="foundingdate" content="1997" />
                  <span itemprop="logo" itemscope itemtype="https://schema.org/ImageObject">
                  <img src="/<?php print path_to_theme(); ?>/images/cd_stacked_blue-new.jpg" alt="<?php print t('Home'); ?>" />
                  <meta itemprop="url" content="https://www.commondreams.org/sites/default/files/cd_stacked_white_600.png">
                  <meta itemprop="width" content="600">
                  <meta itemprop="height" content="60">
                </span>
              </a>
            <?php endif; ?>
            <?php print render($page['branding']); ?>
          </div>
        </div>

        <div class="col header-right">
          <a class="button button--secondary" href="/email-subscription">SUBSCRIBE</a>
          <a class="button button--primary" href="/donate?cta-location=header" target="_blank">DONATE</a>
        </div>
      </div>

    </div>

    <?php if ($page['header']): ?>
      <div id="header-content">
        <?php print render($page['header']); ?>
      </div>
    <?php endif; ?>

<!--    <div id="navigation-container">-->
<!--      --><?php //print render($page['navigation']); ?>
<!--    </div>-->
  </div>

  <div id="highlighted-container">
      <?php print render($page['highlighted']); ?>
  </div>

  <div class="l-main">
    <div class="l-content" role="main">
      <a id="main-content"></a>
      <?php if (!$hide_page_title) : ?>
      <?php print render($title_prefix); ?>
      <?php if ($title): ?>
        <h1><?php print $title; ?></h1>
      <?php endif; ?>
      <?php print render($title_suffix); ?>
      <?php endif; ?>
      <?php print $messages; ?>
      <?php print render($tabs); ?>
      <?php print render($page['help']); ?>
      <?php if ($action_links): ?>
        <ul class="action-links"><?php print render($action_links); ?></ul>
      <?php endif; ?>
      <?php print render($page['content']); ?>
      <?php print $feed_icons; ?>
    </div>

    <?php print render($page['sidebar_first']); ?>
    <?php print render($page['sidebar_second']); ?>
  </div>

  <footer class="l-footer" role="contentinfo">
    <?php print render($page['footer']); ?>
  </footer>

  <?php if ($conf['env'] != 'dev') : ?>
  <!-- START Parse.ly Include: Standard -->
  <div id="parsely-root" style="display: none">
    <span id="parsely-cfg" data-parsely-site="commondreams.org"></span>
  </div>
  <script>
    (function(s, p, d) {
      var h=d.location.protocol, i=p+"-"+s,
        e=d.getElementById(i), r=d.getElementById(p+"-root"),
        u=h==="https:"?"d1z2jf7jlzjs58.cloudfront.net"
          :"static."+p+".com";
      if (e) return;
      e = d.createElement(s); e.id = i; e.async = true;
      e.src = h+"//"+u+"/p.js"; r.appendChild(e);
    })("script", "parsely", document);
  </script>
  <!-- END Parse.ly Include: Standard -->
  <?php endif; ?>
</div>
