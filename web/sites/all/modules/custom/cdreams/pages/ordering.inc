<?php
/**
 * @file
 * Contains the admin for re-arranging HP content.
 */


/**
 * Generate the form to build the homepage
 * ordering administration form.
 */
function cdreams_manage_news_ordering_form($form, &$form_state) {
  if (!isset($form_state['news_items'])) {
    $form_state['news_items'] = _cdreams_find_orderable_news();
    
    $promoted = 0;
    $other    = 4;
    foreach($form_state['news_items'] as &$item) {
      $item['slot'] = ((bool)$item['promoted']) ? $promoted++ : $other++;
      if ($item['slot'] > 15) {
        $item['slot'] = '';
      }
    }
    unset($item);
  }
  $items = $form_state['news_items'];
  
  // -----------------------------------------------
  // Prepare the form by getting the needed CSS and JS functionality in place
  $path = drupal_get_path('module', 'cdreams');
  $form['#attached'] = array(
    'css' => array(
      $path .'/css/ordering-articles.css',
      $path .'/css/ordering-news.css',
      drupal_get_path('module', 'iwapi') .'/theme/css/content-builder.css',
    ),
    'js' => array(
      $path .'/js/ordering.js',
      $path .'/js/landing-page.js'
    ),
    'library' => array(
      array('system', 'jquery.form'),
      array('iwapi', 'iwapi.ajax'),
      array('iwapi', 'iwapi.builders'),
      array('iwapi', 'iwapi.docking'),
      array('system', 'ui.droppable'),
      array('system', 'ui.draggable'),
    ),
  );

  // ---------------------------------
  // Content contains and functions
  $form['canvas'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'content-canvas',
      'class' => array('grid-layout', 'clearfix'),
    ),
  );
 
  $form['unused_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other news articles'),
      
    'content' => array(
      '#markup' => '<input class="container-value" type="hidden" value="" />'.
        '<div class="content-container clearfix"></div>',
    ),
  );
  
  // --------------------------
  // Prep the content containers.
  $form['containers'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'container-data'),
  );

  // Create 16 slots for news articles.
  $slots = range(0, 15);
  foreach ($slots as $slot) {
    $form['containers'][$slot] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'slot-'. $slot,
        'class' => array('content-slot', 'grid-tile'),
      ),
      'slot' => array(
        '#type' => 'hidden',
        '#value' => $slot,
        '#attributes' => array('class' => array('container-value')),
      ),
      'container' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('content-container', 'clearfix')),
      ),
    );
  }
  
  // ----------------------------------
  // Prepare the news articles for placement.
  $form['fields'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'id' => 'field-data',
      'class' => array('news-items'),
    ),
  );
  
  // The article date field was captured GMT.
  $tz = date_default_timezone_get();
  date_default_timezone_set('UTC');

  foreach ($items as $nid => $data) {
    $article_date = date('Y-m-d', $data['date'] + 1);

    $image = field_get_items('node', $data['node'], 'field_image');
    $image = empty($image) ? array() : field_view_value('node', $data['node'], 'field_image', $image[0]);
    $image['#image_style'] = 'thumbnail';
    
    $title = field_get_items('node', $data['node'], 'field_hp_title');
    $title = empty($title) ? $data['node']->title : $title[0]['value'];

    $form['fields'][$nid] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'news-item-'. $nid,
        'class' => array('draggable-field', 'news-item'),
      ),
      
      'item-handle' => array(
        '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
        '#suffix' => '</div>',
        '#markup' => check_plain($title),
      ),
      'parent' => array(
        '#type' => 'select',
        '#empty_option' => '- not used -',
        '#empty_value' => '',
        '#options' => $slots,
        '#default_value' => $data['slot'],
        '#attributes' => array('class' => array('field-parent')),
      ),
      'date' => array(
        '#type' => 'hidden',
        '#value' => $article_date,
        '#attributes' => array('class' => array('date-value')),
      ),
      'actions' => array(
        '#theme' => 'links',
        '#links' => array(
          'edit' => array(
            'title' => '<i class="fa fa-pencil"></i>',
            'html' => TRUE,
            'href' => "node/$nid/ajax-edit",
            'attributes' => array('class' => array('use-ajax')),
          ),
        ),
        '#attributes' => array('class' => array('field-actions', 'inline')),
      ),
      'summary' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-summary')),
        
        'thumbnail' => $image,
        'display_date' => array(
          '#markup' => '<div>'. $article_date .'</div>',
        ),
      ),
    );
  }
  
  // Put the default timezone information back.
  date_default_timezone_set($tz);

  $form['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array(
      'class' => array('iwapi-dockable', 'dock-bottom-attach'),
    ),

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save changes'),
      '#submit' => array('cdreams_manage_promoted_order_submit'),
    ),
  );

  return $form;
}

/**
 * Submit form callback which saves the ordering changes
 * for ordering HP news articles.
 */
