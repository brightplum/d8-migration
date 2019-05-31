<?php
/**
 * @file class/MenuBuilderStyle.inc
 * Definitions for the base menu builder style interface and classes.
 */

/**
 * The menu builder style interface which should be
 * implemented by all styles. The styles are not used
 * to render the current menu item and also to format
 * how child items will be rendered.
 * 
 * The class covers:
 * - Rendering menu items and child items
 * - Generating form elements for configuration.
 * - Generating tool bar elements for edit actions.
 */
interface IMenuBuilderStyle {

  /**
   * Render a single menu item and the child items.
   * 
   * @param array $link
   *  The link properties and attributes.
   * @param array $children
   *  Any children menu items that will be rendered
   *  below this menu item.
   * @param array $class
   *  Any CSS classes to use when rendering.
   * @return array
   *  Return a renderable array of the item contents.
   */
  function render(array $link, array $children, array $class = array());
  
  /**
   * Render an array of menu items as the children,
   * of either a menu or menu item.
   * 
   * @param $menuName
   *  Name of the menu being rendered.
   * @param array $children
   *  An array of menu items and wrap them as
   *  child items.
   * @return array
   *  Return a renderable array of the menu contents.
   */
  function renderChildren($menuName, array $children);
  
  /**
   * 
   * @param unknown_type $element
   * @param unknown_type $state
   * @param unknown_type $form
   * @param unknown_type $form_state
   */
  function toolbarElements(&$element, &$state, $form, &$form_state);
}

/**
 * 
 *
 */
class MenuBuilderStyle implements IMenuBuilderStyle {
  
  function __construct(array $data) {
  }
  
  /**
   * (non-PHPdoc)
   * @see IMenuBuilderStyle::render()
   */
  function render(array $link, array $children, array $class = array()) {
    $menuName = $link['menu_name'];
    
    // Append the correct CSS class to the link.
    $class[] = !empty($children) ? 'expanded' : ($link['has_children'] ? 'collapsed' : 'leaf');
    
    // Set a class if the link is in the active trail.
    if ($link['in_active_trail']) {
      $class[] = 'active-trail';
      $link['localized_options']['attributes']['class'][] = 'active-trail';
    }
    
    // Normally, l() compares the href of every link with $_GET['q'] and sets
    // the active class accordingly. But local tasks do not appear in menu
    // trees, so if the current path is a local task, and this link is its
    // tab root, then we have to set the class manually.
    $router_item = menu_get_item();
    if ($link['href'] == $router_item['tab_root_href'] && $link['href'] != $_GET['q']) {
      $link['localized_options']['attributes']['class'][] = 'active';
    }
    
    // Allow menu-specific theme overrides.
    return array(
      '#theme' => 'menu_link__' . strtr($menuName, '-', '_'),
      '#title' => $link['title'],
      '#href'  => $link['href'],
      '#original_link' => $link,
      '#below' => empty($children) ? array() : $this->renderChildren($menuName, $children),
      '#attributes' => array('class' => $class),
      '#localized_options' => !empty($link['localized_options']) ? $link['localized_options'] : array(),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see IMenuBuilderStyle::renderChildren()
   */
  function renderChildren($menuName, array $children) {
    $output = array();
    
    // Render the child elements
    if (!empty($children)) {
      $output['#sorted'] = TRUE;
      $output['#theme_wrappers'][] = 'menu_tree__' . strtr($menuName, '-', '_');
      
      // We want to handle the last item a little bit
      // differently so we are saving it for later.
      end($children);
      $ekey = key($children);
      $last = array_pop($children);
      
      // Start with the class "first", it will get reset in the loop. If
      // only one item in the array, this array won't get reset, and the
      // 1 item will have classes "first" and "last" applied to it.
      $class = array('first');
      foreach ($children as $key => $item) {
        $output[$key] = $item['style']->render($item['link'], $item['below'], $class);
        $class = array();
      }
      
      // Handle the last element, ensuring that we tag the class "last".
      $class[] = 'last';
      $output[$ekey] = $last['style']->render($last['link'], $last['below'], $class);
    }    
    return $output;
  }
  
  /**
   * (non-PHPdoc)
   * @see IMenuBuilderStyle::toolbarElements()
   */
  function toolbarElements(&$element, &$state, $form, &$form_state) {
    $path = drupal_get_path('module', 'menu_builder');
    $element['#attached'] = array(
      'js' => array($path .'/js/admin-columns.js'),
      'css' => array($path .'/css/admin-columns.css'),
    );
    
    $element['add_column'] = array(
      '#type' => 'submit',
      '#value' => 'Add Column',
      '#attributes' => array(
        'class' => array(
          'menu-builder-action',
          'columns-add-column',
        ),
      ),
    );
  }
}