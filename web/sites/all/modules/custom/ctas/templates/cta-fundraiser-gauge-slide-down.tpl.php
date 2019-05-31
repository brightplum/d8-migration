<?php
/**
 * @file
 * Template for the CTA Fundraiser: Gauge Slide Down
 */
?>

<?php print $content; ?>
<div class="cta-sidebar-gauge">
  <div class="progress">
    <div class="progress-bar progress-bar-striped progress-bar-animated" role="progressbar" style="width: <?php print round($percent_complete, 2); ?>%" aria-valuenow="<?php print $current; ?>" aria-valuemin="0" aria-valuemax="<?php print $max; ?>"><?php print round($percent_complete, 2); ?>%</div>
  </div>
  <div class="text-center small mt-1">We've raised $<?php print number_format($current, 2, ".", ","); ?></div>
</div>
