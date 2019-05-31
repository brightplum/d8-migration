

(function($) {

  IWapi.ajax = IWapi.ajax || {};
  
  /**
   * Directly callable AJAX trigger, which works when you are
   * triggering an AJAX command without need of an event, or
   * as shared between events.
   */
  IWapi.ajax.Action = function(base, element, element_settings) {
    var defaults = {
      url: 'system/ajax',
      selector: '#' + base,
      effect: 'none',
      speed: 'none',
      method: 'replaceWith',
      progress: {
        type: 'throbber',
        message: Drupal.t('Please wait...')
      },
      submit: { 'js': true }
    };
    $.extend(this, defaults, element_settings);

    // Save original user settings.
    this.element_settings = element_settings;

    this.url = element_settings.url.replace(/\/nojs(\/|$|\?|&|#)/g, '/ajax$1');
    this.wrapper = '#' + element_settings.wrapper;
    
    // This is where we primarily diverge from the Drupal handling.
    // When there is no element, much of Drupal's handling is unnecessary.
    if (element) {
      this.element = element; // attach progress extra element processing.
      
      // If there isn't a form, jQuery.ajax() will be used instead, allowing us to
      // bind Ajax to links as well.
      if (this.element.form) {
        this.form = $(this.element.form);
      }
	  
      // Use the Drupal AJAX handlers instead of our own ones.
      this.beforeSubmit = Drupal.ajax.prototype.beforeSubmit;
      this.beforeSend   = Drupal.ajax.prototype.beforeSend;
      this.success      = Drupal.ajax.prototype.success;
      this.complete     = Drupal.ajax.prototype.complete;
    }
    else {
      delete this.progress;
    }
    
    // These items are always relevant and need to be directly inherited.
    this.beforeSerialize = Drupal.ajax.prototype.beforeSerialize;
    this.commands  = Drupal.ajax.prototype.commands;
    this.getEffect = Drupal.ajax.prototype.getEffect;
    
    // Set the options for the ajaxSubmit function.
    // The 'this' variable will not persist inside of the options object.
    var ajax = this;
    ajax.options = {
      url: ajax.url,
      dataType: 'json',
      data: ajax.submit,
      beforeSerialize: function (element_settings, options) {
        return ajax.beforeSerialize(element_settings, options);
      },
      beforeSubmit: function (form_values, element_settings, options) {
        ajax.ajaxing = true;
        return ajax.beforeSubmit(form_values, element_settings, options);
      },
      beforeSend: function (xmlhttprequest, options) {
        ajax.ajaxing = true;
        return ajax.beforeSend(xmlhttprequest, options);
      },
      success: function (response, status) {
        // Sanity check for browser support (object expected).
        // When using iFrame uploads, responses must be returned as a string.
        if (typeof response == 'string') {
	      response = $.parseJSON(response);
	    }
	    return ajax.success(response, status);
	  },
	  complete: function (response, status) {
	    ajax.ajaxing = false;
	    if (status == 'error' || status == 'parsererror') {
	      return ajax.error(response, ajax.url);
	    }
	  }
	};

    // Allow a simple + cacheable GET requests.
    if (element_settings.useGET) {
      this.options.type = 'GET';
      this.options.data = false;
      this.beforeSerialize = IWapi.ajax.Action.beforeSerialize;
    }
    else {
      this.options.type = 'POST';
    }

    // If necessary, prevent the browser default action of an additional event.
    // For example, prevent the browser default action of a click, even if the
    // AJAX behavior binds to mousedown.
    if (element_settings.prevent) {
      $(ajax.element).bind(element_settings.prevent, false);
    }    
  };
  
  
  // In order to allow GET method AJAX requests, we need to prevent the sending of
  // additional data added by the standard beforeSerialize method.
  IWapi.ajax.Action.beforeSerialize = function (element, options) {
    // Allow detaching behaviors to update field values before collecting them.
    // This is only needed when field values are added to the POST data, so only
    // when there is a form such that this.form.ajaxSubmit() is used instead of
    // $.ajax(). When there is no form and $.ajax() is used, beforeSerialize()
    // isn't called, but don't rely on that: explicitly check this.form.
    if (this.form) {
      var settings = this.settings || Drupal.settings;
      Drupal.detachBehaviors(this.form, settings, 'serialize');
    }
  }
  

  /**
   * Prepare the Ajax request before it is sent.
   */
  IWapi.ajax.Action.prototype.beforeSend = function (xmlhttprequest, options) {
    // Nothing is required if no element is provided.
  };
  
  /**
   * Handler for the form redirection completion.
   */
  IWapi.ajax.Action.prototype.success = function (response, status) {
    Drupal.freezeHeight();

    for (var i in response) {
      if (response.hasOwnProperty(i) && response[i]['command'] && this.commands[response[i]['command']]) {
        this.commands[response[i]['command']](this, response[i], status);
      }
    }

    Drupal.unfreezeHeight();

    // Remove any response-specific settings so they don't get used on the next call by mistake.
    this.settings = null;
  };

  /**
   * Default Drupal error handling requires a progress element
   * and will report early termination errors. Neither should
   * be required or used for our needs.
   */
  IWapi.ajax.Action.prototype.error = function (response, uri) {
    // Unlike the standard Drupal AJAX error, we do not 
    // want to report early termination of the request.
    if (response.status) {
      alert(Drupal.ajaxError(response, uri));
    }
    
    // If no element associated to this action, then no progress elements were used.
    if (this.element) {
      // Remove the progress element.
      if (this.progress.element) {
        $(this.progress.element).remove();
      }
      if (this.progress.object) {
        this.progress.object.stopMonitoring();
      }
    
      // Undo hide.
      $(this.wrapper).show();
      // Re-enable the element.
      $(this.element).removeClass('progress-disabled').removeAttr('disabled');
    }
    
    // Reattach behaviors, if they were detached in beforeSerialize().
    if (this.form) {
      var settings = response.settings || this.settings || Drupal.settings;
      Drupal.attachBehaviors(this.form, settings);
    }
  };
  
  /**
   * Generate the AJAX request to the server, optionally
   * sending GET query parameters.
   */
  IWapi.ajax.Action.prototype.execute = function(params) {
    // Create a synonym for this to reduce code confusion.
    var ajax = this;
    var element = this.element || {};

    // Do not perform another ajax command if one is already in progress.
    if (ajax.ajaxing) {
      return false;
    }

    try {
      // If using a Drupal uri (relative URL without a starting '/').
      if (!(/^([a-z]{2,5}:\/\/|\/)/i.test(ajax.url))) {
        ajax.options.url = IWapi.utils.buildURL(ajax.url, params);
      }
      
      ajax.beforeSerialize(ajax.element, ajax.options);
      $.ajax(ajax.options);
    }
    catch (e) {
      // Unset the ajax.ajaxing flag here because it won't be unset during
      // the complete response.
      ajax.ajaxing = false;
      alert("An error occurred while attempting to process " + ajax.options.url + ": " + e.message);
    }

    // For radio/checkbox, allow the default event. On IE, this
    // means letting it actually check the box.
    if (typeof element.type != 'undefined' && (element.type == 'checkbox' || element.type == 'radio')) {
      return true;
    }
    else {
      return false;
    }
  };

  /**
   * Class of modal dialogs created mainly to contain AJAX content,
   * but an general enough for user in other contexts.
   */
  IWapi.ajax.Dialog = function(isModal) {
    this.content = $('<div class="modal-content">');
    this.progress = $('<div class="large-progress-icon login-progress">');
    
    this.container = $('<div class="modal-dialog">')
      .append(this.progress)
      .append(this.content)
      .dialog({ modal:isModal, autoOpen:false, resizable:false, draggable:false, width:450, height: 'auto' });
  };
  
  IWapi.ajax.Dialog.prototype = {
    // Properties
    _allowedAttrRegex: /^(title|width|height|dialogClass|minWidth|minHeight|maxHeight|maxWidth)$/,
    
    // Methods
    show: function(data) {
      var settings = { width: 450, height: 'auto' }; // defaults.

      if (data) {
    	if (data.content) {
          this.progress.hide();
          this.content.html(data.content).show(); // set the new content
          $('form input', this.content).first().focus();
    	}
    	
        if (data.settings && $.isPlainObject(data.settings)) {
          // make sure to prevent any settings that could make unwanted changes.
          for (var key in data.settings) {
            if (this._allowedAttrRegex.test(key)) {
              settings[key] = data.settings[key];
            }
          }
        }
      }

      // Update display settings, then show dialog
      this.container.dialog('option', settings).dialog('open');
   
      // Delay attaching behaviors until the content is displayed.
      if (data && data.content) {
        Drupal.attachBehaviors(this.container);
      }
      
      // Detect if we are in an overlay.
      var topLevel = window.location !== window.parent.location 
        ? window.parent.document : document;

      if ($('#toolbar', topLevel).length) {
        var top = $(window).scrollTop() + $('#toolbar', topLevel).outerHeight();
        $(".modal-dialog").closest('.ui-dialog').css({ top: top + 10 });
      }
      
      // Append a click handler to the overlay to close dialog.
      $(".ui-widget-overlay, .ui-dialog-overlay").click(function() {
        $(".ui-dialog .ui-dialog-content").dialog("close");
      });
    },
    showLoading: function() {
      this.content.hide();
      this.progress.show();
      
      this.container.dialog('option', 'title', 'Loading...');
      this.show();
    },
    close: function() { this.container.dialog('close'); }
  };

  
  // IWapi AJAX callback handlers.
  IWapi.ajax.commands = {
    pageReload: function(ajax, data, status) {
      location.reload();
    },
    
    urlRedirect: function(ajax, data, status) {
      if (data.delay > 0) {
        setTimeout(function () {
          window.top.location.href = data.url;
        }, data.delay);
      }
      else window.top.location.href = data.url;
    },
    
    deleteTableRow: function(ajax, data, status) {
      var element = ajax.element;
      if (ajax.original) {
        IWapi.ajax.commands.dismissModal(ajax, data, status);
        element = ajax.original;
      }

      // Reference table, and then remove row.
      var table = $(element).closest('table');
      $(element).closest('tr').remove();

      // Restripe the table.
      table.find('> tbody > tr:visible, > tr:visible')
        .removeClass('odd even')
        .filter(':even').addClass('odd').end()
        .filter(':odd').addClass('even');
    },

    updateTableRow: function(ajax, data, status) {
      var repl  = $(data.data);
      var row   = $(data.selector);
      var table = row.closest('table');
      
      if (row.hasClass('draggable')) {
    	var dragPat = /(\s|^)tabledrag-/;
    	
    	// Until a better solution can be thought of, only replace cells
    	// that don't have any hint of containing table drag items.
        $('td', repl).each(function(index) {
          var item = $('td', row).get(index);
          
          if (item && $(item).find("[class|='tabledrag']").length == 0 && !dragPat.test($(item).attr('class'))) {
        	$(item).html($(this).html());
          }
        });
      }
      else {
        row.replaceWith(repl);
      }
      
      // Restripe the table.
      table.find('> tbody > tr:visible, > tr:visible')
        .removeClass('odd even')
        .filter(':even').addClass('odd').end()
        .filter(':odd').addClass('even');
    },
    
    dialogModal: function(ajax, data, status) {
      var dlg = ajax.dialog || IWapi.ajax.sharedModal;
      dlg.show(data);

      // Bind our custom event to the form submit
      $('.modal-content form', dlg.container).once('iw-modal', function() {
        var form = $(this);
        var form_id = form.attr('id');
        
        var settings = { url: form.attr('action'), event: 'submit', progress: { type: 'throbber' }};

        Drupal.ajax[form_id] = new Drupal.ajax(form_id, this, settings);
        Drupal.ajax[form_id].form = form;
        Drupal.ajax[form_id].dialog = dlg;
        Drupal.ajax[form_id].original = ajax.element;
        Drupal.ajax[form_id].error = IWapi.ajax.Action.prototype.error;

        $('.form-actions input[type="submit"], .form-actions button[type="submit"]', form).click(function(event) {
          Drupal.ajax[form_id].element = this;
          this.form.clk = this;

          // Display our large loading icon here.
          dlg.showLoading();
          
          // An empty event means we were triggered via .click() and
          // in jquery 1.4 this won't trigger a submit.
          if (event.bubbles === undefined) {
            event.preventDefault();
            $(this.form).trigger('submit');

            return false;
          }
        });
      });
      
      $('a.cancel-action, input.cancel-action', dlg.content).click(function(event) {
        event.preventDefault();
        dlg.close();
        
        return false;
      });
    },

    dismissModal: function(ajax, data, status) {
      (ajax.dialog || IWapi.ajax.sharedModal).close();
      
      // Push status messages if there are any.
      var msgContainer = $('.messages');
      if (data.messages) {
        if (msgContainer.length > 0) {
          msgContainer.first().replaceWith(data.messages);
        }
        else {
          var content = $('#main-content');
          content.is("div") 
            ? content.prepend(data.messages)
            : content.after(data.messages);
        }
      }
      else {
        msgContainer.remove();
      }
    }
  };
  
  // Attach the AJAX callback as commands for the Drupal command processor.
  $(function() {
    Drupal.ajax.prototype.commands.page_reload     = IWapi.ajax.commands.pageReload;
    Drupal.ajax.prototype.commands.url_redirect    = IWapi.ajax.commands.urlRedirect;
    Drupal.ajax.prototype.commands.dialog_modal    = IWapi.ajax.commands.dialogModal;
    Drupal.ajax.prototype.commands.dismiss_dialog  = IWapi.ajax.commands.dismissModal;
    Drupal.ajax.prototype.commands.delete_tablerow = IWapi.ajax.commands.deleteTableRow;
  });
  
  // Create the modal dialog meant to be shared by Modal AJAX calls.
  Drupal.behaviors.iwapiModalDialog = {
    attach: function(context, settings) {
      IWapi.ajax.sharedModal = IWapi.ajax.sharedModal || new IWapi.ajax.Dialog(true);
      
      $('a.iw-use-modal', context).once('iw-modal', function() {
    	var link = $(this);
    	var href = link.attr('href');
    	
        if (href && href.length > 0) {
          link.click(function() { IWapi.ajax.sharedModal.showLoading(); });
        
          var settings = { url: href, event:'click', progress: { type: 'throbber' }};
          Drupal.ajax[href] = new Drupal.ajax(href, this, settings);
      	}
      });
    }
  }

}(jQuery));
