<?php 
/**
 * @file
 * Parse.ly Settings Class.
 */

class ParselySettings {

  private   $apiKey;
  private   $debug;
  private   $debugMsg;
  private   $ignorePaths;
  private   $imageField;
  private   $nodeTypes;
  private   $sections;
  private   $taxonomies;
  private   $thumbnailURL;
  private   $trackAuth;

  public function __construct($vars) {

    $this->apiKey = $this->setApiKey($vars);
    $this->thumbnailUrl = $this->setThumbnailUrl();
    $this->debug = $this->setDebug();
    $this->ignorePaths = $this->setPathsToIgnore();
    $this->nodeTypes = $this->setNodeTypes();
    $this->trackAuth = $this->setTrackAuth();
    $this->imageField = $this->setImageField();
    
  }
    
/* ~~~ Setters (protected) ~~~ */     

  protected function setApiKey($vars) {
    
    return variable_get('parsely_apikey');
    
  } 
  
  protected function setDebug() {    

    return variable_get('parsely_debug');

  }

  protected function setDebugMsg($msg, $loglink) {    

    $this->debugMsg = array(
      $msg => $loglink,
    );
    
    // no return.
    
  }

  protected function setImageField() {    

    return 'field_image';

  }


  protected function setNodeTypes() {    

    $parsley_nodes = variable_get('parsely_nodes'); 
    
    if (!isset($parsley_nodes)) { 
      if (user_access('')) {
        $loglink = l("Configure Parse.ly Settings", 'admin/config/search/parsely');
        WATCHDOG('Parse.ly', "No tracked node types found. Please configure: $loglink");
        $this->setDebugMsg('Have you selected which node types to track?', $loglink); 
      } 
      return;
    } else {       }
    
    $tracked_node_types = array();  

    foreach ($parsley_nodes as $bid => $track) {
      if ($track !== 0 ) { 
        array_push($tracked_node_types, $parsley_nodes[$bid]);    
      }  
    }

    return $tracked_node_types;  

  } 

  protected function setPathsToIgnore() {    

    return variable_get('parsely_ignore');

  }

  protected function setThumbnailUrl() {    

    return variable_get('parsely_metadata_thumbnail_url', '');

  }
  
  
  protected function setTrackAuth() {    

    return variable_get('parsely_track_auth_users');

  }
  
/* ~~~ Getters (public) ~~~ */ 
  
  public function getApiKey() {    

    return $this->apiKey;
    
  }

  public function getDebug() {    

    return $this->debug;
    
  }

  public function getDebugMsg() {    

    return $this->debugMsg;
    
  }

  public function getImageField() {    

    return $this->imageField;
    
  }


  public function getNodeTypes() {    

    return $this->nodeTypes;
    
  }

  public function getPathsToIgnore() {    

    return $this->ignorePaths;
    
  }
  
  public function getThumbnailUrl() {

     return $this->thumbnailUrl;
     
  }
    public function getTrackAuth() {    

    return $this->trackAuth;
    
  }

}
