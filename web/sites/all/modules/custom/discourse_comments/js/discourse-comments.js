
(function($) {

  Drupal.behaviors.discourseComments = {
    attach: function(context, settings) {
      var config = settings.discourseComments;
      var self   = this;
      this.config = config;

      // In order to request discourse topics we need to be able
      // to know the topic ID and Discourse host.
      if (!config || !config.topic.id || !config.host.length) {
        return;
      }
      
      config = $.extend({ params: { best: 5 } }, config);

      $('.discourse-comments', context).first().once('discourse', function() {
        var topic_url = config.host + "/t/" + config.topic.id + '/wordpress.json';
        self.commentsWrapper = $(this);
     
        $.ajax({
          url: topic_url,
          method: "GET",
          data: config.params,
          dataType: 'json',
          success: $.proxy(self, 'render'),
          error: $.proxy(self, 'error')
        });
        
      });
    },
    
    render: function(data, status, xhr) {
      var host = this.config.host;
      var topic = this.config.topic;
      var container = $('<div>').insertAfter('#discourse-comments-header');
      
      var count = (data.posts_count && !isNaN(data.posts_count))
        ? data.posts_count - 1 : 0;
 
      if (count) {  
        var numPosts = (count).toString();
          
        $('.comments-count', this.commentsWrapper).text(numPosts);
        $('.discourse-comments-link span.comment-count').text(numPosts);
        
        for (var idx in data.posts) {
          data.posts[idx].parity = idx & 0x01 ? 'odd' : 'even';
          container.append(Drupal.theme('discourseComment', host, topic, data.posts[idx]));
        }
      }
      else {
        $('.comments-count', this.commentsWrapper).text('0');
        container.append(Drupal.t('No comments yet, be the first!'));
      }
    },
    
    error: function(xhr, status, error) {  
      $('#discourse-comments-header').after(
        '<div class="message error">Unable to load comments from Discourse</div>'
      );
    }
  };

  Drupal.theme.prototype.discourseComment = function(host, topic, post) {
    if (!post.parity) post.parity = 'even';  
     
    var username = Drupal.checkPlain(post.username);
    var user_url = host + '/users/' + username + '/activity'; 
    var post_url = host + '/t/' + topic.slug + '/' + topic.id + '/' +  post.post_number;
    var avatar_url = post.avatar_template.replace(/\{size\}/, '45');

    var date = new Date(post.created_at);
    var age  = (Date.now() - date.getTime())/1000;
    
    var months = [
      'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
      'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'
    ];

    var formattedDate = (age > 23 * 3600)
      ? months[date.getMonth()] + ' ' + date.getDate() + ', ' + date.getFullYear()
      : Math.ceil(age / 3600) + ' hours ago';


    return '<div class="discourse-comment ' + post.parity + ' clearfix">'
      + '<a class="author-icon-link" href="' + user_url + '" target="_blank">'
        + '<img class="author-avatar" src="' + avatar_url + '" />' 
      + '</a>'
      + '<div class="post-body">'
        + '<div class="post-author">'
          + '<a href="' + user_url + '" target="_blank">'
            + '<span class="author-name">' + username + '</span>'
          + '</a>'
          + '<div class="post-date">' + formattedDate + '</div>'
        + '</div>'
        + post.cooked
        + '<a class="post-link" href="' + post_url + '" target="_blank">'
          + Drupal.t('View / Reply')
        + '</a>'
      + '</div>'
    + '</div>';
   };

}(jQuery));
