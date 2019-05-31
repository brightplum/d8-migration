<?php
/**
 * @file
 * Contains field hooks which add formatters and alter UI fields.
 */


/**
 * Implements HOOK_field_widget_info().
 */
function iwformatter_field_widget_info() {
  $widgets['iwformatter_entityref'] = array(
    'label' => t('Entity Search/Create'),
    'description' => t('An entity search, edit and create widget.'),
    'field types' => array('entityreference'),
    'widget class' => 'iwEntityRefFieldWidget', // Allows us to defer code to a widget class
    'settings' => array(
      'match_operator' => 'CONTAINS',
      'size' => 60,
      'path' => '',
    ),
  );
    
  return $widgets;
}

/**
 * Implements hook_field_widget_settings_form().
 */
function iwformatter_field_widget_settings_form($field, $instance) {
  $type = $instance['widget']['type'];
  $widget_info = field_info_widget_types($type);
  $instance['widget']['settings'] += field_info_widget_settings($type);
  
  if (!empty($widget_info['widget class'])) {
    $widgetClass = $widget_info['widget class'];
    $widget = new $widgetClass($field, $instance);
    
    return $widget->getSettingsForm();
  }
}

/**
 * Implements hook_field_widget_form().
 */
function iwformatter_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $widget_info = field_info_widget_types($instance['widget']['type']);
  
  if (!empty($widget_info['widget class'])) {
    $widgetClass = $widget_info['widget class'];
    $widget = new $widgetClass($field, $instance);
    
    return $widget->getWidgetForm($form, $form_state, $langcode, $items, $delta, $element);
  }
}


// ====================================
// Field formatter HOOKs / callbacks
// ====================================

/**
 * Implements HOOK_field_formatter_info().
 */
function iwformatter_field_formatter_info() {
  return array(
    'iwformatter_field_page' => array(
      'label' => t('Summary with link to page view'),
      'field types' => array('text_long', 'text_with_summary'),
      'settings' => array(
        'trim_length' => 600,
        'url_text'    => 'more',
        'allow_trim'  => TRUE,
      ),
    ),
    'iwformatter_image_styles' => array(
      'label' => t('Multi-style images'),
      'field types' => array('image'),
      'settings' => array(
        'styles' => array(),
        'link'   => FALSE,
      ),
    ),
  );
}

/**
 * Implements HOOK_field_formatter_settings_form().
 */
function iwformatter_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display  = $instance['display'][$view_mode];
  $settings = $display['settings'] + array(
    'trim_length' => 600,
    'url_text'    => 'more',
    'allow_trim'  => TRUE,
  );

  $element = array();
  $element['allow_trim'] = array(
    '#title' => t('Allow trimmed value when summary not available'),
    '#type' => 'checkbox',
    '#default_value' => $settings['allow_trim'],
  );
  $element['trim_length'] = array(
    '#title' => t('Trim length'),
    '#type' => 'textfield',
    '#size' => 10,
    '#default_value' => $settings['trim_length'],
    '#element_validate' => array('element_validate_integer_positive'),
    '#required' => TRUE,
  );
  $element['url_text'] = array(
    '#title' => t('More link text'),
    '#type' => 'textfield',
    '#default_value' => $settings['url_text'],
  );
  return $element;
}

/**
 * Implements HOOK_field_formatter_settings_summary().
 */
function iwformatter_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display  = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';
  if (!empty($settings['allow_trim'])) {
    $summary .= t('Trim length') . ': ' . $settings['trim_length'];
    $summary .= ', ';
  }

  $summary .= t('More text: ');
  $summary .= empty($settings['url_text']) ? 'more' : $settings['url_text'];

  return $summary;
}

/**
 * Implements HOOK_field_formatter_view().
 */
function iwformatter_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  list($entity_id, $rev_id, $bundle) =  entity_extract_ids($entity_type, $entity);
  
  $element = array();
  foreach ($items as $delta => $item) {
    $classes = array('field-page');

    if (!empty($item['summary'])) {
      $output = _text_sanitize($instance, $langcode, $item, 'summary');
      $classes[] = 'more-link';
    }
    elseif (!empty($display['settings']['allow_trim'])) {
      $output = _text_sanitize($instance, $langcode, $item, 'value');
      $format = $instance['settings']['text_processing'] ? $item['format'] : NULL;
      $output = text_summary($output, $format, $display['settings']['trim_length']);
      $classes[] = 'more-link';
    }
    else {
      $output = '';
    }

    // Build URL settings that lead to the full page view of this field
    $field_uri = "field-page/{$entity_type}/{$entity_id}/{$field['field_name']}";
    $options   = ($delta != 0) ? array('query' => array('item' => $delta)) : array();

    // Add the link, along with the summary output.
    $element[$delta] = array(
      '#markup' => $output,
      '#suffix' => '<div class="'. implode(' ', $classes) .'">'.
      l($display['settings']['url_text'], $field_uri, $options) .'</div>',
    );
  }
  return $element;
}

