
(function ($) {

  Drupal.behaviors.omage_dreams_homepage = {
    attach: function () {
      // Need to enumerate through these blocks so I can class them individually.
      // nth-child doesn't work in IE8 (grr)
      $(".view-display-id-categorized_news_remaining   .views-row, .view-display-id-news_block_therest .views-row").each(function (index) {
        if ((index + 1) % 4 === 0) {
          $(this).addClass("views-row-last");
        }
      });


      $(".view-display-id-categorized_news_tertiary .views-row, .view-display-id-news_block_tertiary  .views-row").each(function (index) {
        if ((index + 1) % 2 === 0) {
          $(this).addClass("views-row-last");
        }
      });

    }
  };



})(jQuery);