function cdreams_manage_promoted_order_submit($form, &$form_state) {
  $values = $form_state['values']['fields'];
  
  try {
    // Remove promoted from all entities.
    db_update('cdreams_ordering')
      ->fields(array('promoted' => 0))
      ->condition('promoted', 0, '>')
      ->execute();

    foreach ($values as $nid => $data) {
      if ($data['parent'] != '') {
        $slot = intval($data['parent']);

        db_update('cdreams_ordering')
          ->fields(array(
            'weight' => 20 - $slot,
            'promoted' => ($slot < 4) ? 1 : 0,
          ))
          ->condition('entity_type', 'node')
          ->condition('entity_id', $nid)
          ->execute();
      }
    }
 
    drupal_set_message(t('Home page ordering has been saved.'));
  }
  catch (Exception $e) {
    watchdog_exception('cdreams', $e);
    drupal_set_message(t('Unable to save home page sorting.'), 'error');  
  }
}


// =================================
// Order other articles
// =================================

/**
 * Form builder which generates the form for
 * ordering article content. This orders content
 * that is not headline news.
 */
function cdreams_manage_hp_articles_form($form, &$form_state, $type) {
  $types = array(
    'views_article' => 20,
    'newswire' => 5,
  );
  
  if (!isset($types[$type])) {
    $form['message']['#markup'] = t(
      'Sorting is not supported for @type',
      array('@type' => $type)
    ); 
    return $form;
  }
   
  $max_slots = $types[$type];
  if (!isset($form_state['article_items'])) {
    $form_state['max_slots']     = $max_slots;
    $form_state['article_items'] = _cdreams_find_orderable_articles($type, $max_slots);
    
    $pos = 0;
    foreach($form_state['article_items'] as &$item) {
      $item['slot'] = $pos++;
      
      if ($item['slot'] >= $max_slots) {
        $item['slot'] = '';
      }
    }
    unset($item);
  }
  $items = $form_state['article_items'];
  
  // -----------------------------------------------
  // Prepare the form by getting the needed CSS and JS functionality in place
  $path = drupal_get_path('module', 'cdreams');
  $form['#attached'] = array(
    'css' => array(
      $path .'/css/ordering-articles.css',
      drupal_get_path('module', 'iwapi') .'/theme/css/content-builder.css',
    ),
    'js' => array(
      $path .'/js/ordering-articles.js',
      $path .'/js/landing-page.js'
    ),
    'library' => array(
      array('system', 'jquery.form'),
      array('iwapi', 'iwapi.ajax'),
      array('iwapi', 'iwapi.builders'),
      array('iwapi', 'iwapi.docking'),
      array('system', 'ui.droppable'),
      array('system', 'ui.draggable'),
    ),
  );
  
  // ---------------------------------
  // Content contains and functions
  $form['canvas'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'content-canvas',
      'class' => array('grid-layout', 'clearfix'),
    ),
  );
 
  $form['unused_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other articles'),
      
    'content' => array(
      '#markup' => '<input class="container-value" type="hidden" value="" />'.
        '<div class="content-container clearfix"></div>',
    ),
  );
  
  // --------------------------
  // Prep the content containers.
  $form['containers'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'container-data'),
  );

  // Create 16 slots for news articles.
  $slots = range(0, $max_slots - 1);
  foreach ($slots as $slot) {
    $form['containers'][$slot] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'slot-'. $slot,
        'class' => array('content-slot'),
      ),
      'slot' => array(
        '#type' => 'hidden',
        '#value' => $slot,
        '#attributes' => array('class' => array('container-value')),
      ),
      'container' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('content-container', 'clearfix')),
      ),
    );
  }
  
  // ----------------------------------
  // Prepare the news articles for placement.
  $form['fields'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'id' => 'field-data',
      'class' => array('news-items'),
    ),
  );
  
  // The article date field was captured GMT.
  $tz = date_default_timezone_get();
  date_default_timezone_set('UTC');

  foreach ($items as $nid => $data) {
    $image        = array();
    $article_date = date('Y-m-d', $data['date']);

    if ($type === 'views_article') {
      $profiles = field_get_items('node', $data['node'], 'field_author_profile');

      if ($profiles && ($profile = reset($profiles))) {
        $profile = iwapi_entity_object_load($profile['target_id'], 'cdreams_profile');
        $img = field_get_items('cdreams_profile', $profile, 'field_profile_img');

        if (!empty($img)) {
          $image = field_view_value('cdreams_profile', $profile, 'field_profile_img', $img[0]);
          $image['#image_style'] = 'cd_bio_small';
        }
      }
      
      if (empty($image)) {
        $image = array(
          '#markup' => '<div style="background: #fff; width: 65px; text-align: center;">'.
            '<i class="fa fa-user" style="font-size:60px;"></i></div>'
        );
      }
    }
    else {
      $image = field_get_items('node', $data['node'], 'field_image');
      $image = empty($image) ? array() : field_view_value('node', $data['node'], 'field_image', $image[0]);
      $image['#image_style'] = 'thumbnail';
    }
    
    $title = field_get_items('node', $data['node'], 'field_hp_title');
    $title = empty($title) ? $data['node']->title : $title[0]['value'];

    $form['fields'][$nid] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'news-item-'. $nid,
        'class' => array('draggable-field', 'news-item'),
      ),
      
      'item-handle' => array(
        '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
        '#suffix' => '</div>',
        '#markup' => check_plain($title),
      ),
      'parent' => array(
        '#type' => 'select',
        '#empty_option' => '- not used -',
        '#empty_value' => '',
        '#options' => $slots,
        '#default_value' => $data['slot'],
        '#attributes' => array('class' => array('field-parent')),
      ),
      'date' => array(
        '#type' => 'hidden',
        '#value' => $article_date,
        '#attributes' => array('class' => array('date-value')),
      ),
      'actions' => array(
        '#theme' => 'links',
        '#links' => array(
          'edit' => array(
            'title' => '<i class="fa fa-pencil"></i>',
            'html' => TRUE,
            'href' => "node/$nid/ajax-edit",
            'attributes' => array('class' => array('use-ajax')),
          ),
        ),
        '#attributes' => array('class' => array('field-actions', 'inline')),
      ),
      'summary' => array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-summary')),
        
        'thumbnail' => $image,
        'display_date' => array(
          '#markup' => '<div>'. $article_date .'</div>',
        ),
      ),
    );
  }

  date_default_timezone_set($tz);
  
  $form['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array(
      'class' => array('iwapi-dockable', 'dock-bottom-attach'),
    ),

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save changes'),
      '#submit' => array('cdreams_manage_hp_order_submit'),
    ),
  );
 
  return $form;
}

