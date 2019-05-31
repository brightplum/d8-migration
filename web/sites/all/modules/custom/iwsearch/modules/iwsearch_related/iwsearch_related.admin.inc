<?php
/**
 * @file
 * The administration pages used to configure related search parameters.
 */


/**
 * Form builder to create the form used to setup the related
 * content results. A majority of this is controlled by
 * the solr search engine.
 */
function iwsearch_related_settings_form($form, &$form_state) {
   $form[IWSEARCH_RELATED_VAR_SIZE] = array(
     '#type' => 'textfield',
     '#title' => t('Number of related items'),
     '#size' => 4,
     '#field_suffix' => 'items',
     '#default_value' => variable_get(IWSEARCH_RELATED_VAR_SIZE, iwRelatedContent::DEFAULT_SIZE),
     '#description' => t('Number of items that should be displayed in the related content results.'),
   );
   
   $form[IWSEARCH_RELATED_CACHE_TTL] = array(
     '#type' => 'select',
     '#title' => t('Results cache TTL'),
     '#empty_option' => t('- No Caching -'),
     '#empty_value' => 0,
     '#options' => array(
       60     => t('1 minute'),
       300    => t('5 minutes'),
       600    => t('10 minutes'),
       1800   => t('30 minutes'),
       3600   => t('1 hour'),
       7200   => t('2 hours'),
       10800  => t('3 hours'),
       21600  => t('6 hours'),
       43200  => t('12 hours'),
       86400  => t('1 day'),
       172800 => t('2 days'),
       259200 => t('3 days'),
       604800 => t('1 week'),
     ),
     '#default_value' => variable_get(IWSEARCH_RELATED_CACHE_TTL, 3600),
     '#description' => t(
       'Length of time to cache related content search results. This will improve '.
       'preformance, at the cost of slower updates to capture new articles.'
     ),
   );
   
   // Allow users to select where the related content blocks appear.
   $form[IWSEARCH_RELATED_DISPLAY_TYPES] = array(
     '#type' => 'fieldset',
     '#title' => t('Content which display related content'),
     '#tree' => TRUE,
     '#description' => t(
       'Select content and bundles / types that will display'.
       'related content in their display settings.'
     ),
   );
   
   $types    = entity_get_info();
   $defaults = variable_get(IWSEARCH_RELATED_DISPLAY_TYPES, array());
   
   foreach ($types as $type => $info) {
     $info = entity_get_info($type);
     
     $form[IWSEARCH_RELATED_DISPLAY_TYPES][$type] = array(
       '#type' => 'checkboxes',
       '#title' => t('%type bundles', array('%type' => $info['label'])),
       '#options' => _iwapi_extract_field($info['bundles']),
       '#default_value' => isset($defaults[$type]) ? $defaults[$type] : array(),
     );
   }

   // Allow users to setup the node types that appear in the result
   // list. This only applies if Solr is not enabled since iwsearch
   // settings give you more control over this when available.
   if (!module_exists('apachesolr')) {
     $entity_info = entity_get_info('node');
       
     $form[IWSEARCH_RELATED_NODE_RESULT_TYPES] = array(
       '#type' => 'checkboxes',
       '#title' => t('Searchable node bundles'),
       '#options' => _iwapi_extract_field($entity_info['bundles']),
       '#default_value' => variable_get(IWSEARCH_RELATED_NODE_RESULT_TYPES, array()),
       '#description' => t(
         'Leaving this blank, will use all node bundles.'.
         'Also this setting only applies to searches that do not use iwsearch providers.'
       ),
     );
   }

   // Manage sorting changes.
   $sort = variable_get(IWSEARCH_RELATED_VAR_SORT, array()) + array(
     'field' => 'ds_created',
     'dir' => 'desc',
   );
   
   $form[IWSEARCH_RELATED_VAR_SORT] = array(
     '#type' => 'fieldset',
     '#title' => t('Sorting'),
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
     '#tree' => TRUE,

     'field' => array(
       '#type' => 'select',
       '#title' => t('Sort by'),
       '#options' => array(
         'ds_created' => t('Post date'),
         'ds_changed' => t('Change date'),
         'label' => t('By title / label'),
         'random' => t('Random (effected by caching)'),
       ),
       '#default_value' => $sort['field'],
     ),
     'dir' => array(
       '#type' => 'radios',
       '#title' => t('Sort direction'),
       '#options' => array(
         'asc' => t('Ascending'),
         'desc' => t('Descending'),
       ),
       '#default_value' => $sort['dir'],
       '#description' => t(
         'This translates to most recent (descending) and '.
         'oldest (ascending) for dates.'
       ),
     ),
   );
   
   $form['actions'] = array(
     '#type' => 'actions',
     
     'save' => array(
       '#type' => 'submit',
       '#value' => t('Save Settings'),
     ),
   );
   
   return system_settings_form($form);
}

/**
 * Form submit function to save the related search settings.
 */
function iwsearch_related_settings_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $values = $form_state['values'];
  
  foreach ($values[IWSEARCH_RELATED_DISPLAY_TYPES] as $type => &$bundles) {
    $bundles = array_filter($bundles);
  }
  unset($bundles);
  
  foreach ($values as $key => $value) {
    variable_set($key, $value);
  }
}
