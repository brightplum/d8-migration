
<div<?php print $attributes; ?>>
  <h2 class="results-title">
    <strong class="results-count"><?php print number_format($count); ?></strong> 
    results for <?php print $summary; ?>
  </h2>
  
  <div<?php print $content_attributes; ?>>
    <div class="results-content clearfix">
      <?php print render($content); ?>
    </div>
    <?php print $pager; ?>
  </div>
</div>