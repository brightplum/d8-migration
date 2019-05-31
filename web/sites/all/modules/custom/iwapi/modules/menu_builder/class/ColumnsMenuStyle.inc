<?php
/**
 * 
 */

/**
 * 
 */
class ColumnsMenuStyle extends MenuBuilderStyle {
  protected $columns = array();

  /**
   * Create a new ColumnsMenuStyle, to render menu
   * items into a columnized format.
   * 
   * @param array $settings
   *  Settings that determine how the columns are displayed
   *  and how to sort out the different items.
   */
  function __construct($menuName, array $data) {
    parent::__construct($menuName, $data);
    
    // Array to indicate how the menu items should be place in columns.
    $this->columns = empty($data['columns']) ? array() : $data['columns'];
  }
  
  /**
   * Render children all the menu items in their respective columns.
   * 
   * @param array $children
   *  An array of menu items to render
   */
  function renderChildren(array $children) {
    $path = drupal_get_path('module', 'menu_builder'); ;
    
    // Gather all the items into columns.
    $output = array();
    foreach ($this->columns as $key => $items) {
      $colname = is_numeric($key) ? 'column-'. intval($key) : $key;

      // Find all the menu items for the current column and group them.
      $mlids = array_combine($items, $items);
      $output[$colname] = array_intersect_key($children, $mlids);
      $children         = array_diff_key($children, $mlids);
    }
    $output['misc'] = $children;  // Add any leftover items into a miscellaneous column
    
    // Check each of the columns for elements, and attach the wrapper styles.
    foreach ($output as $colname => $column) {
      if (!empty($column)) {
        $output[$colname] = parent::renderChildren($column);
        $output[$colname]['#theme_wrappers'][] = 'menu_builder_column';
        $output[$colname]['#attributes']['class'][] = $colname;
      }
      else {
        unset($output[$colname]);
      }
    }
    
    // Set the first and last class CSS for the first and last column.
    $keys = array_keys($output);
    array_unshift($output[reset($keys)]['#attributes']['class'], 'first');
    array_unshift($output[end($keys)]['#attributes']['class'],   'last');
    
    $classes = array('menu-columns', 'column-container-'. count($output), 'clearfix');
    
    // Apply the styling and the wrapping elements
    $output['#sorted']         = TRUE;
    $output['#theme_wrappers'] = array('container');
    $output['#attached']       = array('css' => array($path .'/css/menu-columns.css'));
    $output['#attributes']     = array('class' => $classes);
 
    return $output;
  }
}