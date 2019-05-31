
(function ($) {
  var destRegExp = /[&?]destination=(.*?)(&|$)/;

  /**
   * Generate a popup login box for users to login.
   */
  Drupal.behaviors.iwapiLoginPopup = {
    attach: function (context, settings) {
      var settings = settings.iwLogin || {};
      var behavior = Drupal.behaviors.iwapiLoginPopup;
      var selector = "#iwlogin-popup-link, a[href*='/user/login'], a[href*='?q=user/login']";

      // Create the IW login dialog only once.
      if (!IWapi.loginDlg) {
        IWapi.loginDlg = new IWapi.ajax.Dialog(true);
        IWapi.loginDlg.container
          .attr('id', 'iwlogin-container')
          .dialog("option", "dialogClass", "login-dialog");
      }

      (settings.forceHttps && location.protocol != 'https:')
        ? $(selector, context).click(behavior.iFrameLogin)
        : $(selector, context).once('iw-modal', behavior.popupLogin);
    },

    /**
     * Loads and presents the user login in a pop-up modal box. This is more
     * efficient than the iFrame method but only works for same domain.
     */
    popupLogin: function() {
      var link = $(this);
      var href = link.attr('href');
      var matches = destRegExp.exec(href);
      var query   = {};

      if (matches && matches[1].length > 0) query.destination = matches[1];
      link.click(function() { IWapi.loginDlg.showLoading(); });

      // Create the AJAX handler call.
      Drupal.ajax[href] =  new Drupal.ajax(href, this, {
        url: IWapi.utils.buildURL("iwlogin/ajax/form", query),
        event:'click',
        progress: { type: 'throbber' }
      });

      Drupal.ajax[href].dialog = IWapi.loginDlg;
      Drupal.ajax[href].error  = IWapi.ajax.Action.prototype.error;
    },

    /**
     * Defers login to an iFrame, this is needed for cases where
     * we are using HTTP but need to use HTTPS for login.
     */
    iFrameLogin: function() {
      var matches = destRegExp.exec($(this).attr('href'));
      var origin  =  IWapi.utils.getCurrentPath();
      var query   = { };

      if (origin && origin.length > 0)      query.origin = origin;
      if (matches && matches[1].length > 0) query.destination = matches[1];

      IWapi.loginDlg.progress.show(); // show progress loading

      var iframe = $('iframe', IWapi.loginDlg.content);
      if (iframe.length == 0){
        var altLogin = $(
          '<div class="alternative-login"><div class="inner">If you are seeing ' +
          'an error or having trouble with the login dialog, try <a href="' +
          IWapi.utils.buildURL("user/login", query) + '">logging in here</a>.</div></div>'
        ).hide();

        // If an unusual amount of time has passed, allow users the chance try the standard login.
        IWapi.loginDlg.container.prepend(altLogin);
        var fallbackTimer = setTimeout(function() { altLogin.show(); }, 2500);

        // Create a new IFrame element to contain the login form.
        iframe = $('<iframe width="100%" height="0" scrolling="auto">').load(function() {
          IWapi.loginDlg.progress.hide();

          iframe.height(400).focus();
          clearTimeout(fallbackTimer);
          altLogin.show();

          // Update the content width, in case.
          IWapi.loginDlg.container.dialog('option', 'width', 600)
           .dialog('option', 'position', ['center', 100])
           .dialog('option', 'title', 'Login');
        });
        IWapi.loginDlg.content.append(iframe);
      }
      else {
        iframe.height(0);
      }

      iframe.attr('src', 'https://' + window.location.hostname + IWapi.utils.buildURL("iwlogin/iframe/page", query));
      IWapi.loginDlg.show();
      return false;
    }
  }
}(jQuery))
