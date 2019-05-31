<?php

/**
 * @file
 * Template overrides as well as (pre-)process and alter hooks for the
 * omega_dreams theme.
 */

function omega_dreams_omega_layout_alter(&$layout) {
  $cPath = current_path();
  if (strpos($cPath, 'donate') !== FALSE) {
    $layout = 'bootstrap';
  }
}

/**
 * Implements hook_omega_layout_alter().
 */
function omega_dreams_omega_layouts_info_alter(&$layout) {
  // Add block inject regions.
  if (module_exists('block_inject')) {
    // Find all the block inject regions.
    $regions = block_inject_get_regions();
    // Add regions to the site.
    foreach ($regions as $region) {
      $layout['cdreams']['info']['regions']['block_inject-' . $region->id] = t('Block Inject: @title', array('@title' => $region->region));
    }
  }
}

function omega_dreams_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'search_block_form') {
    $form['actions']['submit']['#button_type'] = 'button';
    $form['actions']['submit']['#value'] = '<i class="fa fa-search" aria-label="Search"></i>';
    $form['actions']['submit']['#attributes']['aria-label'] = 'Search';
  }
}

function omega_dreams_preprocess_node(&$vars) {
  $vars['theme_hook_suggestions'][] = 'node__mode__'. $vars['view_mode'];
}

function omega_dreams_wysiwyg_editor_settings_alter(&$settings, $context) {
    if ($context['profile']->editor == 'tinymce') {
        $settings['gecko_spellcheck'] = TRUE;
        // Any init setting taking a string, boolean, number or array/object holding those types can be overridden/forced here.
    }
}

function omega_dreams_preprocess_html(&$variables) {
  // You can use preprocess hooks to modify the variables before they are passed
  // to the theme function or template file.
  $sizes = array('76x76', '120x120', '150x150');
  $path  = drupal_get_path('theme', 'omega_dreams') .'/images/apple-touch';

  $uri = url($path, array('absolute' => TRUE));
  foreach ($sizes as $size) {
    drupal_add_html_head(array(
      '#type' => 'html_tag',
      '#tag' => 'link',
      '#attributes' => array(
        'rel' => 'apple-touch-icon-precomposed',
        'href' => $uri ."/favicon-{$size}.png",
        'sizes' => $size,
      ),
    ), "apple-touch-favicon-{$size}");
  }

  // Add a default favicon for other sizes.
  drupal_add_html_head(array(
    '#type' => 'html_tag',
    '#tag' => 'link',
    '#attributes' => array(
      'rel' => 'apple-touch-icon-precomposed',
      'href' => $uri ."/favicon.png",
    ),
  ), "apple-touch-favicon");

  drupal_add_js('//platform.twitter.com/widgets.js', 'external'); // Add This Library

  // Template suggestions.
  if (arg(0) == 'node' && is_numeric(arg(1))) {
    $type = db_query('SELECT type FROM {node} WHERE nid = :nid', [':nid' =>  filter_xss(arg(1))])->fetchField();
    array_unshift($variables['theme_hook_suggestions'], 'html__' . $type);
  }

  // Add waypoints library for the sticky header.
  if (current_path() != 'donate') {
    drupal_add_js(drupal_get_path('module', 'ctas') . '/js/waypoints/jquery.waypoints.min.js', [
      'scope' => 'footer',
    ]);
  }

  if (isset($_GET['cta-theme'])) {
    $variables['classes_array'][] = drupal_html_class('cta-theme-' . $_GET['cta-theme']);
  }

}

