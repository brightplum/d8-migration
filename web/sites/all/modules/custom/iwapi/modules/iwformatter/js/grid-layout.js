
(function($) {

  IWapi.layouts = {};
  
  IWapi.layouts.Grid = function(container) {
    this.wrapper = container;
    this.tiles   = container.find('.grid-tile');
    
    // Wait until images are fully loaded and everything is sized correctly.
    var self = this;
    $(window).bind('load', function() {
      self.resize();
      $(window).resize($.proxy(self, 'resize'));
    });
  };
  
  IWapi.layouts.Grid.prototype = {
    resize: function() {
      var maxWidth = this.wrapper.width(); 

      var w     = 0;
      var rowH  = 0;
      var group = [];
      var tile;
      
      this.tiles.each(function() {
        $(this).height('auto').removeClass('omega').removeClass('alpha'); // Any residue settings.

        var mW = $(this).outerWidth(true) - 1;     // Margin / full space
        var cW = mW - IWapi.utils.parseCssSize($(this).css('marginRight'), this);

        // when the width overflows we are dealing with a new row.
        if (w + cW > maxWidth) {
          group[group.length - 1].addClass('omega');

          while (tile = group.pop()) IWapi.utils.setHeight(tile, rowH);
        
          w = 0;
          rowH = $(this).outerHeight();
        }
        else if (rowH < $(this).outerHeight()) {
          rowH = $(this).outerHeight();
        }
        
        // if first element in a row, add "alpha" CSS class
        if (group.length === 0) {
          $(this).addClass('alpha');
          mW = $(this).outerWidth(true) - 1; // adjusted with Alpha class.
        }
          
        group.push($(this));
        w += mW;
      });
      
      // Flush out the final row.
      if (group.length > 0) {
        group[group.length - 1].addClass('omega');
        while (tile = group.pop()) IWapi.utils.setHeight(tile, rowH);
      }
    }
  };

}(jQuery));