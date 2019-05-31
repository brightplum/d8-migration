<?php
/**
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Represents objects that can be embedded into
 * other content and
 */
abstract class iwEmbedContent {

  static $_regions;


  static function GetEmbedTypes() {
    
  }


  // ----------------------
  // Instance Methods
  // ----------------------
  
  /**
   * Create the renderable array output for
   * this content. It should be usable for
   * Drupal's render() function.
   * 
   * @return array
   *  Renderable array of content to display.
   * 
   * @see render()
   */
  abstract function view();
  
  /**
   * 
   * @staticvar int $delta
   */
  function getBlock() {
    static $delta = 0;

    $disp = $this->view();
    
    if ($disp) {
      if (empty($id)) {
        $id = '-agi-' . ++$delta; // auto-generated ID.
      }

      $block = new stdClass();
      $block->module  = 'iwapi';
      $block->delta   = 'embed_'. $id;
      $block->content = $disp;
      $block->region  = '';

      if (!isset($block->subject)) {
        $block->subject = '';
      }

      return $block;
    }
  }
  
  /**
   * Add this embeddable content into a page
   * region. This will generally appear as
   * a new block.
   * 
   * @param string $region
   *  Name of region to add the content to.
   */
  function appendToRegion($region, $id = NULL) {
    global $theme_key;
    
    static $allowed;

    if (!isset($allowed)) {
      $allowed = system_region_list($theme_key, REGIONS_VISIBLE);
    }

    // Allow the use of embeddable content.
    if (key_exists($region, $allowed) && ($block = $this->getBlock())) {
      $block->region = $region;
      self::$_regions[$block->delta] = $block;
    }
  }
}


/**
 * 
 */
class iwEmbedBlock extends iwEmbedContent {

  static function ListContent() {
    // TODO: Generate a list of available blocks.
  }

  
  // ----------------------
  // Instance variables
  // ----------------------

  public $module;
  public $delta;
  
  
  // ----------------------
  // Instance methods
  // ----------------------
  
  function __construct($module, $delta) {
    $this->module = $module;
    $this->delta  = $delta;
  }

  function fetchBlock() {
    static $info = array();
    
    // retrieve block information.
    if (!isset($info[$this->module])) {
      $info[$this->module] = module_invoke($this->module, 'block_info');
      
      if (!isset($info[$this->module])) {
        $info[$this->module] = array();
      }
    }
    
    if (!empty($info[$this->module][$this->delta])) {
      $data = (object)($info[$this->module][$this->delta] + array(
        'module' => $this->module,
        'delta' => $this->delta,
        'cache' => DRUPAL_NO_CACHE,
      ));
    
      $cid = _block_get_cache_id($data);
      if ($cid && $cache = cache_get($cid, 'cache_block')) {
        $block = $cache->data;
      }
      else {
        $block = module_invoke($this->module, 'block_view', $this->delta);

        // Valid PHP function names cannot contain hyphens.
        $delta = str_replace('-', '_', $this->delta);
        drupal_alter(array('block_view', "block_view_{$this->module}_{$delta}"), $block, $data);

        if (isset($cid)) {
          cache_set($cid, $block, 'cache_block', CACHE_TEMPORARY);
        }
      }
      
      if (!empty($block['content'])) {
        if (is_string($block['content'])) {
          $block['content'] = array('#markup' => $block['content']);
        }

        return $block;
      }
    }
  }

  function view() {
    $block = $this->fetchBlock();
    return isset($block) ? $block['content'] : NULL;
  }
  
  function getBlock() {
    $block = $this->fetchBlock();

    if (isset($block)) {
      $block = (object)$block;
      $block->module = $this->module;
      $block->delta  = $this->delta;
      $block->region = '';
     
      // Override default block title if a custom display title is present.
      if (!isset($block->subject)) {
        $block->subject = (empty($block->title) || $block->title == '<none>')
          ? FALSE : check_plain($block->title);
      }

      return $block;
    }
  }
}

