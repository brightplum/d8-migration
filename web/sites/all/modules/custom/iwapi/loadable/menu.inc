<?php
/**
 * @file includes/menu.inc
 * Includes some menu utilities, to make menu creation / editing easier.
 */


/**
 * Usually called during HOOK_menu() calls to convert items
 * which have AJAX callable equivelents using the "nojs"
 * to "ajax" convention.
 * 
 * It will also look for a parameter named $ajax in the page
 * callback, to alter the argument to TRUE if necessary. 
 * 
 * Creating alternate menu items for better handling both
 * versions yields better AJAX handling of errors and routing.
 * 
 * @param array $items
 *  An array of menu items to be added, which also should
 *  have an AJAX version created.
 * @param array $menu
 *  Reference to the menu array which is being acted on.
 *  The new menu items with their ajax alternatives will
 *  be added.
 */
function _iwapi_menu_add_ajax_items($module, array $items, array &$menu) {
  $mod_dir = drupal_get_path('module', $module);
  
  // Add the pages with AJAX equivs.
  foreach ($items as $uri => $item) {
    $menu[$uri] = $item; // Add the page.
  
    // Make the general adjustments to convert the menu item to an AJAX call.
    $ajax_uri = preg_replace('#(^|/)nojs(/|$)#i', '${1}ajax${2}', $uri, -1, $count);
    if ($count > 0) {
      $menu[$ajax_uri] = array(
        'type' => MENU_CALLBACK,
        'delivery callback' => 'ajax_deliver',
        'theme callback' => 'ajax_base_page_theme',
      ) + $item;
      
      // Ensure that all requirements for checking the function are in place.
      if (!empty($item['file'])) {
        require_once $mod_dir .'/'. $item['file'];
      }
      $item += array('page arguments' => array());
      
      // Using reflection determine if there is an argument that needs to be
      // changed or added to indicate the AJAX version should be used.
      try {
        $args = array();
        $func = new ReflectionFunction($item['page callback']);

        $values = array_values($item['page arguments']);
        foreach ($func->getParameters() as $param) {
          if ($param->name == 'ajax') {
            $args[] = TRUE;
          }
          else {
            $pos = $param->getPosition();
            $args[] = isset($values[$pos]) ? $values[$pos] : $param->getDefaultValue();
          }
        }
        
        // Add our page arguments, with any parameters named "$AJAX" to be set as
        // TRUE. This assumes that this param is used to indicate AJAX processing.
        $menu[$ajax_uri]['page arguments'] = $args + $item['page arguments'];
      }
      catch (Exception $e) {
        watchdog_exception('iwapi_menu', $e);
      }
    }
  }
}
