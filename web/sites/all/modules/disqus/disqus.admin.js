/**
 * @file
 * JavaScript for the Disqus admin pages.
 */
(function ($) {
Drupal.behaviors.disqusFieldsetSummaries = {
  attach: function (context) {
      $('fieldset.comment-node-settings-form', context).drupalSetSummary(function (context) {
        if ($('#edit-disqus-status', context).is(':checked')) {
          return Drupal.checkPlain($('#edit-disqus-status', context).next('label').text());
        }
        else {
          return Drupal.checkPlain($('.form-item-comment input:checked', context).next('label').text());
        }
      });
    }
  }
})(jQuery);