/**
 * Implements HOOK_field_formatter_info_alter().
 */
function iwformatter_field_formatter_info_alter(&$info) {
  foreach ($info as $instance => $value) {
    $info[$instance]['settings']['field_classes'] = array();
  }

  foreach (_iwformatter_listable_formatters() as $formatter) {
    if (isset($info[$formatter])) {
      $info[$formatter]['settings']['list_inline']    = FALSE;
      $info[$formatter]['settings']['list_separator'] = ', ';
    }
  }
}

/**
 * Implements HOOK_field_formatter_settings_form_alter().
 */
function iwformatter_field_formatter_settings_form_alter(&$form, $context) {
  $display = $context['instance']['display'][$context['view_mode']];
  $settings = $display['settings'];

  $form['field_classes'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Formatter Classes'),
    '#default_value' => implode(' ', $settings['field_classes']),
    '#element_validate' => array('iwapi_element_validate_css_classes'),
    '#description' => t('Enter valid CSS class names separated by a space.') .'<br /><small>'.
    t('Note: CSS classes that are not valid will be removed without warning.') .'</small>',
  );

  if (_iwformatter_is_listable_formatter($context['field'], $display)) {
    $form['list_inline'] = array(
      '#type' => 'checkbox',
      '#title' => t('Combine list into a single list with a separator.'),
      '#default_value' => $settings['list_inline'],
    );
    $form['list_separator'] = array(
      '#type' => 'textfield',
      '#title' => t('List separator'),
      '#default_value' => $settings['list_separator'],
      '#description' => t('This item is ignored if list items are not combined into a single field.'),
    );
  }
}

/**
 * Implements hook_field_formatter_settings_summary_alter().
 */
function iwformatter_field_formatter_settings_summary_alter(&$summary, $context) {
  $display = $context['instance']['display'][$context['view_mode']];
  $settings = $display['settings'];

  if (!empty($summary)) {
    $summary .= '<br />';
  }

  if (!empty($settings['field_classes'])) {
    $summary .= t('Classes: @field', array('@field' => implode(' ', $settings['field_classes'])));
  }
  else {
    $summary .= t('Classes: *-None-');
  }

  if (_iwformatter_is_listable_formatter($context['field'], $display) && $settings['list_inline'] && !empty($settings['list_separator'])) {
    $separator = t("Inline list: (%separator)", array('%separator' => $settings['list_separator']));
    $summary = empty($summary) ? $separator : "{$summary}<br />{$separator}";
  }
}


// =====================================
// Theme / preprocess functions
// =====================================

/**
 * Implements HOOK_preprocess_field().
 */
function iwformatter_preprocess_field(&$vars, $hook) {
  $elem = $vars['element'];
  $view = isset($elem['#view_mode']) ? $elem['#view_mode'] : 'default';

  if (empty($elem['#display'])) {
    $instance = field_info_instance($elem['#entity_type'], $elem['#field_name'], $elem['#bundle']);
    $display = field_get_display($instance, $view, $elem['#object']);
    $field = field_info_field_by_id($instance['field_id']);
  }
  else {
    $display = $elem['#display'];
  }

  // Fields rendering doesn't handle additional field classes, we'll merge them in if available.
  if (!empty($elem['#attributes'])) {
    if (!empty($elem['#attributes']['class'])) {
      $vars['classes_array'] = array_merge($vars['classes_array'], $elem['#attributes']['class']);
      unset($elem['#attributes']['class']);
    }
    $vars['attributes_array'] = $elem['#attributes'];
  }

  if (!empty($display['settings']['field_classes'])) {
    $vars['classes_array'] = array_merge($vars['classes_array'], $display['settings']['field_classes']);
  }

  // Put all the fields on a single area with a separator.
  if (!empty($display['settings']['list_inline'])) {
    $item  = reset($vars['items']);
    $delta = key($vars['items']);
    $separator = field_filter_xss($display['settings']['list_separator']);

    $value = render($item);
    while ($next = next($vars['items'])) {
      $value .= $separator . render($next);
    }
    $vars['items'] = array($delta => array('#markup' => $value));
  }
}

/**
 * Implements HOOK_preprocess_image_formatter().
 */
function iwformatter_preprocess_image_formatter(&$vars) {
  if (!isset($vars['item']['attributes'])) {
    $vars['item']['attributes'] = array();
  }
  
  $attrs = &$vars['item']['attributes'];

  if (isset($vars['item']['override']['attributes'])) {    
    $attrs = $vars['item']['override']['attributes'];
  }
  
  if (empty($attrs['class'])) {
    $attrs['class'] = array();
  }
  
  // Add our own CSS class items.
  if (!empty($vars['item']['field_file_image_css_class'])) {
    $css = reset($vars['item']['field_file_image_css_class']);
    $css = empty($css) || !is_array($css) ? array() : reset($css);
    
    if (!empty($css['value'])) { 
      $attrs['class'] = array_merge($attrs['class'], explode(' ', $css['value']));
      $attrs['class'] = array_unique($attrs['class']);
    }
  }
}
