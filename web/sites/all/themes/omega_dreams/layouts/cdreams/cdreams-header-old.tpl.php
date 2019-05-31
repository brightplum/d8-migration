<?php
/**
 * @file
 * Partial: CDreams Header.
 */
?>
<div id="header-container">
  <header class="l-header" role="banner">
    <div class="l-over-branding">
      <?php print render($page['over_branding']); ?>
    </div>
    <div class="l-branding" itemprop="publisher" itemscope itemtype="https://schema.org/Organization">
      <meta itemprop="name" content="Common Dreams">
      <?php if ($logo): ?>
        <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="site-logo">
            <span itemprop="logo" itemscope itemtype="https://schema.org/ImageObject">
              <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
              <meta itemprop="url" content="<?php print $logo; ?>">
              <meta itemprop="width" content="600">
              <meta itemprop="height" content="60">
            </span>
        </a>
      <?php endif; ?>


      <?php if ($site_name || $site_slogan): ?>
        <?php if ($site_name): ?>
          <h1 class="site-name">
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
          </h1>
        <?php endif; ?>

        <?php if ($site_slogan): ?>
          <h2 class="site-slogan"><?php print $site_slogan; ?></h2>
        <?php endif; ?>
      <?php endif; ?>

      <?php print render($page['branding']); ?>
    </div>

    <?php print render($page['header']); ?>

  </header>
</div>

<div id="navigation-container">
  <?php print render($page['navigation']); ?>
</div>
