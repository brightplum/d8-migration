// Use the Drupal namespace so iwapi.js is not required
Drupal.iwapi = Drupal.iwapi || {};


// JQuery enabled JS
(function($) {
  Drupal.behaviors.iwSelectionTable = {
    attach: function(context, settings) {
      $('table.enable-items-table', context).once('enable-table', function() {
    	var inactive = $('<fieldset class="collapsible collapsed form-wrapper">')
    	  .append('<legend><span class="fieldset-legend">Inactive Items</span></legend>')
    	  .append('<div class="fieldset-wrapper">');
    	
    	$(this).after(inactive);
    	
    	// Attach Drupal behaviors and set the wrapper as the bin for inactive items.
    	Drupal.attachBehaviors(inactive.parent());
    	var inactiveBin = inactive.find('.fieldset-wrapper');
 
        $('tr.draggable', $(this)).each(function() {
          var check = $('input[type="checkbox"]', $(this));
          if (check.length > 0)
            new Drupal.iwapi.TableEnableElement($(this), inactiveBin);
        });
      });
    }
  };
 
  /**
   * Constructor for table elements that can be enabled and disabled.
   */
  Drupal.iwapi.TableEnableElement = function (element, bin) {
    this.element     = element;
    this.inactiveBin = bin;

    this.ctrlInput   = $('input[type="checkbox"]', element);
    this.ctrlWrapper = this.ctrlInput.parent();
    this.ctrlParent  = this.ctrlWrapper.parent();
	
    this.toggleEnabled();
    this.ctrlInput.click($.proxy(this.toggleEnabled, this));
  };
  
  /**
   * Handles toggling the on and off state of the table elements.
   */
  Drupal.iwapi.TableEnableElement.prototype.toggleEnabled = function() {
    if (this.ctrlInput.is(":checked")) {
      this.element.show();
      this.ctrlParent.append(this.ctrlWrapper);
    }
    else {
      this.element.hide();
      this.inactiveBin.append(this.ctrlWrapper);
    }
    
    // Restripe the table after completing the placement of items.
    $('> tbody > tr:visible, > tr:visible', this.element.parents('table'))
      .removeClass('odd even')
      .filter(':odd').addClass('even').end()
      .filter(':even').addClass('odd');
  };

  /**
   * Create sortable bins, where items can be grouped and arranged.
   * - Parent items will also be sortable if the "sortable" class is applied.
   */
  Drupal.behaviors.iwSortBins = {
    attach: function(context, settings) {
      $(".sort-bin-wrapper", context).once("sort-bins", function() {

      
      	// Combine specific and default configurations.	
    	var wrapperId = $(this).attr("id");
    	var events = { sortreceive: IWapi.SortBins.receiveItem };

        if (settings.iwSortBins && settings.iwSortBins[wrapperId]) {
          var config = settings.iwSortBins[wrapperId];
          
          // Create the holding tank for inactive items
          if (config['inactive_bin']) {
      	    $(this).append('<div class="sort-bin"><div class="handle-title">Inactive</div></div>');
          }
          
          var handler, handlers = [ 'sortreceive', 'sortupdate', 'sortstart', 'sortstop' ];
          
          // Attach only events that resolve to functions.
          for (var i = 0; i < handlers.length; ++i) {
        	if (config[handlers[i]] && $.isFunction(handler = IWapi.utils.getObject(config[handlers[i]]))) {
              events[handlers[i]] = handler;
        	}
          }
        }
    	
    	var group = { bins: {}, items: [] };
        $("> .sort-bin", this).each(function() {
          var bin = new IWapi.SortBins.Bin($(this), group);
          group.bins[bin.value] = bin;

          bin.container.sortable({
            connectWith: '#' + wrapperId + " > .sort-bin > .container", 
            items: ".bin-item",
            tolerance: 'pointer',
            placeholder: 'ui-state-highlight',
            forcePlaceholderSize: true,
            zIndex: 100
          })
          
          // bind all configured events
          for (var name in events) {
            bin.container.bind(name, group, events[name]);
          }
        });

        $('> .bin-item', this).each(function() {
    	  group.items.push($(this));
    	  $('select.parent-value', this).change(group, IWapi.SortBins.selectChange);
        });
        
        for (var i = 0; i < group.items.length; ++i) {
          $('select.parent-value', group.items[i]).trigger('change');
        }

        $(this).sortable({
    	  items: "> .sort-bin.sortable",
          handle: 'div.handle-title',
          tolerance: 'pointer',
          zIndex: 99
        }).bind('sortupdate', group, Drupal.behaviors.iwSortBins.updateGroup);
      });
    },
    updateGroup: function(event, ui) {
      var group = $(ui.item);
      var weight = 0;

      group.parent().children(".sort-bin").each(function() {
        $('input.bin-weight, select.bin-weight', this).val(weight++);
      });
    }
  };
  
  IWapi.SortBins = {
    Bin: function(bin, group) {
      this.active = true;
      this.bin = bin;
      this.group = group;
      this.container = $('<div class="container clearfix">');
      this.container.appendTo(this.bin);
      
      var value = $("input.bin-value", bin).val();
      this.value = value || 'inactive';
      
      $("input.bin-weight, select.bin-weight", this.bin).parent('.form-item').hide();
    },
    receiveItem: function(event, ui) {
      var item = $(ui.item);
      var bins = event.data.bins;
      var value = item.parents('.sort-bin').find("input.bin-value").val();

      $("select.parent-value", item).val(value);
    },
    selectChange: function(event) {
      var value = $(this).val();
      var bins = event.data.bins;
      var item = $(this).parents('.bin-item');
      
      var bin = bins[value] ? bins[value] : bins.inactive;
      bin.container.append(item);
      item.trigger('sortupdate', { item: item, sender: bin.container });
    }
  };
  
  IWapi.SortBins.Bin.prototype.enable = function() {
    if (!this.active) {
      this.bin.removeClass('bin-disabled').show();

      var items = this.group.items;
      for (var i = 0; i < items.length; ++i) {
        $('select.parent-value > option[value="' + this.value + '"]', items[i])
         .attr('disabled', '').show();
      }
    }
    this.active = true;
  };
  
  IWapi.SortBins.Bin.prototype.disable = function() {
    if (this.active) {
      var inactive = this.group.bins.inactive;
      this.container.find('> .bin-item').each(function() {
        $('select.parent-value', this).val('');
        inactive.container.append(this);
      });

      var items = this.group.items;
      for (var i = 0; i < items.length; ++i) {
        $('select.parent-value > option[value="' + this.value + '"]', items[i])
          .attr('disabled', 'disabled').hide();
      }
      this.bin.addClass('bin-disabled').hide();
    }
    this.active = false;
  };

}(jQuery));