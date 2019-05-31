(function($) {

Drupal.behaviors.image_caption = {
  attach: function (context, settings) {
    $(settings.image_caption.selector).once('caption', function(i) {      
      // Get caption from title attribute
      var captiontext = $(this).attr('title');
      if (!captiontext || captiontext.length === 0) {
        return; // skip if no caption text.
      }
      
      var imgwidth = $(this).width() ? $(this).width() : false;
      
      // Get image alignment and style to apply to container
      if($(this).attr('align')){
        var alignment = $(this).attr('align');
        $(this).css({'float':alignment}); // add to css float
        $(this).removeAttr('align');
      }else if($(this).css('float')){
        var alignment = $(this).css('float');
        $(this).removeClass("image-" + alignment);
      }else{
        var alignment = 'normal';
      }

      $(this).removeAttr('align');
      $(this).removeAttr('style');
      
      //Display inline block so it doesn't break any text aligns on the parent contatiner
      $(this).wrap("<span class=\"image-caption-container\"></span>");
      $(this).parent().addClass('image-caption-container-' + alignment);
      
      if (/(^| )(image-full|width-full)( |$)/i.exec($(this).attr("class"))) {
        $(this).parent().addClass('img-width-full');
      }
      else if(imgwidth) {
        $(this).width(imgwidth);
        $(this).parent().width(imgwidth);
      }

      // Append caption
      $(this).parent().append("<span style=\"display:block;\" class=\"image-caption\">" + captiontext + "</span>");
    });
  }
};

})(jQuery);
