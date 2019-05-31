
(function($) {

  IWapi.tables = { builders: {} };


  /**
   * Create a new instance of a new table builder
   * based on a form and it's elements.
   */ 
  IWapi.tables.TableBuilder = function(element) {
    this.element = element;
    this.events  = [];
    this.order   = [];
    this.columns = {};
    this.fields  = {};
    
    this.sortableOpts = {
      connectWith: '#' + element.attr('id') + ' .content-container',
      items: ".draggable-field",
      tolerance: 'pointer',
      handle: 'div.handle-title',
      placeholder: 'ui-state-highlight',
      forcePlaceholderSize: true,
      zIndex: 100
    };

    // Attach only events that resolve to functions.
    var handlers = [ 'sortreceive', 'sortupdate', 'sortstart', 'sortstop' ];
    for (var i = 0; i < handlers.length; ++i) {
      if ($.isFunction(this[handlers[i]]))
        this.events.push(handlers[i]);
    }
    
    this.canvas = $('.table-canvas-wrapper', element);
    if (!(this.canvas.length > 0)) {
      this.canvas = $('<div class="table-canvas-wrapper">');
      element.prepend(this.canvas);
    }
    
    var self   = this;
    var fields = $('.draggable-field', element);
        
    // Find out which columns are available
    $('input[type="hidden"].column-value', element).each(function() {
      self.addColumn($(this).val(), $(this).closest('.column-wrapper'));
    });
    
    self.disabled = $('<div class="disabled-fields content-container">').sortable(self.sortableOpts);
    
    // Attach events relating to sortable, do this last so
    // events don't trigger until everything is complete.
    for (var k in self.events) {
      var eName = self.events[k];
      self.disabled.bind(eName, { column: '' }, $.proxy(self, eName));
    }
 
 
    self.canvas.after(self.disabled);
    self.disabled.append(fields)
      .wrap('<div class="disabled-wrapper">')
      .before('<h3>Disabled Fields</h3>');
    
    // All columns have been generated, start putting fields where they belong
    fields.each(function() {
      var weight = $('.weight-value', this), parent = $('.parent-column', this);
      var column = parent.val(), weightVal = parseInt(weight.val());
   
      // Hide things that users don't need to see.
      weight.closest('.form-item').hide();
      parent.closest('.form-item').hide();

      if (self.columns[column]) {
        $(this).detach();

        var container = self.columns[column].element;
        var siblings  = container.children('.draggable-field');

        // Insertion sort based on weight values.
        for (var i = 0; i < siblings.length; ++i) {
          if (parseInt($('.weight-value', siblings.get(i)).val()) > weightVal) {
            $(siblings.get(i)).before(this);
            return;
          }
        }

        container.append(this); // Only reached if no greater weight found.
      }
    });
  }    


  // Instance builder methods
  IWapi.tables.TableBuilder.prototype = {
    addColumn: function(colname, html, updateFields) {
      col = {
        value: colname,
        text: html.find('.column-label').text(),
        element: $('<div class="content-container">').sortable(this.sortableOpts)
      };
      html.append(col.element).appendTo(this.canvas);
      
      // Add information about the column
      this.columns[col.value] = col;
      this.order.push(col.value);
      this.updateColumnWidths();

      if (updateFields) {
        // Add the new column to the selectable items.
        $('.draggable-field select.parent-column', this.element).append(
          '<option value="' + col.value + '">' + col.text + '</option>'
        );
      }

      // Attach events relating to sortable, do this last so
      // events don't trigger until everything is complete.
      for (var k in this.events) {
        var eName = this.events[k];
        col.element.bind(eName, { column: col.value }, $.proxy(this, eName));
      }
    },
    
    removeColumn:function(col, updateFields) {
      var self = this;
      var idx  = this.order.indexOf(col.value);
        
      if (idx >= 0 && this.columns[col.value]) {
        if (updateFields) {
          var builderId = this.element.attr('id');
 
          // Remove option and relocate fields.
          $('#' + builderId + ' select option[value="' + col.value + '"]').each(function() {
            var select = $(this).parent();
            
            if (select.val() === col.value) {
              self.disabled.append(select.closest('.draggable-field'));
              select.val('');
            }
            
            $(this).remove();
          });
        }
        
        this.order.splice(idx, 1);
        this.columns[col.value].element.closest('.column-wrapper').remove();
        delete this.columns[col.value];
        
        this.updateColumnWidths();  // update the column displays
      }
    },
    
    updateColumnWidths: function() {
      var numCols  = this.order.length;
      var colWidth = (100 - numCols)/numCols; // 1% calculate space using margins

      for (var c in this.columns) {
        this.columns[c].element.parent().css({ width: colWidth.toString() + '%' });
      }
    },
    
    sortreceive: function(event, ui) {
      var item = $(ui.item);
      var col  = event.data.column;
  
      $('select.parent-column', item).val(this.columns[col] ? col : '');
    },
  
    sortupdate: function(event, ui) {
      var column = $(ui.item).parent('.ui-sortable');
      var weight = 0;

      column.children(".draggable-field").each(function() {
        $('input.weight-value, select.weight-value', this).val(weight++);
      });
    }
  };


  /**
   * Hook-up the table builder functionality.
   */
  $(function() {
    Drupal.ajax.prototype.commands.addTableColumn = function(ajax, data, status) {
      var builder = $(ajax.element).closest('.table-builder-settings');
      var id = builder.attr('id');
      
      if (id && IWapi.tables.builders[id]) {
        IWapi.tables.builders[id].addColumn(data.column, $(data.html), true);
      }
    };
    
    Drupal.ajax.prototype.commands.removeTableColumn = function(ajax, data, status) {
      var builder = $(ajax.element).closest('.table-builder-settings');
      var id = builder.attr('id');
      
      if (id && IWapi.tables.builders[id]) {
        IWapi.tables.builders[id].removeColumn({ value: data.column }, true);
      }
    };
    
    Drupal.ajax.prototype.commands.updateTableField = function(ajax, data, status) {
      var newMarkup = $(data.html);
      $('.weight-value', newMarkup).closest('.form-item').hide();
      $('.parent-column', newMarkup).closest('.form-item').hide();
     
      var settings = data.settings || ajax.settings || Drupal.settings;
      var original = $(ajax.element).closest('.draggable-field');
      Drupal.detachBehaviors(original, settings);
      
      original.replaceWith(newMarkup);
      Drupal.attachBehaviors(newMarkup, settings);
    }
    
    // Register all the table builders.
    $("form .table-builder-settings").each(function() {
       var id = $(this).attr('id');
       
       IWapi.tables.builders[id] = new IWapi.tables.TableBuilder($(this));
    });
  });
    
}(jQuery));
