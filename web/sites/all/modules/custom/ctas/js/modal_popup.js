(function ($) {
  Drupal.behaviors.modalPopup = {
    attach: function (context, settings) {
      if (settings.ctas.modal_popup) {
        // Campaign name.
        var campaign = settings.ctas.modal_popup.campaign;

        // Clear old storage settings.
        var removeArr = [];
        for (var i = 0; i < localStorage.length; i++){
          if (localStorage.key(i).indexOf(campaign) === -1 && localStorage.key(i).indexOf('modal_popup') !== -1) {
            removeArr.push(localStorage.key(i));
          }
        }

        // Iterate over arr and remove the items by key
        for (var i = 0; i <removeArr.length; i++) {
          localStorage.removeItem(removeArr[i]);
        }

        var block_id = "#block-ctas-" + settings.ctas.modal_popup.delta.replace(/_/g, "-");
        $(block_id)
          .once()
          .addClass('cta-processed modal-popup')
          .hide()
          .appendTo('body');

        // Check for "already dontate" flag.
        var already_donated = localStorage.getItem('commondreams.modal_popup-' + campaign + '.already_donated');

        // Check for cd-origin is not set to "rss". We dont want to show this if user
        // has come in from an link in RSS. This most likely means they get our Email
        // Newsletter that pulls from our RSS feed.
        var cd_origin = settings.ctas.modal_popup.cd_origin;
        if (cd_origin) {
          localStorage.setItem('commondreams.modal_popup-' + campaign + '.cd-origin', cd_origin);
        }

        // Continue if "already donated" is not set AND cd_origin is not "rss".
        if (!already_donated && localStorage.getItem('commondreams.modal_popup-' + campaign + '.cd-origin') !== 'rss') {

          // Count the page views.
          var count = 1;
          if (localStorage.getItem('commondreams.modal_popup-' + campaign + '.page_views')) {
            count = localStorage.getItem('commondreams.modal_popup-' + campaign + '.page_views');
            count++;
            localStorage.setItem('commondreams.modal_popup-' + campaign + '.page_views', count);
          }
          else {
            localStorage.setItem('commondreams.modal_popup-' + campaign + '.page_views', 1);
          }

          if (count >= parseFloat(settings.ctas.modal_popup.after_page_views) || !settings.ctas.modal_popup.after_page_views) {
            // Only for > 768px viewports.
            var w = Math.max(document.documentElement.clientWidth, window.innerWidth || 0);
            if (w > 768) {
              $(block_id).show();
              bioEp.init({
                html: '',
                css: '',
                showOnDelay: true
              });
            }
          }
        }
        else {
          $(block_id).remove();
        }

        // Already donated button.
        $(block_id).find("#bio_ep_close").click(function(e) {
          localStorage.setItem('commondreams.modal_popup-' + campaign + '.already_donated', 1);
          //bioEp.hidePopup();
          // Hide all other ctas.
          e.preventDefault();
        });

        // Extra close modal functionality.
        $("#close-modal").click(function(e) {
          localStorage.setItem('commondreams.modal_popup-' + campaign + '.already_donated', 1);
          bioEp.hidePopup();
          e.preventDefault();
        });

      }
    }
  }
})(jQuery);
