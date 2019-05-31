(function ($) {

  Drupal.behaviors.contentMobileParallax = {
    attach: function (context, settings) {
      if (settings.ctas.content_mobile_parallax) {
        // Campaign name.
        var campaign = settings.ctas.content_mobile_parallax.campaign;

        // Clear old storage settings.
        var removeArr = [];
        for (var i = 0; i < localStorage.length; i++) {
          if (localStorage.key(i).indexOf(campaign) === -1 && localStorage.key(i).indexOf('content_mobile_parallax') !== -1) {
            removeArr.push(localStorage.key(i));
          }
        }

        // Iterate over arr and remove the items by key
        for (var i = 0; i < removeArr.length; i++) {
          localStorage.removeItem(removeArr[i]);
        }

        var block_id = "#block-block-" + settings.ctas.content_mobile_parallax.delta;

        // $(block_id + " .parallax-content").paroller({
        //   factor: settings.ctas.content_mobile_parallax.factor,
        //   factorXs: settings.ctas.content_mobile_parallax.factor_xs,
        //   factorSm: settings.ctas.content_mobile_parallax.factor_sm,
        //   factorMd: settings.ctas.content_mobile_parallax.factor_md,
        //   factorLg: settings.ctas.content_mobile_parallax.factor_lg,
        //   type: settings.ctas.content_mobile_parallax.type,
        //   direction: settings.ctas.content_mobile_parallax.direction,
        //   transition: settings.ctas.content_mobile_parallax.transition
        // });
        var hide_banner = localStorage.getItem("commondreams.content-mobile-parallax-" + campaign + ".hide_banner");
        if ($(block_id).length) {
          if (hide_banner) {
            $(block_id).remove();
          }
          else {
            //$(block_id + " .parallax-content").paroller();
            $(block_id).find(".hide-banner").click(function(e) {
              localStorage.setItem("commondreams.content-mobile-parallax-" + campaign + ".hide_banner", "1");
              $(block_id).remove();
              e.preventDefault();
            });
          }
        }


        // Way points animation.
        if (settings.ctas.slide_up_from_bottom || settings.ctas.slide_up_from_bottom_signup) {
          var slideupBlockId = '';
          if (settings.ctas.slide_up_from_bottom) {
            slideupBlockId = "#block-block-" + settings.ctas.slide_up_from_bottom.delta;
          }
          else {
            slideupBlockId = "#block-block-" + settings.ctas.slide_up_from_bottom_signup.delta;
          }

          if ($(slideupBlockId).length) {

            // Enter viewport.
            new Waypoint({
              element: document.getElementById("block-block-" + settings.ctas.content_mobile_parallax.delta),
              offset: function () {
                return Waypoint.viewportHeight();
              },
              handler: function (direction) {
                if (direction === 'down') {
                  $(slideupBlockId).removeClass('slim-display-showing');
                  $(slideupBlockId).find('#cta-handle-close').hide();
                  $(slideupBlockId).find('#cta-handle-open').show();
                }
                if (direction === 'up') {
                  if ($(slideupBlockId).length) {
                    if (!$(slideupBlockId).hasClass('showing') && !$('body').hasClass('slide-out-menu-open')) {
                      if (!localStorage.getItem('commondreams.content-mobile-parallax-' + campaign + '.force-slim')) {
                        $(slideupBlockId).addClass('slim-display-showing');
                        $(slideupBlockId).find('#cta-handle-close').show();
                        $(slideupBlockId).find('#cta-handle-open').hide();
                      }
                    }
                  }
                }
              }
            });

            // Exist viewport.
            new Waypoint({
              element: document.getElementById("block-block-" + settings.ctas.content_mobile_parallax.delta),
              offset: function () {
                return Waypoint.viewportHeight() * -1;
              },
              handler: function (direction) {
                if (direction === 'down') {
                  if ($(slideupBlockId).length) {
                    if (!$(slideupBlockId).hasClass('showing') && !$('body').hasClass('slide-out-menu-open')) {
                      if (!localStorage.getItem('commondreams.content-mobile-parallax-' + campaign + '.force-slim')) {
                        $(slideupBlockId).addClass('slim-display-showing');
                        $(slideupBlockId).find('#cta-handle-close').show();
                        $(slideupBlockId).find('#cta-handle-open').hide();
                      }
                    }
                  }
                }
                if (direction === 'up') {
                  if ($(slideupBlockId).length) {
                    $(slideupBlockId).removeClass('slim-display-showing');
                    $(slideupBlockId).find('#cta-handle-close').hide();
                    $(slideupBlockId).find('#cta-handle-open').show();
                  }
                }
              }
            });

          }
        }
      }
    }
  };
})(jQuery);

