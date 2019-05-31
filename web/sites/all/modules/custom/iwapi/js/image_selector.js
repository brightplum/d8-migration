
(function($) {

  Drupal.behaviors.iwapiImageSelector = {
    attach: function(context, settings) {
      var config = settings.iwapi.elements;
 
      $('.form-type-image-selector', context).once('selector-once', function() {
    	var element = $(this);
    	var ID = $(this).find('input.image-selector-value').attr('id');

    	if (ID && ID.length > 0) {
          new IWapi.elements.ImageSelector(element, config.imageSelectors[ID]);
          delete config.imageSelectors[ID];
    	}

        // Handle first the upload progress button. The progress upload handlers are a bit odd in,
        // that they look for a static form input ID to retrieve the upload ID. This only allows
        // for a single upload at a time unless we do a name switch-a-roo.
    	var progressTypes = {
          apc: 'APC_UPLOAD_PROGRESS',
    	  uploadprogress: 'UPLOAD_IDENTIFIER'
    	};
    	
        $(this).find('input.form-submit.ajax-processed').mousedown(function(event) {
          element.find('input.form-file').hide();
          progress = element.find('input.file-progress');
          if (progress.length && config.progressType && progressTypes[config.progressType]) {
            $(this).hide();
            var originalName = progress.attr('name');
            progress.attr('name', progressTypes[config.progressType]);
            
            // Restore the original name after the upload begins.
            setTimeout(function() { progress.attr('name', originalName); }, 1000);
          }
        });
      });
    }
  }


  // ==========================================
  // Element utility functions
  // ==========================================
  /**
   * Global functions and variables.
   */
  IWapi.elements = IWapi.elements || {
    instances: {},
    addElement: function (element) {
      var ID = element.getID();
      this.removeElement(ID);  // Ensure that previous instances with the same ID are cleaned up before lost.
      if (element) {
        this.instances[ID] = element;
      }
    },
    removeElement: function (elementID) {
      if (this.instances[elementID]) {
        // Ensure that any clean-up code is run before destroying an element.
        this.instances[elementID].dispose();
        delete this.instances[elementID];
      }
    },
  };


  // ==========================================
  // ImageSelector definition
  // ==========================================

  /**
   * A pop-up image selector that can display a preview and a more
   * robust selection interface.
   */
  IWapi.elements.ImageSelector = function(element, options) {
    this.element = element;
    this.select = element.find('select.form-select');
    this.value = element.find('input.image-selector-value');
    
    this.imageStyle = options.style;
    this.extensions = options.extensions || 'jpg jpeg png gif';
    this.extensionRegExp = new RegExp("\\.(" + options.extensions.replace(/\s+/g, '|') + ")$");

    // If there is a selection dialog, we need to support the enhanced selection
    if (this.select.length > 0) {
      // Create the elements to display a preview image.
      element.find('label').first().after($('<div class="image-preview"><img src="" /><label title=""></label></div>'));
      this.select.change($.proxy(this, 'selectionChanged'));
      this.select.val(this.value.val());
      this.selectionChanged();
      
      this.content = $('<div class="popup-selection-content"/>');	
      this.popupButton = $('<a href="#" class="button">Select Image</a>')
        .click($.proxy(this, 'displayDialog'));

      this.select.after(this.popupButton).hide();
      this.toggler = $('<a href="#">Simple Mode</a>');
      this.popupButton.wrap('<div class="popup-selection-wrapper"/>')
        .after(this.toggler);

      this.toggler.click($.proxy(this, 'toggleSelect'))
        .wrap('<div class="select-mode-toggle"/>');
    }
    // validate the image upload file client-side to avoid unnecessary uploads that will fail.
    element.find('input.form-file').change($.proxy(this, 'validateExtension'));
    
    // Add itself to the managed list of current element instances.
    IWapi.elements.addElement(this);
  };
  
  /**
   * Function that knows where to find the Element ID.
   * In the case of the ImageSelector, the ID is attached to its
   * value input.
   * 
   * @returns string
   *  Value which should be used as the ID of this element.
   */
  IWapi.elements.ImageSelector.prototype.getID = function() {
    return this.value.attr('id');
  } 
  
  /**
   * Function to properly clean-up resources of an element.
   */
  IWapi.elements.ImageSelector.prototype.dispose = function() {
    // remove allocated dialog and selector content.
	if (this.content) {
      this.content.dialog('destroy');
      this.content.empty().remove();
	}
  }
  
  /**
   * Parse the URL from the value stored in the select > options tags.
   * The function can also optionally convert the URL to the image style
   * setting of the ImageSelector.
   * 
   * @param rawValue
   *  The string value containing the full value from which to parse the image url
   * @param applyStyle
   *  Boolean which determines if we want the url as is, or changed to the ImageSelector's
   *  image display style. TRUE means to use the style, and FALSE means to return the URL
   *  without modifications.
   *  
   * @returns string
   *  The URL variation requested.
   */
  IWapi.elements.ImageSelector.prototype.getImageUrl = function(rawValue, applyStyle) {
    var matches = (/^\[(\d+)\] (.*)$/i).exec(rawValue);
    if (matches) {
      if (applyStyle) {
        return (this.imageStyle && this.imageStyle.length > 0) ?
    	   matches[2].replace(/\/styles\/thumbnail/, '/styles/' + this.imageStyle) : matches[2].replace(/\/styles\/thumbnail\/[^\/]*/, '');
      }
      else {
        return matches[2];
      }
    }
    return '';
  };

  /**
   * Opens the ImageSelector dialog box, and initializes it
   * if it has not already been initialized.
   */
  IWapi.elements.ImageSelector.prototype.displayDialog = function() {
    if (this.content.isInit) {
      var value = this.select.val();
      this.content.dialog('open');
      
      this.content.find('li input[value="' + value + '"]')
        .parent('.image-selector-preview').click();
    }
    else {
      this.content.isInit = true;
      this.content.dialog({
        title: "Select Image",
        dialogClass: "image-selector-popup",
        modal: true,
        resizable: false,
        height: 520, width: 680
      });

      var list = $('<ul/>');
      var context = this;
      var value = this.select.val();
      this.select.children('option').each(function() {
        var path = context.getImageUrl($(this).val());
    	if (path.length > 0) {
          var thumb = $('<li class="image-selector-preview" />');
          thumb.append('<input type="hidden" value="' + $(this).val() + '"/>')
          thumb.append('<img src="' + path + '" />');
          thumb.append('<label title="' + $(this).text() + '">' + $(this).text() + '</label>');
          
          list.append(thumb);
    	  thumb.click(function(event) {
            $(this).siblings('.selected').removeClass('selected');
            $(this).addClass('selected');
            return false;
      	  });
          if ($(this).val() == value) {
            thumb.addClass('selected');
          }
        }
      });
      
      var actions = $('<div class="dialog-actions"/>')
        .append($('<a href="#" class="button">Select</a>').click($.proxy(this, 'selectImage')))
        .append($('<a href="#">Cancel</a>').click($.proxy(this, 'cancel')));

      this.content.append(list).append(actions);
      list.wrap('<div class="select-container clearfix"/>');
    }
    return false;
  }
  
  /**
   * Event triggered when the selected image has been changed. Mainly assures
   * that the dialog elements, and select form element stays in-sync. Also updates
   * the preview image to the new selection.
   */
  IWapi.elements.ImageSelector.prototype.selectionChanged = function(event) {
    var path = this.getImageUrl(this.select.val(), true);
    var label = this.select.find('option[value="' + this.select.val() + '"]').text();
    var img = this.element.find('.image-preview img').attr('src', path);
    this.element.find('.image-preview label').text(label).attr('title', label);
  }
  
  /**
   * Toggles the user interface between the selection dialog and select
   * form element.
   */
  IWapi.elements.ImageSelector.prototype.toggleSelect = function(event) {
    if (this.toggler.text() == 'Simple Mode') {
      this.toggler.text('Advanced Mode');
      this.popupButton.hide();
      this.select.show();
    }
    else {
      this.toggler.text('Simple Mode');
      this.popupButton.show();
      this.select.hide();
    }
    return false;
  }
  
  /**
   * An image has been selected in the dialog, and the result needs
   * to be propagated to the form select value.
   */
  IWapi.elements.ImageSelector.prototype.selectImage = function() {
	var value = this.content.find('ul .image-selector-preview.selected input').val();

    this.content.dialog('close');
	this.select.val(value).change();
    return false;
  }

  /**
   * The close event was called on the selection dialog. Closes the dialog.
   */
  IWapi.elements.ImageSelector.prototype.cancel = function() {
    this.content.dialog('close');
    return false;
  }
  
  /**
   * Function that validates the file upload input is populated with the correct
   * extension. This will is primarily used with the "onChange" to prevent sending
   * an upload that will ultimately fail.
   */
  IWapi.elements.ImageSelector.prototype.validateExtension = function(event) {
    if (!this.extensionRegExp.test(event.target.value)) {
      alert(Drupal.t("The selected file @filename cannot be uploaded. Only files with the following extensions are allowed: @extensions.", {
        '@filename': this.value,
        '@extensions': this.extensions.replace(/\s+/g, ', ')
      }));
      $(this).val('');
      return false;
    }
  }
  
})(jQuery);