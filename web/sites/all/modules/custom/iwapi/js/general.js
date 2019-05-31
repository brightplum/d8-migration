(function ($) {

  Drupal.behaviors.loggedInContentSwitch = {
    attach: function (context, settings) {
      if ($('body').hasClass('logged-in')) {
        $('.logged-in-content', context).show();
        $('.anonymous-content', context).hide();
      }
      else {
        $('.logged-in-content', context).hide();
        $('.anonymous-content', context).show();
      }
    }
  }

  Drupal.behaviors.jsHiddenElements = {
    attach: function (context, settings) {
      $('.js-hidden', context).hide();
    }
  }
  
  Drupal.behaviors.iwFormStyling = {
    attach: function(context, settings) {        
      var callback = function() {
        var value = $(this).val();

        var func = (!value || value == '0' || value.length === 0)
          ? 'addClass' : 'removeClass';

        $(this)[func]('empty-value');
      }
   
      // Setup each select element with style changing.
      $('select.has-empty', context).each(function() {
        var color = $(this).css("color");
        
        $(this).find('option').css({ color: color });
        $(this).change(callback);
        
        callback.call(this); // call with this item.
      });
    }
  }

}(jQuery));
