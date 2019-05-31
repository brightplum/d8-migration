
var IWapi = IWapi || { utils: { } };

(function($) {  
  IWapi.utils.jqVer = $.fn.jquery.split('.')
    .map(function(i){return('00'+i).slice(-3)}).join('.');

  // box-sizing was added to jQuery 1.8.0
  var bboxCheck = IWapi.utils.jqVer < "001.008.000";

  IWapi.utils.setHeight = function(elem, height) {
    var o = (bboxCheck && elem.css('box-sizing') === 'border-box')
      ? 0 : elem.outerHeight() - elem.height();

    elem.height(height - o);
  }

  /**
   * Convert a CSS style dimension into pixel value.
   * This will check for the units and do conversions.
   * 
   * @param string size
   *  String representation 
   * @param object element
   *  The element which the size value belongs.
   * @param string dim
   *  The dimension this value belongs to (width|height).
   *  This is only important for '%'.
   * @returns float
   *  The value in pixels of the size.
   */
  IWapi.utils.parseCssSize = function(size, element, dim) {
    if (!size.length) return 0;
      
    dim = dim | 'width';
    var matches = /(\d+(?:\.\d*))?\s?(px|em|%|)/i.exec(size);

    if (matches && matches.length) {
      var value = parseFloat(matches[1]);
 
      if (isNaN(value)) return 0;

      switch (matches[2]) {
        case '':   // Empty, treat like pixel value.
        case 'px':
          return value; 
        case '%':
          return $(element).parent()[dim]() * value / 100;
        case 'em':
          return IWapi.utils.em2px(value, element);
      }
    }

    throw new 'Unsupported unit or value for size conversion.';
  }

  /**
   * Convert an 'em' value into a pixel value. This will be
   * a best estimate
   * 
   * @param float value
   *  Value in 'em' to convert.
   * @param object element
   *  The element for which the 'em' value was applied.
   * @returns float
   *  Value in pixels that equals the 'em' that were passed.
   */
  IWapi.utils.em2px = function(value, element) {
    var styles = {
      display: 'none', border: 0,
      height: 'auto',  margin: 0, padding: 0,
      fontSize: '1em', lineHeight: 1
    };
      
    var e = $('<div>&nbsp;</div>').css(styles).appendTo($(element).parent());
    var h = e.height();
    e.remove();
    
    return value * h;
  }

}(jQuery));

// -------------------------
// URLs and paths utilities
// -------------------------

/**
 * Determine if clean URLs are available or not, based
 * on the construction of the current path.
 * 
 * @returns boolean
 *  true use of clean URLs is detected
 */
IWapi.utils.useCleanURL = function() {
  // determine if clean URLs should be used.
  // the result is stored in a static variable "isClean" for later reuse.
  if (IWapi.utils.useCleanURL.isClean == undefined) {
    var url  = /(\?|&)q=/.exec(window.location.href);
    IWapi.utils.useCleanURL.isClean = (url == null && IWapi.utils.getCurrentPath().length > 0);
  }
  return IWapi.utils.useCleanURL.isClean;
};

/**
 * Gets the current page Drupal URL (excluding the query or base path).
 * This would be the Drupal internal path.
 * 
 * @returns string
 *   the current path.
 */
IWapi.utils.getCurrentPath = function() {
  if (IWapi.utils.getCurrentPath.path == undefined) {
    // can't use the useCleanURL() function, because it will cause a infinite recursive loop!
    var uri = /(\?|&)q=(.*)/.exec(window.location.href);
    IWapi.utils.getCurrentPath.path = (uri == null || uri.length < 2) ? window.location.pathname.replace(Drupal.settings.basePath, '') : uri[1];
  }
  return IWapi.utils.getCurrentPath.path;
};

/**
 * Build a URL based on a Drupal internal path. This function will test
 * for the availability of clean URL's and prefer them if available.
 * 
 * @returns string
 *  the valid Drupal URL based on values passed.
 */
IWapi.utils.buildURL = function(rawURL, params) {
  // leave absolute URL's alone
  if ((/^[a-z]{2,5}:\/\//i).test(rawURL)) {
    return rawURL;
  }

  rawURL = rawURL ? rawURL.replace(/^[\/,\s]+|([\/,\s]+$)/g, "") : "";

  var queryStr = "";
  var isCleanURL = IWapi.utils.useCleanURL() || rawURL.length == 0;
  if (params) {
    if (typeof(params) == "string") {
      queryStr = params;
    }
    else {
      for (var name in params) {
        queryStr += '&' + encodeURIComponent(name) + '=' + encodeURIComponent(params[name]);
      }
      queryStr = queryStr.substr(1);
    }
    if (queryStr.length > 0) {
      queryStr = (isCleanURL ? '?' : '&') + queryStr;
    }
  }
  
  return Drupal.settings.basePath + (isCleanURL ? '': "?q=") + rawURL + queryStr;  
};


// -------------------------
// Objects and inheritance
// -------------------------

/**
 * Utility function used to find an object based on a string name.
 *  IE: 'IWapi.mapkit.MarkerManager'
 *  
 * @returns Object
 *  the object matching the name, or NULL if it cannot be found.
 */
IWapi.utils.getObject = function(name) {
  if (!(name && name.split)) {
    return null;
  }

  var part;
  var ref = window;
  var parts = name.split('.');

  while (part = parts.shift()) {
    if ((ref = ref[part]) === undefined) { 
      return null;
    }
  }

  return ref;
};

/**
 * Allow us to properly extend JS classes, by copying the prototype.
 * This function properly inherits a prototype by creating a new
 * instance of the parent object first, so modifications to the subclas
 * do not effect the parent class.
 * 
 * Use jQuery.extend to simply override functions of object instances,
 * this function is preferred when multiple instance want to share
 * the a modified prototype.
 * 
 * @param Object subclass
 *  Function constructor which inherits it's prototype from parent
 * @param Object parent
 *  Base or super class.
 */
IWapi.utils.inherit = function(subclass, parent) {
  var overrides = subclass.prototype;

  // we need to copy this prototype, by assigning it to a function
  // without parameters. Otherwise, we'll get initialization errors.
  var tmp = function() {};
  tmp.prototype = parent.prototype;
  subclass.prototype = new tmp();

  // Reapply all the prototype function overrides, if needed.
  if (overrides) {  
    for (var i in overrides) {
      subclass.prototype[i] = overrides[i];  
    }  
  }
  subclass.prototype.parent = parent.prototype;
  subclass.prototype.constructor = subclass;
};
