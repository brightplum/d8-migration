(function ($) {
  Drupal.behaviors.slideUpFromBottomCta = {
    attach: function (context, settings) {
      if (settings.ctas.slide_up_from_bottom_signup) {
        // Campaign name.
        var campaign = settings.ctas.slide_up_from_bottom_signup.campaign;

        // Clear old storage settings.
        var removeArr = [];
        for (var i = 0; i < localStorage.length; i++){
          if (localStorage.key(i).indexOf(campaign) === -1 && localStorage.key(i).indexOf('slide-up-from-bottom-signup') !== -1) {
            removeArr.push(localStorage.key(i));
          }
        }

        // Iterate over arr and remove the items by key
        for (var i = 0; i <removeArr.length; i++) {
          localStorage.removeItem(removeArr[i]);
        }

        var block_id = "#block-block-" + settings.ctas.slide_up_from_bottom_signup.delta;
        var hide_banner = localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.hide_banner');

        // Only show after one page view.
        if (localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.page_views')) {
          // count = localStorage.getItem('commondreams.cta.' + block_id + '.page_views');
          // count++;
          // localStorage.setItem('commondreams.cta.' + block_id + '.page_views', count);
        }
        else {
          localStorage.setItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.page_views', 1);
          hide_banner = true;
        }

        if (localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.force-slim')) {
          $(block_id).find('.cta-handle').hide();
        }

        // Remove if set to hide.
        if (hide_banner) {
          $(block_id).remove();
        }
        else {
          $('body').addClass('cta-slide-up-from-bottom-applied');
          $(block_id)
            .once()
            .addClass('cta-processed cta-slide-up-from-bottom')
            .appendTo('body')

            .find('.cta-handle a').not('.already_donated').click(function(e) {
              $(this).parents(block_id).toggleClass('slim-display-showing');
              if ($(this).parents(block_id).hasClass('slim-display-showing')) {
                $(this).parents(block_id).find('#cta-handle-close').show();
                $(this).parents(block_id).find('#cta-handle-open').hide();
              }
              else {
                $(this).parents(block_id).find('#cta-handle-close').hide();
                $(this).parents(block_id).find('#cta-handle-open').show();
              }
              e.preventDefault();
            });

          // Force super slim.
          $(block_id).find('#cta-handle-mobile-close').click(function(e) {
            $(block_id).toggleClass('super-slim-display-showing');
            localStorage.setItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.force-slim', true);
            $(block_id).find('.cta-handle').hide();
            e.preventDefault();
          });

          // On load, only if its not been shown.
          if (localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign) !== 'shown') {
            if ($(document).width() > 768) {
                $(block_id).removeClass('showing')
                  .find('#cta-handle-open').hide().parent().find('#cta-handle-close').show();
                $('body').removeClass('cta-slide-up-from-bottom-showing');
            }
            else {
                $(block_id).toggleClass('slim-display-showing')
                  .find('#cta-handle-open').show().parent().find('#cta-handle-close').hide();
                $('body').toggleClass('cta-slide-up-from-bottom-showing');
            }
            localStorage.setItem('commondreams.slide-up-from-bottom-signup-' + campaign, 'shown');
          }

          // Super slim.
          if (localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.force-slim')) {
            $(block_id).toggleClass('super-slim-display-showing');
          }

          // Waypoints animation.
          var waypoint = new Waypoint({
            element: document.getElementsByClassName('l-content'),
            handler: function(direction) {
              if (direction === 'down') {
                if (!$(block_id).hasClass('showing') && !$('body').hasClass('slide-out-menu-open')) {
                  if (!localStorage.getItem('commondreams.slide-up-from-bottom-signup-' + campaign + '.force-slim')) {
                    $(block_id).addClass('slim-display-showing');
                    $(block_id).find('#cta-handle-close').show();
                    $(block_id).find('#cta-handle-open').hide();
                  }
                }
              }
              if (direction === 'up') {
                $(block_id).removeClass('slim-display-showing');
                $(block_id).find('#cta-handle-close').hide();
                $(block_id).find('#cta-handle-open').show();
              }
            }
          });

          slimHeaderOriginalContent = $(block_id).find('#slim-display-content').html();
          var continuousElements = document.getElementsByClassName('slim-header-content');
          for (var i = 0; i < continuousElements.length; i++) {

            new Waypoint({
              element: continuousElements[i],
              offset: 10,
              //enabled: false,
              handler: function(direction) {
                if ($(block_id).find('#slim-display-content').length) {

                  if (direction === 'down') {
                    var content = $(this.element).data('cta-slim-header');
                    $(block_id).find('#slim-display-content').html(content);
                  }
                  if (direction === 'up') {
                    $(block_id).find('#slim-display-content').html(slimHeaderOriginalContent);
                  }
                }
              }
            })
          }
        }
      }
    }
  }

  // function () {
  //
  // }

})(jQuery);
