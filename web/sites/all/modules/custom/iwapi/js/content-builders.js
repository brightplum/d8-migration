
(function($) {

  IWapi.builders = {
    /**
     * Help format and clean-up additional information fields.
     */
    formatDraggable: function(draggable) {
      var rightEdge = 6;
      var ops = [
        { selector:".field-actions", icon: "fa-pencil", dropDown: true },
        { selector:".field-summary", icon: "fa-list-alt", dropDown: false }
      ];

      $("> input.draggable-field-op", draggable).each(function() {
        $(this).css({ top: 4, right: rightEdge });
        rightEdge += $(this).outerWidth(true) + 2;
      });
      
      for (var i in ops) {
        var op = $(ops[i].selector, draggable);
  
        if (op.length > 0) {         
          var oplink = $('<a class="draggable-field-op" href="#"><i class="fa ' + ops[i].icon +'"></i></a>')
            .css({ top: 6, right: rightEdge })
            .appendTo(draggable)
            .bind('click', op, function(event) {
              event.data.toggle();
              
              if (event.data.is(":visible")) {
                $("a.draggable-field-op.active-op").trigger('click');
                $(this).addClass("active-op");
              }
              else {
                $(this).removeClass("active-op");
              }
 
              event.preventDefault();
            });
    
          if (ops[i].dropDown) {
            oplink.addClass("dropdown");
            op.css({
              top: oplink.position().top + oplink.outerHeight() - 2,
              right: rightEdge
            });
          }

          rightEdge += oplink.outerWidth(true) + 2;
          op.hide();
        }
        
        // Save some space so icons don't overlap titles / text.
        $('.item-handle', draggable).css({ "padding-right": rightEdge });
      }
    },
    
    placeDrag: function(drag, target, uiOS) {
      var hasSpan = /(^|\s)span-\d+(\s|$)/i.test(drag.attr('class'));
      
      drag.detach();
      for (var i = 0; i < target.children.length; ++i) {
        var item = $(target.children.get(i));
        var offset = item.offset();
          
        if (offset.top > uiOS.top || (hasSpan && ((offset.top + item.outerHeight()) > (uiOS.top + drag.height()/2) && uiOS.left < offset.left + 20))) {
          item.before(drag);
          return;
        }
      }
      target.container.append(drag);
    }
  };

  IWapi.builders.ContentBuilder = function(element, containers, fields) {
    self = this;

    this.element    = element;
    this.containers = {};
    this.events     = [];

    // Attach only events that resolve to functions.
    var handlers = [ 'drop', 'dropout', 'dropover' ];
    for (var i = 0; i < handlers.length; ++i) {
      if ($.isFunction(this[handlers[i]])) this.events.push(handlers[i]);
    }
    
    var canvasCont = $("> .content-container", element);
    this.containers["<root>"] = { key: "<root>", element: canvasCont };
    
    this.buildToolbar(); // see if there is toolbar content, and construct it if there is.
    
    // find all content containers / placeholder locations.
    containers.children('.content-container').each(function() {
      var name = $(this).siblings("input[type=hidden].container-value").val();
      self.containers[name] = { key: name, element: $(this) };
    });
    
    // Put all the draggable elements into their places.
    containers.each(function() { self.insertItem($(this)); });
    fields.each(function() { self.insertItem($(this)); });
    
    for (var i in this.containers)
      self.insertContainer(this.containers[i].element);
  }


  // Add the instance methods.
  IWapi.builders.ContentBuilder.prototype = {
  
    /**
     * Find elements that belong in the toolbar, and if they
     * exist, allocate the space and setup the tool items.
     */
    buildToolbar: function() {
      var element = this.element;
      var unused = $("#used-fields, #edit-unused-fields");
      var tools  = $("#builder-tools, #edit-tools");
      
      if (unused.length > 0 || tools.length > 0) {
        var toolbar = $('<div id="builder-toolbar" class="iwapi-dockable dock-top-right-sticky">');
        
        element.wrap('<div id="builder-wrapper" class="iwapi-dockto clearfix">')
          .css({ float:"left" })
          .after(toolbar);
        
        if (tools.length > 0) toolbar.append(tools);
        
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
      var weight = $('.field-weight', item), parent = $('.field-parent', item);
      
      var container = parent.val();
      var weightVal = parseInt(weight.val());
      
      if (container === undefined || container === null || container.length < 1) {
        container = "<unused>";
      }
    
      // Hide things that users don't need to see.
      weight.closest('.form-item').hide();
      parent.closest('.form-item').hide();
      
      item.draggable({
        handle: ".item-handle",
        cancel: "a, select", // clicking an icon won't initiate dragging
        revert: "invalid", // when not dropped, the item will revert back to its initial position
        helper: "clone",
        cursor: "move",
        zIndex: 250,
        start: function(event, ui) {
          item.placeholder = $("<div>").attr('class', item.attr('class'))
            .addClass('placeholder')
            .css({ background: "#ffff99", height: 40 });
 
          item.hide().after(item.placeholder);
        },
        drag: function(event, ui) {
          if (self.draggingTarget && item.placeholder)
            IWapi.builders.placeDrag(item.placeholder, self.draggingTarget, ui.offset);
        },
        stop: function(event, ui) {
          item.placeholder.remove();
          item.show();
        }
      });
      
      IWapi.builders.formatDraggable(item);

      // Find the correct position of the element
      if (this.containers[container]) {
        var container = this.containers[container].element;
        var siblings  = container.children('.draggable-field, .draggable-container');

        // Insertion sort based on weight values.
        for (var i = 0; i < siblings.length; ++i) {
          if (parseInt($('.field-weight', siblings.get(i)).val()) > weightVal) {
            $(siblings.get(i)).before(item);
            return;
          }
        }

        container.append(item); // Only reached if no greater weight found.
      }
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
      var element = event.data.element;
      var selector = "> .draggable-field:not(.placeholder), > .draggable-container";
      var target = { container: element, children: $(selector, element) };
        
      IWapi.builders.placeDrag(ui.draggable, target, ui.offset);
      $('> .form-item .field-parent', ui.draggable).val(event.data.key);
      
      var weight = 1;
      $(selector, element).each(function() { $("> .form-item .field-weight", this).val(weight++); });
   
      delete this.draggingTarget;
      event.stopPropagation();
    },
    
    dropover: function(event, ui) {
      delete this.draggingTarget;

      this.draggingTarget = {
        container: event.data.element,
        children: $('> .draggable-field, > .draggable-container', event.data.element)
      };
      
      event.stopPropagation();
    },
    
    dropout: function(event, ui) {
      delete this.draggingTarget;
      event.stopPropagation();
    }
  };


  /**
   * Initialize content builder handlers and UI events.
   */
  $(function() {
    var fields     = $("#field-data > .draggable-field");
    var containers = $("#container-data > .draggable-container");
    
    var canvas = $("#content-canvas");
    $("#node-page-grid").change(function() {
      var cssName = $("> .content-container", canvas).attr('class');
      cssName = cssName.replace(/((?:^| )grid-size-)\d+( |$)/i, "$1" + $(this).val() + "$2");
      
      $("> .content-container", canvas).attr('class', cssName);
    });
 
    var builder = new IWapi.builders.ContentBuilder(canvas, containers, fields);
    $(window).trigger('scroll');
    
    // Add the AJAX handlers for interactions.
    Drupal.ajax.prototype.commands.addContentWrapper = function(ajax, data, status) {
      var html = $(data.html).appendTo('#field-data');
      var weight = $("#content-canvas > .content-container").children().length;
      
      $(".field-weight", html).val(weight); // add the correct field weight information.
      $(".field-parent", html).val("<root>");
      builder.insertItem(html);
      
      if (html.hasClass('draggable-container')) {
        builder.insertContainer($('.content-container', html), true);
      }
      
      var scroll = (document.documentElement.scrollTop || document.body.scrollTop);
      var offset = html.offset();

      if (offset.top > scroll + $(window).height() || offset.top < scroll)
        $(document).scrollTop(offset.top);
    };
    
    Drupal.ajax.prototype.commands.updateDraggableItem = function(ajax, data, status) {
      $("a.draggable-field-op.active-op").trigger('click');

      var draggable = $(data.selector);
      draggable = draggable.hasClass("ui-draggable") ? draggable : $(".ui-draggable", draggable);

      if (draggable.length > 0) {
        $(".item-handle .item-label", draggable).html(data.label);
        
        var css  = draggable.attr("class").replace(/((?:^| )span-)\d+( |$)/i, "");
        if (data.span && data.span > 0) css += " span-" + data.span;

        // Update the draggable item summary.
        draggable.attr("class", css);
        $(".field-summary", draggable).empty().append($(data.summary).contents());
      }
    };
  });

}(jQuery));
