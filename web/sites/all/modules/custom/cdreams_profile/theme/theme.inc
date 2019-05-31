<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


/**
 * The base theme function for myvice_product_add_list.
 * It will render the links and descriptions for all
 * product types available.
 * 
 * @param $vars
 *  Values used to render the list items.
 */
function theme_cdreams_profile_add_list($vars) {
  $content = $vars['content'];
  
  $list['attributes'] = $content['#attributes'];
  $list['attriubtes']['class'][] = 'admin-list';
  
  foreach ($content['#items'] as $item) {
    $description = empty($item['description']) ? '' : filter_xss_admin($item['description']);
    
    $list['items'][] = '<span class="label">'. l($item['title'], $item['href']) .'</span>'
      . '<div class="description">'. $description .'</div>';
  }
  
  return theme('item_list', $list);
}