function omega_dreams_html_head_alter(&$head_elements) {
  foreach ($head_elements as $key => $element) {

    // Replace all non http with https.
    $protocol = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off' || $_SERVER['SERVER_PORT'] == 443) ? "https://" : "http://";
    if ($protocol == "https://") {
      $head_elements['metatag_canonical']['#value'] = str_replace('http:', 'https:', $head_elements['metatag_canonical']['#value']);
      $head_elements['metatag_shortlink']['#value'] = str_replace('http:', 'https:', $head_elements['metatag_shortlink']['#value']);
    }

    // Fix Canonicals on the following paths that IW has
    // a special callback for. See cdreams.module line cdreams_menu().
    if ($key == 'metatag_canonical') {
      $arg = arg(0);

      // Only gets picked up from a View. $arg will be "node" if on a detail page.
      if (preg_match('/news|views|newswire|further/', $arg)) {
        $path = explode('/', $_GET['q']);
        if (isset($path[1]) && strlen($path[1]) > 4) {
          // This is the bogus URL, reset the canonical.
          $head_elements['metatag_canonical']['#value'] = url($arg, ['absolute' => TRUE]);
          $head_elements['metatag_shortlink']['#value'] = url($arg, ['absolute' => TRUE]);
        }
        elseif (isset($path[4])) {
          // This is the bogus URL, reset the canonical.
          unset($path[4]);
          $head_elements['metatag_canonical']['#value'] = url(implode('/', $path), ['absolute' => TRUE]);
          $head_elements['metatag_shortlink']['#value'] = url(implode('/', $path), ['absolute' => TRUE]);
        }
      }
    }
  }
}

/**
 * Implements TEMPLATE_preprocess_search_result()
 *
 * Remove author attributions from search results.
 */
function omega_dreams_preprocess_search_result(&$vars) {
  $vars['info'] = '';
  $vars['info_split'] = array();

  if (!empty($vars['result']['fields']['ss_field_hp_author'])) {
    $vars['info_split']['author'] = $vars['result']['fields']['ss_field_hp_author'];
  }

  if (!empty($vars['result']['fields']['ds_field_article_date'])) {
    $ts = strtotime($vars['result']['fields']['ds_field_article_date']);

    $vars['info_split']['date'] = format_date($ts, 'custom', 'm/d/Y');
  }

  $vars['info'] = implode(' - ', $vars['info_split']);
}

/**
 * Override theme_pager_link().
 */
