(function ($) {
  Drupal.behaviors.slideDownFromTopCta = {
    attach: function (context, settings) {
      if (settings.ctas.slide_down_from_top) {
        // Campaign name.
        var campaign = settings.ctas.slide_down_from_top.campaign;

        var shown = localStorage.getItem('commondreams.slide-down-from-top-' + campaign);

        // Clear old storage settings.
        var removeArr = [];
        for (var i = 0; i < localStorage.length; i++){
          if (localStorage.key(i).indexOf(campaign) === -1 && localStorage.key(i).indexOf('slide-down-from-top') !== -1) {
            removeArr.push(localStorage.key(i));
          }
        }

        // Iterate over arr and remove the items by key
        for (var i = 0; i <removeArr.length; i++) {
          localStorage.removeItem(removeArr[i]);
        }

        var block_id = "#block-block-" + settings.ctas.slide_down_from_top.delta;
        if (settings.ctas.slide_down_from_top.delta === 'cta_fundraiser_gauge_slide_down') {
          block_id = '#block-ctas-' + settings.ctas.slide_down_from_top.delta.replace(/_/g, "-");
        }

        var hide_banner = localStorage.getItem('commondreams.slide-down-from-top-' + campaign + '.hide_banner');
        if (!hide_banner) {
          $('body').addClass('cta-slide-down-from-top-applied');
          $(block_id)
            .once()
            .addClass('cta-processed cta-slide-down-from-top')
            .appendTo('#header-container')
            .find('.cta-handle a').not('.already_donated').click(function() {
              $(this).parents(block_id).toggleClass('showing')
                .find('#cta-handle-open').toggle().parent().find('#cta-handle-close').toggle();
              $('body').toggleClass('cta-slide-down-from-top-showing');
            });

          $(block_id).find('#slider_close_button').click(function(e) {
            $(this).parents(block_id).toggleClass('showing')
                .find('#cta-handle-open').toggle().parent().find('#cta-handle-close').toggle();
            $('body').toggleClass('cta-slide-down-from-top-showing');
            e.preventDefault();
          });

          // On load, only if its not been shown.
          if (localStorage.getItem('commondreams.slide-down-from-top-' + campaign) !== 'shown') {
            if ($(document).width() > 768) {
                $(block_id).toggleClass('showing')
                    .find('#cta-handle-open').hide().parent().find('#cta-handle-close').show();
                $('body').toggleClass('cta-slide-down-from-top-showing');
            }
            else {
                $(block_id).toggleClass('slim-display-showing')
                    .find('#cta-handle-open').show().parent().find('#cta-handle-close').hide();
                $('body').toggleClass('cta-slide-down-from-top-showing');
            }
          }
          localStorage.setItem('commondreams.slide-down-from-top-' + campaign, 'shown');

          // Already donated button.
          $(block_id).find(".hide-banner").click(function(e) {
            localStorage.setItem('commondreams.slide-down-from-top-' + campaign + '.hide_banner', 1);
            $(block_id).remove();
            e.preventDefault();
          });

          // Waypoints animation.
          var waypoint = new Waypoint({
            element: document.getElementsByClassName('l-content'),
            handler: function(direction) {
              if (direction === 'down') {
                if (!$(block_id).hasClass('showing') && !$('body').hasClass('slide-out-menu-open')) {
                  $(block_id).addClass('slim-display-showing');
                }
              }
              if (direction === 'up') {
                $(block_id).removeClass('slim-display-showing');
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
