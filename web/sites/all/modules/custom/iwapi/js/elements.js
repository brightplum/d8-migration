
(function($) {

  Drupal.behaviors.selectOrTextElement = {
    attach: function(context, settings) {
      $('form .form-type-select-or-text').once('select-or-text', function() {
        var selector   = $(this).find('select');
        var tandemText = $(this).find("input[type='text']");

        tandemText[(selector.val() && selector.val().length > 0) ? 'hide' : 'show']();
        
        selector.change(function() {
          tandemText[$(this).val() && $(this).val().length > 0 ? 'hide' : 'show']();
        })
    
      });
    }
  };
  
  // Allow popups with help text
  Drupal.behaviors.iwapiPopupHelp = {
    attach: function(context, settings) {
      $(".iw-popup-help", context).once(function () {
        var content = $(".help-content", $(this)).dialog({
          title: "Help",
          modal: true,
          autoOpen: false
        });
        
        var width = $('#page').width();
        
        if (width && width > 200) {
          content.dialog("option", "minWidth", width >> 1);
        }
    	
        $("a.help-tooltip-label", $(this)).click(function() {
          content.dialog('open');

          // Append a click handler to the overlay to close dialog.
          $(".ui-widget-overlay, .ui-dialog-overlay").click(function() {
            $(".ui-dialog .ui-dialog-content").dialog("close");
          });
          return false;
        });
      });
    }
  };

} (jQuery));