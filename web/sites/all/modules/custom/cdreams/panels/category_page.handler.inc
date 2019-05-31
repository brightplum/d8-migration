<?php


$handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'term_view_panel_context';
  $handler->task = 'term_view';
  $handler->subtask = '';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'Menu Category',
    'no_blocks' => 1,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => '',
    'css_id' => '',
    'css' => '',
    'contexts' => array(),
    'relationships' => array(),
    'access' => array(
      'plugins' => array(
        0 => array(
          'name' => 'term_vocabulary',
          'settings' => array(
            'machine_name' => array(
              'menu_categories' => 'menu_categories',
            ),
          ),
          'context' => 'argument_term_1',
          'not' => FALSE,
        ),
      ),
      'logic' => 'or',
    ),
  );
  $display = new panels_display();
  $display->layout = 'mondrian';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'main' => NULL,
      'secondary' => NULL,
      'tertiary-first' => NULL,
      'tertiary-second' => NULL,
      'tertiary-third' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '%term:name';
  $display->uuid = '4f570b58-b556-49a0-8e75-9c12a9b36efb';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-a78af559-7496-446e-9da7-2980dd96b9cc';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'categorized_news';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '1',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'categorized_news_primary',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = 'a78af559-7496-446e-9da7-2980dd96b9cc';
    $display->content['new-a78af559-7496-446e-9da7-2980dd96b9cc'] = $pane;
    $display->panels['main'][0] = 'new-a78af559-7496-446e-9da7-2980dd96b9cc';
    $pane = new stdClass();
    $pane->pid = 'new-5eda340d-a22c-45c1-9b12-d9d1fe521d9e';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'categorized_news';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '0',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'categorized_news_secondary',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '5eda340d-a22c-45c1-9b12-d9d1fe521d9e';
    $display->content['new-5eda340d-a22c-45c1-9b12-d9d1fe521d9e'] = $pane;
    $display->panels['main'][1] = 'new-5eda340d-a22c-45c1-9b12-d9d1fe521d9e';
    $pane = new stdClass();
    $pane->pid = 'new-4979aae3-ef1c-4c5d-af99-73ee0e4f00de';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'categorized_news';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '2',
      'pager_id' => '0',
      'offset' => '2',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'categorized_news_tertiary',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '4979aae3-ef1c-4c5d-af99-73ee0e4f00de';
    $display->content['new-4979aae3-ef1c-4c5d-af99-73ee0e4f00de'] = $pane;
    $display->panels['main'][2] = 'new-4979aae3-ef1c-4c5d-af99-73ee0e4f00de';
    $pane = new stdClass();
    $pane->pid = 'new-d6243413-3f1f-4be1-be5a-cad36573e89e';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'categorized_news';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '12',
      'pager_id' => '0',
      'offset' => '4',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'categorized_news_remaining',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 3;
    $pane->locks = array();
    $pane->uuid = 'd6243413-3f1f-4be1-be5a-cad36573e89e';
    $display->content['new-d6243413-3f1f-4be1-be5a-cad36573e89e'] = $pane;
    $display->panels['main'][3] = 'new-d6243413-3f1f-4be1-be5a-cad36573e89e';
    $pane = new stdClass();
    $pane->pid = 'new-dc92f64c-35b9-4f19-bf41-f51e7080ff0f';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'further';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '1',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'block_1',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => 'Further...',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 4;
    $pane->locks = array();
    $pane->uuid = 'dc92f64c-35b9-4f19-bf41-f51e7080ff0f';
    $display->content['new-dc92f64c-35b9-4f19-bf41-f51e7080ff0f'] = $pane;
    $display->panels['main'][4] = 'new-dc92f64c-35b9-4f19-bf41-f51e7080ff0f';
    $pane = new stdClass();
    $pane->pid = 'new-d214e65c-784b-43c3-aa6e-c1dd77a99094';
    $pane->panel = 'main';
    $pane->type = 'views';
    $pane->subtype = 'newswire';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '3',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'block_1',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => 'Progressive Newswire',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 5;
    $pane->locks = array();
    $pane->uuid = 'd214e65c-784b-43c3-aa6e-c1dd77a99094';
    $display->content['new-d214e65c-784b-43c3-aa6e-c1dd77a99094'] = $pane;
    $display->panels['main'][5] = 'new-d214e65c-784b-43c3-aa6e-c1dd77a99094';
    $pane = new stdClass();
    $pane->pid = 'new-68ca625b-55a5-4c30-82d1-8c4ec776915f';
    $pane->panel = 'secondary';
    $pane->type = 'block';
    $pane->subtype = 'block-1';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => 'Email Sign-Up',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '68ca625b-55a5-4c30-82d1-8c4ec776915f';
    $display->content['new-68ca625b-55a5-4c30-82d1-8c4ec776915f'] = $pane;
    $display->panels['secondary'][0] = 'new-68ca625b-55a5-4c30-82d1-8c4ec776915f';
    $pane = new stdClass();
    $pane->pid = 'new-4bbe2381-7a0c-41d5-bb44-7a19e7919862';
    $pane->panel = 'secondary';
    $pane->type = 'block';
    $pane->subtype = 'block-2';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => 'Follow Us',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '4bbe2381-7a0c-41d5-bb44-7a19e7919862';
    $display->content['new-4bbe2381-7a0c-41d5-bb44-7a19e7919862'] = $pane;
    $display->panels['secondary'][1] = 'new-4bbe2381-7a0c-41d5-bb44-7a19e7919862';
    $pane = new stdClass();
    $pane->pid = 'new-b56028b7-6e20-409c-9c55-7d6a0da334b5';
    $pane->panel = 'secondary';
    $pane->type = 'views';
    $pane->subtype = 'views_articles';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '0',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'attachment_1',
      'context' => array(
        0 => 'argument_term_1.tid',
      ),
      'override_title' => 1,
      'override_title_text' => 'Views',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = 'b56028b7-6e20-409c-9c55-7d6a0da334b5';
    $display->content['new-b56028b7-6e20-409c-9c55-7d6a0da334b5'] = $pane;
    $display->panels['secondary'][2] = 'new-b56028b7-6e20-409c-9c55-7d6a0da334b5';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = '0';
  $handler->conf['display'] = $display;
  $handlers['term_view_panel_context'] = $handler;