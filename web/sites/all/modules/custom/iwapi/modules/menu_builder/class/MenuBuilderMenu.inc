<?php
/**
 * @file class/MenuBuilderMenu.inc
 * Class which faciliates the user of menus built by menu_builder.
 */

/**
 * MenuBuilderMenu is the base class used to manage and use the
 * menu styles and utilities that come with menu builder.
 */
class MenuBuilderMenu {

  protected static $style_settings = array(); // Global set of style settings, for all MenuStyle instances.

  protected $menuName;  // Name of the menu being used by this builder
  protected $admin;     // Build the menu in administrative mode? or Normally.
  
  protected $tree;      // The menu tree of links to render
  protected $style;     // Built style object that will help direct rendering.
  

  // ==============================
  // Static Methods
  // ==============================

  /**
   * Static function used to load additional menu style information.
   * This would be additional data about how to render this menu
   * with the builder settings. 
   * 
   * @param $menuName
   *  Name of the menu retrieve the data for.
   * @param $reset
   *  Boolean to indicate if we should rebuild this cache.
   *  TRUE if a rebuild is needed, FALSE otherwise.
   */
  static function styleSettings($menuName, $reset = FALSE) {
    if (empty($menuName)) {
      return array();
    }
    
    if (!isset(self::$style_settings[$menuName]) || $reset) {
      /*
       * forget caching while we are still in the building process.
      $cid = 'menu_builder:settings:' . $menuName;    
      if (!$reset && $cache = cache_get($cid, 'cache')) {
        self::$style_settings[$menuName] = $cache->data;
      }
      else {
      */
        $results = db_select('menu_builder_styles', 'mb')
          ->fields('mb')
          ->condition('menu_name', $menuName)
          ->execute();
        
        // Store the results for later usage, and to return.
        self::$style_settings[$menuName] = array();
        while(($data = $results->fetchAssoc()) !== FALSE) {
          $data['data'] = empty($data['data']) ? array(): unserialize($data['data']);
          self::$style_settings[$menuName][$data['mlid']] = $data;
        }
     /*   
        cache_set($cid, self::$style_settings[$menuName]);
      }
      */
    }
    return self::$style_settings[$menuName];
  }
  
  /**
   * Create a new instance of a MenuBuilderStyle.
   * 
   * @param $mlid
   *  The menu item ID to retrieve the style settings for.
   * @return IMenuBuilderStyle
   *  An instance of a MenuBuilderStyle or derivative object
   *  to handle the correct styling for the current settings.
   */
  protected function createStyle($mlid = 0) {
    static $settings;
    
    if (!isset($settings)) {
      $settings = self::styleSettings($this->menuName);
    }

    $styles = _menu_builder_get_menu_styles();
    if (array_key_exists($mlid, $settings)) {
      $setup = $settings[$mlid] + array('data' => array());
      $styleClass = $styles[$setup]['handler'];

      // Create a new instance of the correct style object.
      return new $styleClass($setup['data']);
    }
    return new MenuBuilderStyle(array());
  }
  
  /**
   * Generate a new menu builder instance, which will manage
   * the building and display of a menu.
   * 
   * @param $menu
   *  Either a menu tree structure like the one created by
   *  menu_tree_page_data(), or the machine name of the menu.
   */
  function __construct($menu, $filter_visible = TRUE) {
    $this->tree  = array();
    $this->admin = !($filter_visible);
    
    $menu = is_string($menu) ? menu_tree_page_data($menu) : $menu;
    if ($link = reset($menu)) {
      $this->menuName = $link['link']['menu_name'];
      
      try {
        $this->style = $this->createStyle(0);  // Style object for this menu
        $this->buildItems($menu, $this->tree); // Recursively build the menu tree.
      }
      catch (iwException $e) { $e->logToMessages(); }
      catch (Exception $e)   { watchdog_exception('MenuBuilder', $e); }
    }
  }
  
  /**
   * Get a copy of the menu style used by this builder
   * to create the actual menu items.
   * 
   * @return array
   *  Clone of the menu style object used for this item.
   *  This is the original object, so it is possible to
   *  manipulate the style.
   */
  function getStyleSettings() {
    $settings = self::styleSettings($this->menuName);
    return isset($settings[0]) ? $settings[0] : array();
  }
  
  /**
   * Set and save the menu style for this menu.
   * 
   * @param IMenuBuilderStyle $style
   *  An object that implements the IMenuBuilderStyle,
   *  which will be used render the menu.
   */
  function setStyleSettings(array $settings) {
    $settings = self::styleSettings($this->menuName);
    
    // Determine if this is a new setup or an update.
    if (isset($settings[0])) {
      $keys = array('mlid', 'menu_name');
      $setup = $settings[0];
    }
    else {
      $keys = array();
      $setup = array();
    }
    
    // Force the mlid and menu_name fields to link to this menu.
    $setup['mlid'] = 0;
    $setup['menu_name'] = $this->menuName;
    if ($style = self::createStyle($setup)) {
      $this->style = $style;
      drupal_write_record('menu_builder_styles', $setup, $keys);
      cache_clear_all('menu_builder:settings:' . $this->menuName, 'cache');
    }
  }

  /**
   * Recursively build the menu items for this menu. The resulting 
   * tree will only be populated with visible items available items.  
   * 
   * @param array $menu
   *  The current set of menu items to manage at this iteration.
   * @param array $dest
   *  Reference to the destination array, where the 
   *  built items should be added to.
   * @param array $settings
   *  The full list of styles that apply this this menu,
   *  retrieved from the MenuBuilderMenu::getStyleSettings().
   */
  protected function buildItems(array $menu, array &$dest) {
    foreach ($menu as $data) {
      if ($this->admin || ($data['link']['access'] && !$data['link']['hidden'])) {
        $mlid = $data['link']['mlid'];

        // Store the settings for this menu object.
        $dest[$mlid] = array(
          'link'  => $data['link'],
          'style' => $this->createStyle($mlid),
          'below' => array(),
        );    

        // Take care of building a child elements
        if (!empty($data['below'])) {
          $this->buildItems($data['below'], $dest[$mlid]['below']);
        }
      }
    }
  }

  /**
   * Generate the renderable array of the full menu display.
   */
  function render() {
    if (!empty($this->style)) {
      return $this->style->renderChildren($this->menuName, $this->tree);
    }
  }
  

  // =============================
  // Implement iwIConfigurable 
  // =============================
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::options()
   */
  function options() {
    $styles = _menu_builder_get_menu_styles();
    return array(
      'style' => _iwapi_extract_field($styles),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendOptions()
   */
  function appendOptions(&$element, &$state, $form) {
    $options = $this->options();
    $styles  = _menu_builder_get_menu_styles();

    $default_style = isset($state['style']) ? $state['style'] : 'basic';
    $element['style'] = array(
      '#type' => 'select',
      '#title' => t('Menu Style'),
      '#options' => $options['style'],
      '#default_value' => $default_style,
    );
    
    $element['data'] = array(
      '#type'  => 'fieldset',
      '#title' => t('@style Settings', array('@style' => $styles[$default_style]['label'])),
      '#tree'  => TRUE,
    );
    
    if (isset($this->style) && $this->style instanceof iwIConfigurable) {
      $this->style->appendOptions($element['data'], $state['data'], $form);
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateOptions()
   */
  function validateOptions($element, &$state) {
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitOptions()
   */
  function submitOptions($element, $values, &$state) {
  }
}