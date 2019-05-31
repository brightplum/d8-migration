

(function($) {
  
  IWapi.datalock = {
    CreateTimer: function(lock) {
      var timerObj = this;
     
      // Internal properties
      this.attachTo = 'body';
      this.start    = new Date().getTime(); // When we started counting.
      this.time     = lock.remaining;       // Seconds remaining for the lock.
      this.username = lock.username;        // Name of the user who owns lock.
      this.captured = lock.captured;        // Does this user have the lock?
      this.releaseOnUnload = true;          // Release lock when beforeUnload event triggers?
      
      // IFF there is a form associated to this lock, the form should manage
      // the lock through its submission handling. Any other causes of the page
      // to refresh or requests to leave, will result in releasing the lock.
      if (lock.form && lock.form.length) {
        this.attachTo = '#' + lock.form;
    	$(this.attachTo).submit(function() {
          timerObj.releaseOnUnload = false;
        });
      }

      // Build the display / theming      
      var theme = this.captured ? 'themeCaptured' : 'themePending';
      IWapi.datalock[theme](this, this.attachTo);
      
      // ----------------------------
      // Create events and functions
      this.lockAction = new IWapi.ajax.Action('LockActions', null, {
        useGet: true,
        url: 'datalock/' + lock.token
      });
      
      // Start the intervals.
      this.startTimers();
    }
  }
  
  // ----------------------------
  // Timer prototype definition
  IWapi.datalock.CreateTimer.prototype = {

	startTimers: function() {
      if (!this.running) {
        // Create the interval timers, tick 0.2/s, ping every 3.0/s
        this.intTickRef = window.setInterval($.proxy(this, 'updateDisplay'), 200);
        this.intPingRef = window.setInterval($.proxy(this, 'ping'), 180000);
        this.running = true;
	  }
	},
      
    /**
     * Update the timer display.
     */
    updateDisplay: function() {
      var now   = new Date().getTime();
      var diff  = this.time - (now - this.start)/1000;
      
      if (diff <= 0) {
        this.complete();
      }
      else {
        var hours = Math.floor(diff/60);
        var mins  = Math.floor(diff) % 60;
        
        mins = (mins < 10) ? '0' + mins : mins.toString();
        this.countElem.text(hours.toString() +':' + mins);
      }
    },
    
    /**
     * Close the interval timers, and create a refresh prompt.
     */
    complete: function() {
      clearInterval(this.intTickRef);
      clearInterval(this.intPingRef);
      this.running = false;
      
      if (!this.captured) {
        this.container.html(
          'This content is now available for editing! Refresh' +
          'this page to edit the content, but remember that you ' + 
          'might not be the only one waiting.' +
          '<div><a href="#">Reload page</a>' 
        );
        
        // Reload the page, this will cause
        $('a', this.container).click(function() {
          location.reload();
          return false;
        });
      }
      else {
        this.countElem.text('EXPIRED');
      }
    },
    
    /**
     * Ping the server for an update of the lock
     * status. Has it been released, Ownership
     * change, re-sync timer, etc...
     */
    ping: function() {
      this.lockAction.execute({ oplock: 'ping' });
    },
    
    /**
     * Release the lock, either before leaving or
     * by request if user owns this lock.
     */
    release: function() {
      if (this.captured && this.releaseOnUnload) {
        var url = this.lockAction.url;
        
        // Minimal request to release the currently held lock.
        // We do not want a response or to trigger updates.
        $.ajax({
          url: IWapi.utils.buildURL(url, { oplock: 'release' }),
          async: false,
        });
        
        return null;
      }
    }
  }

  /**
   * Generate the captured display version of the timer block
   */
  IWapi.datalock.themeCaptured = function(timer, attachTo) {
    timer.container = $('#datalock-timer');
    if (timer.container.length == 0) {
      timer.container = $('<div id="datalock-timer" class="modal captured">').appendTo(attachTo);
    }
    
    timer.container.html(
      "<strong>It's your turn to add to your Team's workspace!</strong>" + 
      '<div>You have: <span class="counter"></span> minutes</div>' + 
      '<div class="actions clearfix">' +
        '<a class="refresh-timer" href="#">I need more time</a>' +
      '</div>'
    );
    
    // Wire up the different HTML elements for good access.
    timer.container = $('#datalock-timer');
    timer.userElem  = $('.username', timer.container);
    timer.countElem = $('.counter', timer.container);
    
    // Link the refresh action to the button
    $('a.refresh-timer', timer.container).click(function() {
      timer.refreshing = true;
      timer.lockAction.execute({ oplock: 'refresh' });
      return false;
    });
  }
  
  /**
   * Theme the timer as pending. Waiting for the lock to be freed.
   */
  IWapi.datalock.themePending = function(timer, attachTo) {
    var userStr = (timer.username && timer.username.length)
      ? '<span class="username">' + timer.username + '</span>' : 'Another user';

    timer.container = $('#datalock-timer');
    if (timer.container.length == 0) {
      timer.container = $('<div id="datalock-timer" class="modal">').appendTo(attachTo);
    }
    
    timer.container.html(
      "<div>One of your teammates is adding to your Team's workspace right now.</div>" +
      '<div>There are <span class="counter"></span> minutes left before you can have a turn.<div>' + 
      '<div>Note: The timer may jump back up to 15 minutes if your teammate asks for ' + 
      'more time, or your teammate may finish early.</div>'
    );

    timer.userElem  = $('.username', timer.container);
    timer.countElem = $('.counter', timer.container);
  }

  
  /**
   * AJAX event to record the current state of this
   * lock. This will occur after requests to refresh
   * the lock, or after trying to capture.
   */
  IWapi.ajax.commands.updateDatalock = function(ajax, data, status) {
    var lockTimer = IWapi.datalock.timer;
    
    lockTimer.start = new Date().getTime();
    lockTimer.time  = data.remaining;
    lockTimer.captured = data.captured;
    
    if (lockTimer.time > 0 && !lockTimer.running) {
      lockTimer.startTimers();
    }
    
    // Lock has changed hands.
    if (lockTimer.username != data.username) {
      lockTimer.username = data.username;
      lockTimer.userElem.text(lockTimer.username);
    }
    
    // A request to refresh the timer was made, keep an on if it succeeded.
    if (lockTimer.refreshing) {
      lockTimer.refreshing = false;

      // Tried to re-capture but failed, someone else must have it.
      if (!lockTimer.captured) {
        IWapi.datalock.themePending(lockTimer, lockTimer.attachTo);
      }
    }
  }
  
  $(function() {
    // Bind the AJAX update call to the proper Drupal AJAX command.
    Drupal.ajax.prototype.commands.update_datalock = IWapi.ajax.commands.updateDatalock;
  
    // Create a datalock timer which will inform the user of the current lock state.
    if (Drupal.settings.iwDatalock) {
      var lock = Drupal.settings.iwDatalock;
      
      // Create a new timer and make it available to 
      IWapi.datalock.timer = new IWapi.datalock.CreateTimer(lock);
      
      // If user tries to leave this page, release the lock.
      $(window).bind('beforeunload', $.proxy(IWapi.datalock.timer, 'release'));
    }
  });
  
}(jQuery));
