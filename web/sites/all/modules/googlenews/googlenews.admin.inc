<?php

/**
 * @file
 * Admin functionality for the GoogleNews module.
 */

/**
 * Form builder; administration settings.
 */
function googlenews_admin_settings() {
  global $base_url;

  $form['help'] = array(
    '#markup' => '<p>' . t('Settings for controlling the <a href="@news-sitemap">Google News sitemap file</a>.', array('@news-sitemap' => url('googlenews.xml'))) . "</p>\n",
  );
  $form['count'] = array(
    '#markup' => '<p>' . t('There are currently @count node(s) suitable for output.', array('@count' => count(googlenews_list_nodes()))) . "</p>\n",
  );

  $form['googlenews_publication_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Publication name'),
    '#default_value' => variable_get('googlenews_publication_name', ''),
    '#description' => t("Leave blank to use the site's name instead: :site_name", array(':site_name' => variable_get('site_name', 'Drupal'))),
  );

  // Optional Views integration.
  if (module_exists('views')) {
    $form['googlenews_source'] = array(
      '#type' => 'radios',
      '#title' => t('Source to use'),
      '#options' => array(
        'manual' => t('Content types selected below'),
      ),
      '#required' => TRUE,
      '#default_value' => variable_get('googlenews_source', 'manual'),
      // '#description' => t('For simple sites using the content types option might be sufficient.'),
    );

    $form['googlenews_source']['#options']['views'] = t('Views; a better option for complex sites');

    $options = array();
    // List all views which use "node" as the base table.
    foreach (views_get_all_views() as $view_id => $view) {
      if ($view->base_table == 'node') {
        foreach ($view->display as $display_id => $display_value) {
          $options[$view->human_name][$view_id . '|' . $display_id] = $display_value->display_title;
        }
      }
    }
    $form['googlenews_view'] = array(
      '#type' => 'select',
      '#title' => t('View to use'),
      '#options' => $options,
      '#default_value' => variable_get('googlenews_view', 'frontpage|default'),
      '#description' => t('This is used to identify which nodes will be displayed, it does not affect the actual content display.'),
      '#states' => array(
        'visible' => array(
          'input[name="googlenews_source"]' => array('value' => 'views'),
        ),
      ),
    );
  }

  $node_types = node_type_get_names();
  $form['googlenews_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select the content types to include'),
    '#default_value' => variable_get('googlenews_node_types', array_keys($node_types)),
    '#options' => $node_types,
  );

  // If Views is available, only show the "content types" selection if that
  // option is selected.
  if (module_exists('views')) {
    $form['googlenews_node_types']['#states'] = array(
      'visible' => array(
        'input[name="googlenews_source"]' => array('value' => 'manual'),
      ),
    );
  }

  $form['array_filter'] = array(
    '#type' => 'value',
    '#value' => TRUE,
  );

  $form['googlenews_cache_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Cache timeout (minutes)'),
    '#default_value' => variable_get('googlenews_cache_timeout', 15),
    '#description' => t('The number of minutes that the sitemap file will be cached for before it is regenerated.'),
  );

  $form['googlenews_content_hours'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum content age (hours)'),
    '#default_value' => intval(variable_get('googlenews_content_hours', 48)),
    '#description' => t('All content (nodes) created within this number of hours will be included in the sitemap file. It is recommended to leave this at the default of 48 hours.'),
  );
  if (module_exists('views')) {
    $form['googlenews_content_hours']['#states'] = array(
      'visible' => array(
        'input[name="googlenews_source"]' => array('value' => 'manual'),
      ),
    );
  }

  $form['googlenews_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Default base URL'),
    '#default_value' => variable_get('googlenews_base_url', ''),
    '#description' => t('This is the default base URL used in the googlenews sitemap (e.g. http://example.com). Leave blank to automatically use the default value: %base', array('%base' => $base_url)),
  );

  // Optional tags.
  $form['tags'] = array(
    '#type' => 'fieldset',
    '#title' => t('Optional tags'),
    '#description' => t('Enter values to use for any of the optional GoogleNews tags. Tokens are supported, see below for a list of available tokens that may be used.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  foreach (googlenews_tag_list() as $tag_name => $tag) {
    $var_name = 'googlenews_tag_' . $tag_name;
    $form['tags'][$var_name] = array(
      '#type' => 'textfield',
      '#title' => $tag['title'],
      '#default_value' => variable_get($var_name, ''),
      '#description' => $tag['description'],
    );
    // If an 'options' value was present, display the field as a selector.
    if (!empty($tag['options'])) {
      $form['tags'][$var_name]['#type'] = 'select';
      $form['tags'][$var_name]['#options'] = $tag['options'];
      // Some selectors allow multiple values so display them as checkboxes.
      if (!empty($tag['multiple'])) {
        $form['tags'][$var_name]['#type'] = 'checkboxes';
        $form['tags'][$var_name]['#multiple'] = TRUE;
        $form['tags'][$var_name]['#default_value'] = variable_get($var_name, array());
      }
    }
  }

  // Display the list of available placeholders if token module is installed.
  if (module_exists('token')) {
    $form['tags']['token_help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('node'),
      '#dialog' => TRUE,
    );
  }

  $form['#submit'][] = 'googlenews_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * FormAPI submission callback for googlenews_admin_settings.
 */
function googlenews_admin_settings_submit(&$form, &$form_state) {
  cache_clear_all('googlenews', 'cache');
  cache_clear_all(url('googlenews.xml', array('absolute' => TRUE)), 'cache_page');
  drupal_set_message(t('The cached sitemap has been cleared, all settings changes will take immediate effect.'));
}
