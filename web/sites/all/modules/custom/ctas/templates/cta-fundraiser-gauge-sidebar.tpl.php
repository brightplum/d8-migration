<?php
/**
 * @file
 * Template for the CTA Fundraiser: Gauge sidebar
 */
$max_number = number_format($max);
if ($max > 999) {
  $max_number = number_format($max / 1000) . 'K';
}
?>
<?php print $content; ?>
<div class="cta-sidebar-gauge">
  <div class="goal">Goal: <?php print $max_number; ?></div>
  <div class="progress">
    <div class="progress-bar progress-bar-striped progress-bar-animated" role="progressbar" style="width: <?php print round($percent_complete); ?>%" aria-valuenow="<?php print $current; ?>" aria-valuemin="0" aria-valuemax="<?php print $max; ?>"><?php print round($percent_complete); ?>%</div>
  </div>
  <?php if ($raised) : ?>
  <div class="text-center small mt-1"><?php print $raised; ?></div>
  <?php endif; ?>
</div>