function omega_dreams_pager_link($variables) {
  $text = $variables['text'];
  $page_new = $variables['page_new'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  $attributes = $variables['attributes'];

  $page = isset($_GET['page']) ? $_GET['page'] : '';
  if ($new_page = implode(',', pager_load_array($page_new[$element], $element, explode(',', $page)))) {
    $parameters['page'] = $new_page;
  }

  $query = array();
  if (count($parameters)) {
    $query = drupal_get_query_parameters($parameters, array());
  }
  if ($query_pager = pager_get_query_parameters()) {
    $query = array_merge($query, $query_pager);
  }

  // Set each pager link title
  if (!isset($attributes['title'])) {
    static $titles = NULL;
    if (!isset($titles)) {
      $titles = array(
        t('« first') => t('Go to first page'),
        t('‹ previous') => t('Go to previous page'),
        t('next ›') => t('Go to next page'),
        t('last »') => t('Go to last page'),
      );
    }
    if (isset($titles[$text])) {
      $attributes['title'] = $titles[$text];
    }
    elseif (is_numeric($text)) {
      $attributes['title'] = t('Go to page @number', array('@number' => $text));
    }
  }

  // Pagination with rel=“next” and rel=“prev”. Does not support well multiple
  // pagers on the same page - it will create relnext and relprev links
  // in header for that case only for the first pager that is rendered.
  static $rel_prev = FALSE, $rel_next = FALSE;
  if (!$rel_prev && $text == t('‹ previous')) {
    $rel_prev = TRUE;
    $query = drupal_get_query_parameters();
    if (isset($query['page'])) {
      $query['page']--;
    }
    drupal_add_html_head_link(array(
      'rel' => 'prev',
      'href' => url(current_path(), ['query' => $query]),
    ));
  }
  if (!$rel_next && $text == t('next ›')) {
    $rel_next = TRUE;
    // Add $grddl_profile as link-tag.
    $query = drupal_get_query_parameters();
    if (isset($query['page'])) {
      $query['page']++;
    }
    else {
      $query['page'] = 1;
    }
    drupal_add_html_head_link(array(
      'rel' => 'next',
      'href' => url(current_path(), ['query' => $query]),
    ));
  }

  // @todo l() cannot be used here, since it adds an 'active' class based on the
  //   path only (which is always the current path for pager links). Apparently,
  //   none of the pager links is active at any time - but it should still be
  //   possible to use l() here.
  // @see http://drupal.org/node/1410574
  $attributes['href'] = url($_GET['q'], array('query' => $query));
  return '<a' . drupal_attributes($attributes) . '>' . check_plain($text) . '</a>';
}

function omega_dreams_apachesolr_search_noresults() {
  return t('
<p><strong><a href="https://archive.commondreams.org/" target="_blank">Also consider searching our archives</a></strong></p>
<ul>
<li>Check if your spelling is correct, or try removing filters.</li>
<li>Remove quotes around phrases to match each word individually: <em>"blue drop"</em> will match less than <em>blue drop</em>.</li>
<li>You can require or exclude terms using + and -: <em>big +blue drop</em> will require a match on <em>blue</em> while <em>big blue -drop</em> will exclude results that contain <em>drop</em>.</li>
</ul>');
}

/**
 * Implements hook_preprocess_field().
 */
function omega_dreams_preprocess_field(&$variables) {
  // Add structured data data to author field.
  if ($variables['element']['#field_name'] == 'field_article_date') {
    $markup = $variables['element'][0]['#markup'];
    $date = date('r', $variables['element']['#items'][0]['value']);
    $variables['items'][0]['#markup'] = '<span class="pb-timestamp" itemprop="datePublished" content="' . $date . '">' . $markup . '<time class="dateline" datetime="' . $date . '" itemprop="dateModified" content="' . $date . '"></time></span>';
  }
  // Add structured data to image field.
  if (preg_match('/field_article_img|field_profile_img/', $variables['element']['#field_name'])) {
    if (isset($variables['items']) && !empty($variables['items'])) {
      $image_url = file_create_url($variables['items'][0]['#item']['uri']);
      $variables['items'][0]['#prefix'] = '<div itemscope itemprop="image" itemtype="http://schema.org/ImageObject"><meta itemprop="url" content="' . $image_url . '">';
      $variables['items'][0]['#suffix'] = '</div>';
    }
  }
  // Newswire author.
  if ($variables['element']['#field_name'] == 'field_organization') {
    $variables['items'][0]['#prefix'] = '<span class="pb-byline" itemprop="author" itemscope itemtype="http://schema.org/Person"><span itemprop="name">';
    $variables['items'][0]['#sufix'] = '</span></span>';
  }
  // Add structured data to headline.
  if ($variables['element']['#field_name'] == 'body') {
    $wordcount = count(explode(" ", strip_tags(trim($variables['element'][0]['#markup']))));
    $schema_image = !empty($variables['element']['#object']->field_image) ? file_create_url($variables['element']['#object']->field_image[LANGUAGE_NONE][0]['uri']) : NULL;
    $variables['items'][0]['#prefix'] = '<div itemprop="articleBody">';
    if ($schema_image) {
      $variables['items'][0]['#prefix'] .= '<meta itemprop="image" content="' . $schema_image . '">';
    }
    $variables['items'][0]['#prefix'] .= '<meta itemprop="wordCount" content="' . $wordcount . '">';
    $variables['items'][0]['#suffix'] = '</div>';
  }
  // Add structured data to subtitle.
  if ($variables['element']['#field_name'] == 'field_subtitle') {
    $variables['items'][0]['#prefix'] = '<div itemprop="description">';
    $variables['items'][0]['#suffix'] = '</div>';
  }
}

/**
 * OVERRIDE theme_button().
 *
 * Returns HTML for a button form element.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #attributes, #button_type, #name, #value.
 *
 * @ingroup themeable
 */
function omega_dreams_button($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'submit';
  element_set_attributes($element, array('id', 'name', 'value'));

  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];
  if (!empty($element['#attributes']['disabled'])) {
    $element['#attributes']['class'][] = 'form-button-disabled';
  }

  if ($element['#button_type'] == 'button') {
    return '<button' . drupal_attributes($element['#attributes']) . '>' . $element['#value'] . '</button>';
  }
  else {
    return '<input' . drupal_attributes($element['#attributes']) . ' />';
  }

}

/**
 * Implements hook_preprocess_views_view_row_rss.
 */
function omega_dreams_preprocess_views_view_row_rss(&$vars) {
  unset($vars['row']->elements[1]);
  $item = &$vars['row'];
  $vars['item_elements'] = empty($item->elements) ? '' : format_xml_elements($item->elements);
}
