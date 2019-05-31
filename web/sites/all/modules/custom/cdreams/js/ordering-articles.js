
(function($) {

  IWapi.builders = {
    /**
     * Help format and clean-up additional information fields.
     */
    formatDraggable: function(draggable) {
      var rightEdge = 6;
      var actions = $('.field-actions', draggable);
      
      if (actions.length > 0) {
        $('li a', actions).each(function() {
          $(this).addClass('draggable-field-op')
            .css({ top: 6, right: rightEdge })
            .appendTo(draggable);
  
          rightEdge += $(this).outerWidth(true) + 2;
        });

        actions.remove();
        $('.item-handle', draggable).css({ "padding-right": rightEdge });
      }
    },
    
    entitySaved: function(ajax, data, status) {
      var update = $("#news-item-" + data.entityId);
      
      alert(update.length());
      var img = (data.thumbnail && data.thumbnail.length) ? data.thumbnail : '';
      
      // Update the title and article image.
      $(".item-handle", update).html('<span class="drag-icon"></span>' + data.label);
      $(".field-summary img", update).attr('src', img);
    }
  };

  IWapi.builders.ContentBuilder = function(element, containers, fields) {
    self = this;

    this.element    = element;
    this.containers = {};
    this.events     = [];
    this.maxSlot    = 0;

    // Attach only events that resolve to functions.
    var handlers = [ 'drop', 'dropout', 'dropover' ];
    for (var i = 0; i < handlers.length; ++i) {
      if ($.isFunction(this[handlers[i]])) this.events.push(handlers[i]);
    }

    this.buildToolbar(); // see if there is toolbar content, and construct it if there is.

    // find all content containers / placeholder locations.
    containers.each(function() {
      var name = $("> input[type=hidden].container-value", this).val();
      self.containers[name] = { key: name, element: $('> .content-container', this) };
      self.element.append($(this));

      if (self.maxSlot < parseInt(name)) self.maxSlot = parseInt(name);
    });

    // Put all the draggable elements into their places.
    fields.each(function() { self.insertItem($(this)); });

    for (var i in this.containers) {
      self.insertContainer(this.containers[i].element);
    }
    
    // Make sure users can't just drop items here.
    this.containers['<unused>'].element.droppable("option", "disabled", true);
    this.markBoundsAndCollapse();
  }

  // Add the instance methods.
  IWapi.builders.ContentBuilder.prototype = {

    /**
     * Find elements that belong in the toolbar, and if they
     * exist, allocate the space and setup the tool items.
     */
    buildToolbar: function() {
      var element = this.element;
      var unused = $("#unused-fields, #edit-unused-fields");
      
      if (unused.length > 0) {
        var toolbar = $('<div id="builder-toolbar" class="iwapi-dockable dock-top-right-sticky">');
        
        element.wrap('<div id="builder-wrapper" class="iwapi-dockto clearfix">')
          .css({ float:"left" })
          .after(toolbar);
        
        if (unused.length > 0) {
          toolbar.append(unused);
          this.containers["<unused>"] = { key: "", element: $(".content-container", unused) };
        }
        
        var tOffset = toolbar.outerWidth(true) + element.outerWidth(true) - element.width() + 6;
        var resizeCallback = function() {
          element.width(Math.max(375, $("#builder-wrapper").width() - tOffset));
        };
        
        // With a constant sized tool bar, adjust the canvas width.
        $(window).resize(resizeCallback);
        resizeCallback();
        
        Drupal.attachBehaviors(toolbar.parent());
      }
    },

    insertItem: function(item) {
      var self = this;
      var parent = $('.field-parent', item);
      var container = parent.val();
      
      if (container === undefined || container === null || container.length < 1) {
        container = "<unused>";
      }
    
      // Hide things that users don't need to see.
      parent.closest('.form-item').hide();
      
      item.draggable({
        handle: ".item-handle",
        cancel: "a, select", // clicking an icon won't initiate dragging
        revert: "invalid", // when not dropped, the item will revert back to its initial position
        helper: "clone",
        cursor: "move",
        zIndex: 750,
        revert: function(droppable) {
          if (droppable === false) {
            this.detach();

            self.pushItems($('.field-parent', this).val(), $(this));
            self.markBoundsAndCollapse();
          }
        },
        
        start: function(event, ui) {
          var container = ui.helper.closest('.content-container');
          var slot = container.siblings('.container-value').val();

          $(this).hide();
          if (slot.length > 0 && parseInt(slot) >= 0) {
            self.pullItems(parseInt(slot));
          }
        },

        stop: function(event, ui) { $(this).show().removeAttr('style'); }
      });

      IWapi.builders.formatDraggable(item);
      if (this.containers[container]) this.containers[container].element.append(item);
    },
    
    insertContainer: function(container, addOption) {
      var name = container.siblings("input[type=hidden].container-value").val();
      if (!this.containers[name]) {
        this.containers[name] = { key: name, element: container };
      }
      
      if (addOption) {
        var label = container.siblings(".item-handle").find(".item-label").text();
        $(".draggable-container, .draggable-field", this.element)
          .find("select.field-parent").each(function() {
            if ($('option[value="' + name + '"]', this).length === 0) $(this).append('<option value="' + name + '">' + label + '</option>');
          });
      }
      
      container.droppable({
        greedy: true,
        accept: ".draggable-field, .draggable-container",
        tolerance: "pointer"
      });
      
      // Attach events relating to droppable, do this last so
      // events don't trigger until everything is complete.
      for (var k in this.events) {
        var eName = this.events[k];
        container.bind(eName, this.containers[name], $.proxy(this, eName));
      }
    },
      
    /* droppable events */
    drop: function(event, ui) {
      ui.draggable.detach();

      this.pushItems(parseInt(event.data.key), ui.draggable);
      this.markBoundsAndCollapse();
      
      // Provide slight delay for things to settle before resizing.
      setTimeout(CDream.gridLayout.resize, 100);
    },
    
    pullItems: function(slot) {
      var fill = slot;
 
      while (slot < this.maxSlot) {
        var key = (++slot).toString();
        if (this.containers[key]) {
          var temp = this.containers[key].element;
          var kids = temp.children();

          if (kids.length > 0) {
            var fillStr = (fill++).toString();
            $('> .form-item .field-parent', kids).val(fillStr);
            this.containers[fillStr].element.prepend(kids);
          }
        }
      }
        
      this.markBoundsAndCollapse();
    },
 
    pushItems: function(slot, item) {
      var date = $('.date-value', item).val();

      // Bubble up if we're amoung older date articles.
      while (slot > 0) {
        var key = (slot - 1).toString();
        
        var cont = this.containers[key].element;
        if ($(".date-value", cont.children()).val() >= date) {
          break;
        }
        else {
          --slot;
        }
      }

      
      // The remaining items work with the date comparisons.
      while (slot <= this.maxSlot && item.length) {
        var key = slot.toString();
        var cont = this.containers[key].element;
        var elem = cont.children();

        if (!elem.length || $(".date-value", elem).val() <= date) {
          $('> .form-item .field-parent', item).val(key);
          cont.append(item);
          
          if (!elem.length) return;
          else item = elem.detach();
        }
        
        ++slot;
      }

      if (item) {
        $('> .form-item .field-parent', item).val('');
        this.containers["<unused>"].element.append(item);
      }
    },
    
    markBoundsAndCollapse: function() {
      var slot = 0, fill = -1; // starting at slot #4 we start to tag datelines.
      var date = '', day = 0;
      
      $('#content-canvas .section-header').remove();
      $('#content-canvas .content-slot').removeClass('has-header');

      while (slot <= this.maxSlot) {
        var key = slot.toString();

        if (this.containers[key]) {
          var curr = this.containers[key].element;
          curr.attr('class', curr.attr('class').replace(/(^|\s)day-\d+(\s|$)/i, ' '));

          if (curr.children().length) {
            if (fill > 0) {
              var fillStr = (fill++).toString();
              $('> .form-item .field-parent', curr.children()).val(fillStr);
              this.containers[fillStr].element.append(curr.children());
              curr = this.containers[fillStr].element;
            }
          
            if ($('.date-value', curr.children()).val() !== date) {
              date = $('.date-value', curr.children()).val();
              curr.before($('<div class="section-header">' + date +'</div>'));
              curr.closest('.content-slot').addClass('has-header');
              ++day;
            }

            curr.addClass('day-' + day); // Add a day color
          }
          else if (fill < 0) fill = slot;
        }
        ++slot;
      }
      
      if (fill >= 0) {
        while (fill <= this.maxSlot && $('#edit-unused-fields .content-container > .draggable-field').length) {
          var fillStr = (fill++).toString();
          var extra = $('#edit-unused-fields .content-container > .draggable-field').first();
        
          $('> .form-item .field-parent', extra).val(fillStr);
          this.containers[fillStr].element.append(extra);
        }
      }
    }
  };


  /**
   * Initialize content builder handlers and UI events.
   */
  $(function() {
    Drupal.ajax.prototype.commands.entity_saved = IWapi.builders.entitySaved;
    
    var canvas = $("#content-canvas");
    var fields = $("#field-data > .draggable-field");
    var containers = $("#container-data > .content-slot");

    new IWapi.builders.ContentBuilder(canvas, containers, fields);
  });


  // Make sure to trigger window dimension changes.
  $(window).load(function() {
    $(window).trigger('resize').trigger('scroll');
  });

}(jQuery));
