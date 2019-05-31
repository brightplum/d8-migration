<?php
/**
 * @file
 * 
 */

class DiscoursePost extends DiscourseTransaction {
  
 
  function createTopic($entity, $type) {
    list($id, $rev, $bundle) = entity_extract_ids($type, $entity);
    
    // Determine which category to drop this content into.
    $entInfo  = variable_get(DISCOURSE_COMMENTS_VAR_ENTITIES, array());
    $category = empty($entInfo[$type][$bundle]['category'])
      ? 1 : $entInfo[$type][$bundle]['category'];
    
    $cloned = clone $entity;
    $cloned->status = NODE_PUBLISHED;
    $cloned->discoursePreview = TRUE;
    
    // Find and build content to get included into the topic
    if (method_exists($cloned, 'view')) {
      $content = $cloned->view('teaser');
    }
    elseif (function_exists($type .'_view')) {
      $callback = $type .'_view';
      $content = $callback($cloned, 'teaser');
    }

    $content['links']['#access'] = FALSE;
    $content['csp']['#access']   = FALSE;
    unset($content['#contextual_links']);

    // Make all links absolute and clean out non-display items.
    $canonical = variable_get(DISCOURSE_COMMENTS_VAR_CANONICAL, $_SERVER['HTTP_HOST']);
    $pattern = array('#(<(?:a|img)\s[^>]*?(?:href|src)=[\'"])(/[^\'"]*)#im', "#\r|\n#m");
    $replace = array('$1http://'. $canonical .'$2', '');
    
    // If current host does not match canonical, update links to point correctly.
    if ($canonical !== $_SERVER['HTTP_HOST']) {
      $pattern[] = '#(<(?:a|img)\s[^>]*?(?:href|src)=[\'"]https?://)'. $_SERVER[HTTP_HOST] .'(/[^\'"]*)#im';
      $replace[] = '$1'. $canonical .'$2';
    }
    
    // Create the correct embed url.
    $uri = entity_uri($type, $cloned);
    $embed_url = 'http://'. $canonical . url($uri['path'], array('absolute' => FALSE) + $uri['options']);
    
    $data = array(
      'title' => entity_label($type, $cloned),
      'raw' => preg_replace($pattern, $replace, render($content)),
      'category' => $category,
      'archetype' => 'regular',
      'skip_validations' => 'true',
      'embed_url' => $embed_url,
    );

    // Attach a link back to the original content.
    $data['raw'] .= '<ul><li>'. l('Back to full article', $embed_url) .'</li></ul>';
    
    $url = $this->buildUrl('posts', array(), FALSE);
    $resp = $this->execute($url, $data);
    $data = $resp->data;
    
    // Save post settings
    db_merge('discourse_comments')
      ->key(array('entity_type' => $type, 'entity_id' => $id))
      ->fields(array(
        'topic_id'   => $data->topic_id,
        'topic_slug' => $data->topic_slug,
        'posted'     => REQUEST_TIME,
      ))
      ->execute();
 
    return $data;
  }
  
  /**
   * Create a new category for adding content to.
   */
  function createCategory($name, $color, $textcolor = '333333') {
    $url = $this->buildUrl('categories.json', array());

    $data = array(
      'name' => $name,
      'color' => $color,
      'text_color' => $textcolor,
    );
    
    $resp = $this->execute($url, $data, TRUE);
    return $resp->data;
  }

}
