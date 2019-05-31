
(function($) {

  // State information of the comments
  IWapi.entityNote = {
    prevOp: null,
    commands: { }
  }
  
  IWapi.entityNote.processContent = function(data, ajax) {
    // Remove the old data, we no longer need it.
    var prevOp = IWapi.entityNote.prevOp || {};
    
    if (prevOp.command !== data.command && prevOp.original) {
      prevOp.original.show();
    }
    if (prevOp.content) {
      prevOp.content.remove();
    }
    IWapi.entityNote.prevOp = null;
	    
    // Attach new content to the document
    var content = $('<div></div>').html(data.html);
    var form = $('form', content);

    content = content.contents(); // unwrap the contents.

    // Is this a form or a new comment?
    if (form.length > 0) {
      // Process new form before adding it.
      form.each(function() {
        var form_id = $(this).attr('id');

        var settings = { url: form.attr('action'), event: 'submit', progress: { type: 'throbber' }};
		    
        Drupal.ajax[form_id] = new Drupal.ajax(form_id, this, settings);
        Drupal.ajax[form_id].form = $(this);
        Drupal.ajax[form_id].original = ajax.element;
		
        $('input[type=submit], button', this).click(function(event) {
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

    return { content: content, forms: form };
  }
  
  IWapi.entityNote.commands.replyForm = function(ajax, resp, status) {
    var data = IWapi.entityNote.processContent(resp, ajax);
    
    var replyContainer = $(resp.selector).next();
    if (!replyContainer.hasClass("indented")) {
      replyContainer = $('<div class="indented"></div>');
      $(resp.selector).after(replyContainer);
    }
    
    // Is this a form or a new comment?
    if (data.forms.length > 0) {
      replyContainer.prepend(data.content);
      
      IWapi.entityNote.prevOp = {
        command: resp.command,
        content: data.content
      };
    }
    else if (data.content.length) {
      replyContainer.append(data.content);
    }

    Drupal.attachBehaviors(data.content);
  }
  
  IWapi.entityNote.commands.editForm = function(ajax, resp, status) {
    var prevOp = IWapi.entityNote.prevOp || {};
	
    if (!(prevOp.command === resp.command || prevOp.original)) {
      prevOp.command = resp.command;
      prevOp.original = $(resp.selector);
      prevOp.original.hide();
    }

    var data = IWapi.entityNote.processContent(resp, ajax);
    prevOp.original.after(data.content);
    prevOp.content = data.content;

    if (data.forms.length > 0) {
      IWapi.entityNote.prevOp = prevOp;
    }
    else {
      // We've replaced this content, we can drop the original.
      prevOp.original.remove();
    }

    Drupal.attachBehaviors(data.content);
  }
  
  IWapi.entityNote.commands.addNew = function(ajax, resp, status) {
    var wrapper = ajax.form.closest('.comments-wrapper');
    var newForm = $(resp.form).clearForm();
    ajax.form.replaceWith(newForm);

    if (resp.comment) {
      var content = $(resp.comment);
 
      wrapper.append(content);
      var scrollTo = content.offset().top - $(window).height()/2 + content.outerHeight();
      $(window).scrollTop(scrollTo);
    }

    Drupal.attachBehaviors(wrapper);
  }

  // jQuery document.ready();
  $(function() {
    Drupal.ajax.prototype.commands.entity_note_reply = IWapi.entityNote.commands.replyForm;
    Drupal.ajax.prototype.commands.entity_note_edit  = IWapi.entityNote.commands.editForm;
    Drupal.ajax.prototype.commands.entity_note_add   = IWapi.entityNote.commands.addNew;
  });


  /**
   * Drupal behavior to AJAX enable the comment form.
   * Entity notes need to be treated special.
   */
  Drupal.behaviors.entityNoteAdd = {
    attach: function(context) {
      $('form.add-entity-note', context).once('entity-note', function() {
        var form_id = 'entity-note-add-new';
        $(this).attr(form_id);

        var entityType = $('input[name=entity_type]', this).val();
        var entityId   = $('input[name=entity_id]', this).val();
        
        var settings = {
          url: IWapi.utils.buildURL("entity-note/add/ajax/" + entityType + '/' + entityId),
          event: 'submit',
          progress: { type: null }
        };

        $(this).attr('action', settings.url);
        Drupal.ajax[form_id] = new Drupal.ajax(form_id, this, settings);
        Drupal.ajax[form_id].form = $(this);
        
        $('input[type=submit], button', this).click(function(event) {
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

}(jQuery));
