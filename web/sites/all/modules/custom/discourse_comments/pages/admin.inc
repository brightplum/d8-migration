<?php
/**
 * @file
 * Contains the settings and configuration forms for managing Discourse comments.
 */


// =========================
// Content settings
// =========================

/**
 * Create the form which allows us to find and
 * set information on which entities and bundles
 * Discourse will use / find.
 */
function discourse_comments_entities_form($form, &$form_state) {
  $entities = variable_get(DISCOURSE_COMMENTS_VAR_ENTITIES, array());
  $protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";
  $server = variable_get(DISCOURSE_COMMENTS_VAR_SERVER, FALSE);
  if (!empty($server['host'])) {
    try {
      $req = new DiscourseRequest($protocol . $server['host'], $server['api_key'], $server['api_username']);
      $categories = _iwapi_extract_field($req->fetchCategories(), 'name');
    }
    catch(Exception $e) {
      watchdog_exception('discourse_comments', $e);
    }
  }

  if (empty($categories)) {
    $categories = array('1' => 'Uncategorized');
  }

  $types = entity_get_info();
  unset($types['taxonomy_term'], $types['taxonomy_vocabulary']);
  unset($types['comment'], $types['user'], $types['wysiwyg_profile']);
  unset($types['redirect'], $types['file']);

  $form[DISCOURSE_COMMENTS_VAR_ENTITIES] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );

  foreach ($types as $name => $info) {
    $form[DISCOURSE_COMMENTS_VAR_ENTITIES][$name] = array(
      '#type' => 'fieldset',
      '#title' => t('@label Type', array('@label' => $info['label'])),
      '#description' => t(
        'Select a category to insert content of this type into. ' .
        'Types which do not have a category will be excluded.'
      ),
    );

    foreach ($info['bundles'] as $bundle => $bundle_info) {
      $form[DISCOURSE_COMMENTS_VAR_ENTITIES][$name][$bundle]['category'] = array(
        '#type' => 'select',
        '#title' => t('%bundle category', array('%bundle' => $bundle_info['label'])),
        '#options' => $categories,
        '#empty_option' => t('- Unused -'),
        '#default_value' => empty($entities[$name][$bundle]['category'])
          ? NULL : $entities[$name][$bundle]['category'],
      );
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );

  return $form;
}

/**
 * Form submit callback
 */
function discourse_comments_entities_form_submit($form, &$form_state) {
  $entities = ($form_state['values'][DISCOURSE_COMMENTS_VAR_ENTITIES]);

  foreach ($entities as $type => $bundles) {
    foreach ($bundles as $bundle => $val) {
      if (strlen($val['category']) == 0) {
        unset($entities[$type][$bundle]);
      }
      else {
        $entities[$type][$bundle]['category'] = intval($val['category']);
      }
    }
  }

  $entities = array_filter($entities);
  variable_set(DISCOURSE_COMMENTS_VAR_ENTITIES, $entities);

  // Reset the field info so that field extras will get updated.
  _field_info_field_cache()->flush();
}


// =========================
// Server settings
// =========================

/**
 * Configure the server, and comment fetching settings.
 */
function discourse_comments_settings_form($form, &$form_state) {
  $protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";

  $defaults = variable_get(DISCOURSE_COMMENTS_VAR_SERVER, array()) + array(
    'host' => '',
    'api_key' => '',
    'api_username' => '',
  );

  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('Discourse Credentials'),
    '#tree' => TRUE,

    'host' => array(
      '#type' => 'textfield',
      '#title' => t('Discourse Host'),
      '#required' => TRUE,
      '#field_prefix' => t($protocol),
      '#default_value' => preg_replace('#^https?://#', '', $defaults['host']),
    ),
    'api_key' => array(
      '#type' => 'textfield',
      '#title' => t('API Key'),
      '#default_value' => $defaults['api_key'],
    ),
    'api_username' => array(
      '#type' => 'textfield',
      '#title' => t('Username'),
      '#default_value' => $defaults['api_username'],
    ),
  );

  // Settings for fetching Discourse comments.
  $defaults = variable_get(DISCOURSE_COMMENTS_VAR_FETCH_PARAMS, array()) + array(
    'best' => 5,
    'min_trust_level' => NULL,
    'min_score' => NULL,
    'min_replies' => NULL,
  );

  $form['params'] = array(
    '#type' => 'fieldset',
    '#title' => t('Comment Fetching'),
    '#tree' => TRUE,

    'best' => array(
      '#type' => 'textfield',
      '#title' => t('Number of comments'),
      '#size' => 5,
      '#element_validate' => array('element_validate_integer_positive'),
      '#default_value' => $defaults['best'],
    ),
    'min_trust_level' => array(
      '#type' => 'select',
      '#title' => t('Trust level'),
      '#options' => range(0, 5),
      '#empty_option' => '- No minimum -',
      '#default_value' => $defaults['min_trust_level'],
      '#description' => t(
        'Only fetch comments that have a trust level '.
        'greater than or equal to this setting.'
      ),
    ),
    'min_score' => array(
      '#type' => 'textfield',
      '#title' => t('Minimum score'),
      '#size' => 5,
      '#element_validate' => array('element_validate_integer_positive'),
      '#default_value' => $defaults['min_score'],
      '#description' => t(
         'Minimum score required prior to pulling comments across (score = '
        . '15 points per like, 5 per reply, 5 per incoming link, 0.2 per read).'
        . 'Leave this field blank if you do not want to set a minimum score.'
      ),
    ),
    'min_replies' => array(
      '#type' => 'textfield',
      '#title' => t('Minimum # of replies'),
      '#size' => 5,
      '#element_validate' => array('element_validate_integer_positive'),
      '#default_value' => $defaults['min_replies'],
      '#description' => t('Minimum number of replies before a comment will be included.'),
    ),
  );

  // Setup content settings.
  $form['content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Push Settings'),

    DISCOURSE_COMMENTS_VAR_CANONICAL => array(
      '#type' => 'textfield',
      '#title' => t('Canonical HOST for links back.'),
      '#field_prefix' => $protocol,
      '#default_value' => variable_get(DISCOURSE_COMMENTS_VAR_CANONICAL, $_SERVER['HTTP_HOST']),
      '#description' => t(
        'It is important to ensure that the server uses '.
        'the correct HTTP host when sending to discourse.'
      ),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',

    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );

  return $form;
}

/**
 * Submit save callback.
 */
function discourse_comments_settings_form_submit($form, &$form_state) {
  $server = $form_state['values']['server'];

  $server['host'] = preg_replace(array('#^https?://#', '#/+$#'), '', $server['host']);
  $server['api_username'] = strtolower($server['api_username']);

  variable_set(DISCOURSE_COMMENTS_VAR_SERVER, $server);

  // Save the fetch parameters.
  $params = array_filter($form_state['values']['params'], 'drupal_strlen');
  $params = array_map('intval', $params);

  if (empty($params['min_score'])) {
    $params['bypass_trust_level_score'] = 1;
  }

  variable_set(DISCOURSE_COMMENTS_VAR_FETCH_PARAMS, $params);

  // Save canonical server settings.
  $canonical = $form_state['values'][DISCOURSE_COMMENTS_VAR_CANONICAL];
  $canonical = preg_replace(array('#^https?://#', '#/+$#'), '', $canonical);

  variable_set(DISCOURSE_COMMENTS_VAR_CANONICAL, $canonical);
}