/**
 * Submit form callback which saves the ordering changes
 * for ordering HP news articles.
 */
function cdreams_manage_hp_order_submit($form, &$form_state) {
  $values = $form_state['values']['fields'];
  
  try {
    foreach ($values as $nid => $data) {
      if ($data['parent'] != '') {
        $slot = intval($data['parent']);

        db_update('cdreams_ordering')
          ->fields(array(
            'weight' => $form_state['max_slots'] - $slot,
            'promoted' => 0,
          ))
          ->condition('entity_type', 'node')
          ->condition('entity_id', $nid)
          ->execute();
      }
    }
 
    drupal_set_message(t('Home page ordering has been saved.'));
  }
  catch (Exception $e) {
    watchdog_exception('cdreams', $e);
    drupal_set_message(t('Unable to save home page sorting.'), 'error');  
  }
}


// =================================
// Utility and helpers
// =================================

/**
 * Find items that are used in ordering the news
 * content. These will be collected in the order
 * they appear on the homepage.
 */
function _cdreams_find_orderable_news() {
  $items = array();

  $query = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('n.type', 'headline')
    ->condition('n.status', NODE_PUBLISHED);
  
  $query->join('cdreams_ordering', 'o', 'o.entity_type=:type AND o.entity_id=n.nid', array(':type' => 'node'));
  $rs = $query->fields('o', array('promoted', 'date'))
    ->condition('o.promoted', 0, '<>')
    ->orderBy('o.weight', 'DESC')
    ->range(0, 4)->execute();

  while ($row = $rs->fetchAssoc()) {
    $items[$row['nid']] = $row;
  }

  // Add the loaded nodes to get referenced.
  foreach (node_load_multiple(array_keys($items)) as $nid => $node) {
    $items[$nid]['node'] = $node;
  }
  
  // Include our promoted and additional articles with normal ordering.
  return $items + _cdreams_find_orderable_articles('headline', 12);
}

/**
 * Retrieve unpromoted items in the date / weight / creation
 * ordering. Will only UNPROMOTED items.
 * 
 * @param string $type
 *  Machine name of the node type to use.
 * @param int $number
 *  Number of articles to find in sorted order.
 * @return array
 *  An array of node information keyed by {node}.nid
 */
function _cdreams_find_orderable_articles($type, $number) {
  $items = array();

  $query = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('n.type', $type)
    ->condition('n.status', NODE_PUBLISHED);
  
  $query->join('cdreams_ordering', 'o', 'o.entity_type=:type AND o.entity_id=n.nid', array(':type' => 'node'));
  $query->fields('o', array('promoted', 'date'))
    ->condition('o.promoted', 0)
    ->orderBy('o.date', 'DESC')
    ->orderBy('o.weight', 'DESC')
    ->orderBy('o.created', 'DESC');

  $finalDay = clone $query;
  $rs = $query->range(0, $number)->execute();

  $last_day = REQUEST_TIME;
  while ($row = $rs->fetchAssoc()) {
    $items[$row['nid']] = $row;
    
    if ($last_day > $row['date']) {
      $last_day = $row['date'];
    }
  }

  $rs = $finalDay->condition('o.date', $last_day)
    ->execute();
  
  while ($row = $rs->fetchAssoc()) {
    $items[$row['nid']] = $row;
  }
  
  // Add the loaded nodes to get referenced.
  foreach (node_load_multiple(array_keys($items)) as $nid => $node) {
    $items[$nid]['node'] = $node;
  }

  return $items;
}
