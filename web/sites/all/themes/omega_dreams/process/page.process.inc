<?php

/**
 * Implements hook_process_page().
 */
function omega_dreams_process_page(&$variables) {
  // Mobile menu.
  $main_menu = menu_tree_all_data('main-menu');
  $variables['mobile_menu'] = menu_tree_output($main_menu);
}
