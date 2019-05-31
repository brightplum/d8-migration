<?php
/**
 * @file menu_builder.admin.inc
 * Functions used to generate configuration forms and helper functions.
 */

/**
 * Menu callback which contains the form API structure of the menu
 * builder form. This form will be used to structure menu items.
 */
 function menu_builder_build_form($form, &$form_state, $menu) {
  global $menu_admin;
   
  $data = _menu_build_tree($menu['menu_name']);
  // We indicate that a menu administrator is running the menu access check.
  $menu_admin = TRUE;
  menu_tree_check_access($data['tree'], $data['node_links']);
  $menu_admin = FALSE;
  
  $builder = new MenuBuilderMenu($data['tree'], FALSE);
  
  // Generate a fixed right-side block containing menu building tools.
  $columns = range(1, 10);
  $form['toolbar'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    
    'columns' => array(
      '#type' => 'select',
      '#title' => t('Number of columns'),
      '#options' => array_combine($columns, $columns),
      '#description' => t('Note that changing this settings may cause menu items to shuffle.'),
    ),
  );
  
  
  
  return $form;
 }