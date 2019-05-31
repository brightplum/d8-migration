<?php
/**
 * 
 */

class DiscourseRequest extends DiscourseTransaction {

  /**
   * 
   */
  function fetchCategories() {
    static $categories;

    if (!isset($categories)) {
      $categories = array();
      
      $url  = $this->buildUrl('categories.json');
      $resp = $this->execute($url);

      foreach ($resp->data->category_list->categories as $category) {
        $categories[$category->id] = array(
          'name' => $category->name,
          'slug' => $category->slug,
          'color' => $category->color,
          'text_color' => $category->text_color,
          'description' => $category->description_text,
        );
      }
    }
    
    return $categories;
  }
  
  /**
   * 
   */
  function fetchComments($entity, $type, array $params = array('best' => 5)) {
    list($id, $rev, $bundle) = entity_extract_ids($type, $entity);
    
    $dc = db_select('discourse_comments', 'dc')
      ->fields('dc', array('topic_id', 'topic_slug'))
      ->condition('entity_type', $type)
      ->condition('entity_id', $id)
      ->execute()->fetchObject();

    if (!$dc) {
      if (!variable_get(DISCOURSE_COMMENTS_VAR_POST_MISSING, FALSE)) {
        return NULL;
      }

      $post = new DiscoursePost($this->server, $this->apiKey, $this->user);
      $dc   = $post->createTopic($entity, $type);

      // Creating a Topic provides the correct set of params to return.
      return (object)array(
        'id' => $dc->topic_id,
        'slug' => $dc->topic_slug,
        'posts_count' => 1,
        'filtered_post_counts' => 1,
        'posts' => array(),
        'participants' => array(
          (object)array(
            'id' => $dc->user_id,
            'username' => $dc->username,
            'uploaded_avatar_id' => $dc->uploaded_avatar_id,
            'avatar_template' => $dc->avatar_template,
          ),
        ),
      );
    }

    // Make the actual request to get the comments.
    $url  = $this->buildUrl("t/$dc->topic_id/wordpress.json", $params);
    $resp = $this->execute($url);

    if (empty($resp->data->slug)) {
      $resp->data->slug = $dc->topic_slug;
    }

    return $resp->data;
  }

}
