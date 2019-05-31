(function ($) {
  Drupal.behaviors.superHeader = {
    attach: function (context, settings) {
      if (settings.ctas.super_header) {
        var block_id = "#block-block-" + settings.ctas.super_header.delta;

        var hide_banner = localStorage.getItem('commondreams.cta.hide_super_header');
        if (!hide_banner) {
          $('body').addClass('cta-super-header-applied header-container-slim');
          $(block_id)
            .once()
            .addClass('cta-processed cta-super-header')
            .insertBefore('#header-container');

          if ($(document).width() > 767) {
            $(block_id).toggleClass('showing');
            $('body').toggleClass('cta-super-header-showing');
          }

          //$(block_id + " .parallax-content").paroller();

          // Waypoints animation.
          var waypoint = new Waypoint({
            element: document.getElementById('header-container'),
            offset: 0,
            handler: function(direction) {
              if (direction === 'down') {
                if ($(document).width() > 767) {
                  $('body').removeClass('cta-super-header-showing');
                  $('.cta-super-header').removeClass('showing');
                }
              }
              if (direction === 'up') {
                if ($(document).width() > 767) {
                  $('body').addClass('cta-super-header-showing');
                  $('.cta-super-header').addClass('showing');
                }
              }
            }
          })
        }
        else {
          $(block_id).remove();
        }

      }
    }
  }
})(jQuery);
