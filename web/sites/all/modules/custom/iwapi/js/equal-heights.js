
(function($) {

  Drupal.behaviors.iwEqualHeightGroups = {
    attach: function(context, settings) {
      var groups = [];
      
      var equalHeights = function(parent, item) {
        // We need to establish groups of equal heights items.
        if (parent.data('heights-group')) {
          parent.data('heights-group').push(item);
          var max = parent.data('max-height');
          if (max < item.outerHeight(true)) {
            parent.data('max-height', item.outerHeight(true));
          }
        }
        else {
          parent.data('max-height', item.outerHeight(true));
          parent.data('heights-group', [item]);
          groups.push(parent);
        }
      }
 
      $('.content-column.equal-heights', context).each(function() {
        var cntr  = $(this).closest(".content, .content-container, .region");
        var child = $("> .inner-column", this);

        if (cntr.length > 0 && child.length > 0) equalHeights(cntr, child);
      });
      
      $('.panel-display .equal-heights', context).each(function() {
        var cntr  = $(this).closest(".panel-panel").parent();
        var child = $(this).closest(".inside");

        if (cntr.length > 0 && child.length > 0) equalHeights(cntr, child);
      });
      
      // Go through each group and re-assign the heights
      for (var i in groups) {
        var max  = groups[i].data('max-height');
        var data = groups[i].data('heights-group');
      
        for (var j in data) {
          var adjust = 0;
          
          switch (data[j].css('boxSizing')) {
            case 'content-box':
              adjust = data[j].outerHeight(true) - data[j].height();
              break;
            case 'border-box':
            default:
              adjust = data[j].outerHeight(true) - data[j].outerHeight();
          }

          data[j].css({'minHeight': max - adjust});
        }
      }
    }
  }

}(jQuery));
