
// -------------------------
// Docking and containers
// -------------------------

(function($) {

  // ==================================
  // Global variables and functionality
  // ==================================
  IWapi.docking = {
    // detect the edges and behavior type desired for the docked object.
    // Regexp matches [ 1st = edge, 2nd = alignment, 3rd = display style ] 
    edgeExpr: /(?:\s|^)dock-(top|bottom|left|right)(?:-(top|bottom|left|right|center))?(?:-(attach|sticky|inside|outside))?(?:\s|$)/i,

    // Contain the docked item (check for collisions).
    containedExpr: /(?:\s|^)contained-(horizontal|vertical)(?:\s|$)/i,

    behavior: {
      attach: function(context, settings) {
            
        IWapi.docking.resizeWindow(null); // Content possibly added.

        var wrappers = ".iwapi-dockto, form, .content-container, #page";
        $('.iwapi-dockable', context).once('iwapi-dock', function() {
          new IWapi.docking.Docker($(this), $(this).closest(wrappers));
        });
      }
    },

    // Global tracker of window dimensions.
    WindowRect: { top: 0, left: 0, bottom: 0, right:0 },
  
    /**
     * Event catch resizing of the window dimensions
     */
    resizeWindow: function() {
      var rect = IWapi.docking.WindowRect;
      var topLevel = document;

      // Detect proper reference document (this happens in an overlay).
      if (window.location !== window.parent.location) {
        var topLevel = window.parent.document;
      }

      // Adjust offsets with toolbar information.
      if ($('#toolbar', topLevel).length) {
        rect.top = $('#toolbar', topLevel).outerHeight();
      }

      rect.right  = $(window).width();
      rect.bottom = $(window).height();
    },
    
    /**
     * Resize a docking object to the container width
     */
    resizeToContainerWidth: function() {
      this.dockCSS.width = this.cont.outerWidth() + this.item.width() - this.item.outerWidth(true);
      this.item.width(this.dockCSS.width);
    },
    
    edgeDetectHorizontal: function() {
      var cRect = new IWapi.docking.BoundingRect(this.cont);
      var iRect = new IWapi.docking.BoundingRect(this.item);
      
      // TODO: Detect edge collisions and adjust positions if found.
    },
    
    edgeDetectVertical: function() {
      var cRect = new IWapi.docking.BoundingRect(this.cont);
      var iRect = new IWapi.docking.BoundingRect(this.item);
      
      // TODO: Detect edge collisions and adjust positions if found.
      if (iRect.left <= cRect.left) { // prioritize left edge
        
      }
      else if (iRect.right > cRect.right) {
        
      }
    },
    
    BoundingRect: function(element) {
      var offset = element.offset();
  
      this.top  = offset.top;
      this.left = offset.left;
      this.bottom = offset.top + element.outerHeight();
      this.right = offset.left + element.outerWidth();
    }
  }
  
  IWapi.docking.BoundingRect.prototype = {
    shiftX: function(x) { this.left += x; this.right  += x; },
    shiftY: function(y) { this.top  += y; this.bottom += y; },
 
    width: function()  { return this.right - this.left; },
    height: function() { return this.bottom - this.top; }
  }
  
  // Our custom docking setup and attach code.
  IWapi.docking.Docker = function(element, container) {
    this.item = element;
    this.cont = (container && container.length) ? container : element.parent();
    
    var matches = IWapi.docking.edgeExpr.exec(this.item.attr("class"));
    if (matches && IWapi.docking.edges[matches[1]]) {
      var edge = IWapi.docking.edges[matches[1]];
        
      this.dockCSS = { zIndex: 500 };
      this.dockCSS[matches[1]] = 0;
        
      // Edges have an initializer for each docking style.
      edge[matches[3]](this, matches[2]);
      this.item.css(this.dockCSS);
    }
    
    var func = ['resize', 'scroll'];
    for (var i in func) {
      if (this[func[i]]) {
        this[func[i]]();
        $(window).bind(func[i] + ".iwapiDocked", $.proxy(this, func[i]));
      }
    }
  };

  
  // ============================
  // Docking edge handlers
  // ============================
  IWapi.docking.edges = {
    top: {
      attach: function(dock, align) {
        dock.dockCSS.top = IWapi.docking.WindowRect.top;

        dock.original = dock.item;
        dock.item = dock.item.clone()
          .removeAttr('id')
          .addClass('fixed-clone')
          .appendTo(dock.cont);

        dock.inView = this.inView;
        dock.resize = IWapi.docking.resizeToContainerWidth;
        dock.scroll = function() { dock.item[dock.inView() ? 'show':'hide'](); }
      },

      sticky: function(dock, align) {
        dock.dockCSS.top = IWapi.docking.WindowRect.top;

        // So the container doesn't jump, set the min height of the container.
        dock.cont.css({ minHeight: dock.item.height() });

        dock.inView = this.inView;
        if (align && align.length > 0) {
          dock.resize = function() {
            var css = {};
            var wnd = IWapi.docking.WindowRect;
            var bound = new IWapi.docking.BoundingRect(dock.cont);

            css[align] = dock.dockCSS[align] = wnd[align] - bound[align];
            dock.item.css(css);
          }
        }
        dock.scroll = function() { dock.item.css(dock.inView() ? { position:'fixed'}:{position:'static'}) };
      },

      inView: function() {
        var r = new IWapi.docking.BoundingRect(this.cont);
        var w = IWapi.docking.WindowRect;
        var oOffset = this.original ? this.original.offset() : this.cont.offset();

        var scroll = (document.documentElement.scrollTop || document.body.scrollTop);
        var wndBot = scroll + w.bottom;

        return w.top < r.bottom && wndBot > r.top && scroll > (oOffset.top - w.top);
      }
    },

    bottom: {
      attach: function(docker, align) {       
        docker.original = docker.item;
        docker.item = docker.item.clone()
          .removeAttr('id')
          .addClass('fixed-clone')
          .appendTo(docker.cont);

        docker.inView = this.inView;
        docker.resize = IWapi.docking.resizeToContainerWidth;
        docker.scroll = function() { docker.item[docker.inView() ? 'show':'hide'](); }
      },

      sticky: function(dock, align) {        
        dock.inView = this.inView;

        if (align && align.length > 0) {
          dock.resize = function() {
            var css = {};
            var wnd = IWapi.docking.WindowRect;
            var bound = new IWapi.docking.BoundingRect(dock.cont);

            css[align] = dock.dockCSS[align] = wnd[align] - bound[align];
            dock.item.css(css);
          }
        }
        dock.scroll = function() { dock.item.css(dock.inView() ? { position:'fixed'}:{position:'static'}) };
      },

      inView: function() {
        var r = new IWapi.docking.BoundingRect(this.cont);
        var h = this.item.outerHeight(false);
        var w = IWapi.docking.WindowRect;

        var oOffset = this.original ? this.original.offset() : this.cont.offset();

        var scroll = (document.documentElement.scrollTop || document.body.scrollTop);
        var wndBot = scroll + w.bottom;

        return (w.top < r.bottom && wndBot > r.top && (wndBot - h) < oOffset.top);
      }
    },

    left: {
      init: function() { },
      scroll: function() { }
    }
  };


  /**
   * Initialize the window sizing, and add the docking behaviors.
   */
  $(function() {
    $(window).resize(IWapi.docking.resizeWindow);

    // Add the behavior after other behaviors are run so we can
    // control when this behavior is called. Needs to happen
    // after other things initialized.
    Drupal.behaviors.iwDockingElements = IWapi.docking.behavior;
    IWapi.docking.behavior.attach(document, Drupal.settings);
  });

} (jQuery));
