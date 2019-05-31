<?php 
/**
 * @file
 * Core Parse.ly Drupal functions.
 */

require_once 'section.inc';

class Parsley {

  protected   $creator;
  protected   $keywords;
  protected   $articleId;
  protected   $articleSection;
  protected   $context;
  protected   $dateCreated;
  protected   $headline;
  protected   $type;
  protected   $url;
  
  public function __construct($node) {

    $this->articleID = $this->setID($node);
    $this->creator = $this->setCreator($node);
    $this->dateCreated = $this->setDate($node);
    $this->keywords = $this->setTags($node);
    $this->articleSection = $this->setSection($node);
    
  }
    
/* ~~~ Setters (protected) ~~~ */     

  protected function setID($node) {
    
    $prefix = variable_get('parsely_content_id_prefix', '');
    if (!empty($prefix)) {
      $prefix = $prefix . '-';
    }
    
    return $prefix.$node->nid;
    
  } 
    
  // @TODO: profile this function.   
  protected function setCreator($node) {
    if (!isset($author_field)) { 
      
      if(variable_get('parsely_authors_field_type')==0) {

        return format_username($node);
      
      } elseif (variable_get('parsely_authors_field_type')==1) { 
          $node = menu_get_object();
          $author_field = (variable_get('parsely_authors_field'));      
          $author = (array)$node->$author_field;
          $author_node = (array)($node->$author_field);
          $author = $author_node[LANGUAGE_NONE][0]['value'];

          return $author;
      
      } elseif (variable_get('parsely_authors_field_type')==2) {
          $node = menu_get_object();
          $author_field = (variable_get('parsely_authors_field'));      
          $author_node = (array)($node->$author_field);
          $author = node_load($author_node[LANGUAGE_NONE][0]['nid']);

          return $author->title;

      } else {

        return format_username($node);
      }  
    }
  }
      
  protected function setDate($node) {
   
    $pub_date = NULL;
    if (property_exists($node, 'published_at') && is_numeric($node->published_at)) {
      $pub_date = $node->published_at;
    }
    else {
      $pub_date = $node->created;
    }

  return gmdate("Y-m-d\TH:i:s\Z", $pub_date);
}
      
  protected function setTags($node) {
    
    $vocabularies = variable_get('parsely_tag_vocabularies');
    if (!module_exists('taxonomy') || $vocabularies === NULL) {
      return array();
    }

    $active_terms_query = db_select('taxonomy_index', 'ti');
    $active_terms_query->join('taxonomy_term_data', 'ttd', 'ti.tid=ttd.tid');
    $active_terms_result = $active_terms_query->fields('ttd', array('name'))
    ->condition('ti.nid', $node->nid)
    ->condition('ttd.vid', $vocabularies, 'IN')
    ->execute();

    $tags = array();
    if ($active_terms_result->rowCount() > 0) {
      $tags = $active_terms_result->fetchCol();
    }

    return $tags;
  } 
  
  protected function setSection($node) {

    return parsely_get_section($node);
    
  } 
    
  
/* ~~~ Getters (public) ~~~ */ 
  
  public function getCreator() {    
    
    return $this->creator;
    
  }

  public function getDate() {    
    
    return $this->dateCreated;
    
  }

  public function getID() {    
    
    return $this->articleID;
    
  }

  public function getSection() {    
    
    return $this->articleSection;
    
  }

  public function getTags() {    
    
    return $this->keywords;
    
  }
  
  public function getURL($node) {
    
    $uri = entity_uri('node', $node);
    return url($uri['path'], array_merge($uri['options'], array('absolute' => TRUE)));

  }

}
