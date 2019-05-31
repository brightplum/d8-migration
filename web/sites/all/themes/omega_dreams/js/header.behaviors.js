/*global Waypoint */

var slimHeaderOriginalContent = '';

(function ($) {

    /**
     * Behaviors are Drupal's way of applying JavaScript to a page. In short, the
     * advantage of Behaviors over a simple 'document.ready()' lies in how it
     * interacts with content loaded through Ajax. Opposed to the
     * 'document.ready()' event which is only fired once when the page is
     * initially loaded, behaviors get re-executed whenever something is added to
     * the page through Ajax.
     *
     * You can attach as many behaviors as you wish. In fact, instead of overloading
     * a single behavior with multiple, completely unrelated tasks you should create
     * a separate behavior for every separate task.
     *
     * In most cases, there is no good reason to NOT wrap your JavaScript code in a
     * behavior.
     *
     * @param context
     *   The context for which the behavior is being executed. This is either the
     *   full page or a piece of HTML that was just added through Ajax.
     * @param settings
     *   An array of settings (added through drupal_add_js()). Instead of accessing
     *   Drupal.settings directly you should use this because of potential
     *   modifications made by the Ajax callback that also produced 'context'.
     */
    Drupal.behaviors.headerBehavior = {
        attach: function (context, settings) {

            // Sticky header.

            // Only if super header is not showing.
            if (!$('.cta-super-header-applied').length) {
              new Waypoint(
                {
                  element: document.getElementsByClassName('l-page'),
                  offset: 100,
                  handler: function (direction) {
                    if (direction === 'down') {
                      $('body').addClass('header-container-slim');
                    }
                    if (direction === 'up') {
                      $('body').removeClass('header-container-slim');
                    }
                  }
                }
              );
            }


            // Search toggle.
            $('#search-toggle').click(function (e) {
                $(this).toggleClass('active');
                $('#header-content').toggleClass('showing');
                e.preventDefault();
            });

            // Slide out nav.
            $('#slide-menu-button').click(function (e) {
                $(this).toggleClass('open');
                $('body').toggleClass('slide-out-menu-open');
                $('#navigation').toggleClass('open');
                e.preventDefault();
            });

            $('#close-slide-menu-button').click(function (e) {
                $('#navigation').removeClass('open');
                e.preventDefault();
            });

            $(document).keyup(function (e) {
                // esc
                if (e.keyCode === 27) {
                    $('#slide-menu-button').removeClass('open');
                    $('body').removeClass('slide-out-menu-open');
                    $('#navigation').removeClass('open');
                }
            });

        }
    };

})(jQuery);
