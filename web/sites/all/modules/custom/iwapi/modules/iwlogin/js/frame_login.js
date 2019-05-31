
(function ($) {
  /**
   * Generate a popup login box for users to login.
   */
  Drupal.behaviors.iwapiLoginFrame = {
    attach: function (context, settings) {

      // Make sure that links inside the IFrame, update the browser location.
      $('a', context).click(function() {
        window.top.location.href = $(this).attr('href');
        return false;
      });

      // Set the focus to be the first text input.
      $('form input[type=text]').first().focus();

      // Only gets attached to the iFrame version of the login box, it should
      // ignore context and hook to any form on the page. DO NOT restrict to a context!
      $('form').each(function() {
        var form = $(this);
        var form_id = form.attr('id');
        
        var element = { url: form.attr('action'), event: 'submit', progress: { type: 'throbber' }};

        Drupal.ajax[form_id] = new Drupal.ajax(form_id, this, element);
        Drupal.ajax[form_id].form = form;
        Drupal.ajax[form_id].error = IWapi.ajax.Action.prototype.error;

        $('input[type=submit], button', form).click(function(event) {
          $('#iwlogin-container').children().hide();
          $('#iwlogin-container').prepend('<div class="large-progress-icon login-progress">');
        	
          Drupal.ajax[form_id].element = this;
          this.form.clk = this;

          // An empty event means we were triggered via .click() and
          // in jquery 1.4 this won't trigger a submit.
          if (event.bubbles == undefined) {
            $(this.form).trigger('submit');
            return false;
          }
        });
      });
    }
  }

}(jQuery))
