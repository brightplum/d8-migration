
(function($) {

  CDream = {
   jVer: jQuery.fn.jquery.split('.').map(function(i){return('0'+i).slice(-2)}).join('.'),
   gridLayout: {}
 };

  CDream.gridLayout.resize = function(event) {
    var adjBound = CDream.jVer < '01.08.00';

    $('.l-content .grid-layout, .region-content .grid-layout').each(function() {
      var content  = $('.view-content', this);
      content = content.length ? content : $(this);
      
      var maxWidth = content.width(); 

      var w     = 0;
      var rowH  = 0;
      var group = [];
      var tile;
      
      content.children('.grid-tile').each(function() {
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
        if (group.length == 0) $(this).addClass('alpha');

        group.push($(this));
        w += mW;
      });
      
      // Flush out the final row.
      if (group.length > 0) {
        group[group.length - 1].addClass('omega');
        while (tile = group.pop()) IWapi.utils.setHeight(tile, rowH);
      }
    });
  }

  $(window).load(function() {
    CDream.gridLayout.resize();
    $(window).resize(CDream.gridLayout.resize);
  });

}(jQuery));