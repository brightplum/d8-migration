
// Create the namespace for iwFormatter widgets.
iwFormatter = {
  widgets: { lookUp: {} }
};


(function($) {
    
  if (Drupal.jsAC) {
    var callback = Drupal.jsAC.prototype.select;
    
    // Alter the autocomplete selection to notify us a change was made.
    Drupal.jsAC.prototype.select = function(node) {
      callback.apply(this, [node]);                // run the current callback first.
      $(this.input).trigger('autocompleteSelect'); // notify that autocomplete triggered.
    }
  }
  
  Drupal.behaviors.entityRefWidget = {
    attach: function(context, settings) {
      if (settings.iwformatter && settings.iwformatter.entityRefWidgets) {
        var refSettings = settings.iwformatter.entityRefWidgets;
        
        $('.field-widget-iwformatter-entityref input.form-autocomplete', context).once('ref-widget', function() {
          var id = $(this).attr('id');
          var ac = $('#' + id + '-autocomplete').val();
          var matches = /(?:\?q=|\/)(entityreference\/autocomplete\/[^\/]+\/(.+))($|&)/i.exec(ac);
          var info = matches[2].split(/\//);

          if (info) {
            var labels  = ['field', 'entityType', 'bundle', 'srcId'];
            var fieldInfo = {};
            for (var i in labels) fieldInfo[labels[i]] = info[i].replace('-', '_');

            if (refSettings[fieldInfo.field] && !iwFormatter.widgets.lookUp[id]) {
              iwFormatter.widgets.lookUp[id] = new iwFormatter.widgets.EntityRefWidget($(this), fieldInfo, refSettings[fieldInfo.field]);
            }
          }
        });
      }
    }
  };

  /**
   * Object constructor for creating entity widget objects.
   */
  iwFormatter.widgets.EntityRefWidget = function(element, info, settings) {
    var self = this;
    this.item = element;
    this.fieldInfo = info;

    var bundles = [];
    for (var k in settings.bundles) bundles.push(k);

    for (var k in settings) {
      if ($.isArray(settings[k])) settings[k] = settings[k].pop();
    }

    var editLink = this.createLink('fa-edit', 'Edit referenced content', settings.edit);
    var viewLink = this.createLink('fa-eye', 'View referenced content', settings.view);
    
    viewLink.unbind('click');
    viewLink.click(function() {
      self.updateId();

      var uri = settings.view.replace("%entity_id", self.entityId.toString());
      var uri = uri.replace("/ajax/", '/nojs/');
      $(this).attr('href', IWapi.utils.buildURL(uri));
      $(this).attr('target', '_blank');
    });
    

    if (bundles.length == 1) {
      var createUrl = settings.create.replace("%bundle", bundles[0]);
      this.createLink('fa-plus', 'Create new content', createUrl, true).addClass('active');
    }
    else {
      // TODO: create a dialog which allows selection of bundle types.
    }

    var updateLinks = function() {
      self.updateId();
  
      var method = self.entityId ? 'addClass' : 'removeClass';
  
      editLink[method]('active');
      viewLink[method]('active');
    }

    // Attach events last after everything is setup.
    this.item.bind('change', updateLinks)
      .bind('autocompleteSelect', updateLinks);
    
    // Update the links statuses to the initial settings.
    updateLinks();
  };

  /**
   * Add object functionality, scoped with "this"
   */
  iwFormatter.widgets.EntityRefWidget.prototype = {
    createLink: function (icon, msg, url, isStatic) {
      var self = this;
      var link = $('<a href="#" title="' + msg + '" class="widget-icon"><i class="fa ' + icon + '"></i></a>');
      
      if (isStatic) {
        link.click(function(event) {
          var action = new IWapi.ajax.Action(self.item.attr('id'), self.item, { url: url });
          action.execute();
            
          event.preventDefault();
          return false;
        });
      }
      else {
        link.click(function(event) {
          self.updateId();

          if (self.entityId) {
            var action = new IWapi.ajax.Action(self.item.attr('id'), self.item, {
              url: url.replace("%entity_id", self.entityId.toString())
            });
            action.execute();
          }

          event.preventDefault();
          return false;
        });
      }

      this.item.after(link);
      return link;
    },
    
    updateId: function() {
      var matches = (/\((\d+)\)[\s"']*$/i).exec(this.item.val());

      this.entityId = (matches && matches[1] !== "0") ? parseInt(matches[1]) : null;
    }
  };


  /**
   * Add an onReady callback to add additional AJAX callbacks.
   */
  $(function() {
    Drupal.ajax.prototype.commands.entity_saved = function(ajax, data, status) {
      var elem = $(ajax.original ? ajax.original : ajax.element);

      elem.val(data.label + " (" + data.entityId + ")");
      elem.trigger("changed").trigger("autocompleteSelect");
    }
  });

}(jQuery));

