(function($) {

  Drupal.behaviors.equalHeightColumns = {
    attach: function(context, settings) {
      var groups = [];
 
      $('.field-group-format.equal-height-column', context).each(function() {
        var parent = $(this).parent().closest("div.content, div.node__content, .field-group-format");
        var item   = $(this).find("> .column-inner");

        // We need to establish groups of equal heights items.
        if (parent.data('heights-group')) {
          parent.data('heights-group').push(item);
          var max = parent.data('max-height');
          if (max < item.height()) {
            parent.data('max-height', item.height());
          }
        }
        else {
          parent.data('max-height', item.height());
          parent.data('heights-group', [item]);
          groups.push(parent);
        }
      });
      
      // Go through each group and re-assign the heights
      for (var i = 0; i < groups.length; ++i) {
        var max  = groups[i].data('max-height');
        var data = groups[i].data('heights-group');
        
        for (var j = 0; j < data.length; ++j) {
          data[j].css({'min-height':max});
        }
      }
    }
  }

}(jQuery));
