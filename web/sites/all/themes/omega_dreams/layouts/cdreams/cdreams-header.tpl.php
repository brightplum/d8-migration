<?php
/**
 * @file
 * Partial: CDreams Header.
 */
?>
<div id="header-container" class="header-enhanced">
  <header class="l-header" role="banner">
    <div class="l-over-branding">
      <?php print render($page['over_branding']); ?>
    </div>
    <div class="l-branding" itemprop="publisher" itemscope itemtype="https://schema.org/Organization">
      <meta itemprop="name" content="Common Dreams">
      <?php if ($logo): ?>
        <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo">
          <span itemprop="logo" itemscope itemtype="https://schema.org/ImageObject">
<!--            <img src="--><?php //print $logo; ?><!--" alt="--><?php //print t('Home'); ?><!--" />-->
            <img src="<?php print base_path() . path_to_theme() . '/images/cd_logo_white.jpg'; ?>" alt="<?php print t('Home'); ?>" />
            <meta itemprop="url" content="<?php print $logo; ?>">
            <meta itemprop="width" content="600">
            <meta itemprop="height" content="60">
          </span>
        </a>
      <?php endif; ?>

      <div class="nav">
        <?php print render($page['navigation']); ?>
      </div>

      <div class="search">
        <a href="#"><i class="fa fa-search"></i> </a>
      </div>

      <div class="social" itemscope="" itemtype="http://schema.org/Organization">
        <a itemprop="sameAs" target="_blank" href="http://www.facebook.com/commondreams.org"><em class="fa fa-facebook-square"></em></a><a itemprop="sameAs" target="_blank" href="http://www.twitter.com/commondreams"><em class="fa fa-twitter-square"></em></a><a target="_blank" href="/rss.xml"><em class="fa fa-rss-square"></em></a><a itemprop="sameAs" target="_blank" href="https://plus.google.com/u/0/b/112945048567964551573?rel=publisher"><em class="fa fa-google-plus-square"></em></a>
      </div>
    </div>

  </header>

  <div id="search">
    <form>
      <input type="text" value="" placeholder="Search Common Dreams"><i class="fa fa-search"></i>
    </form>
  </div>
</div>
