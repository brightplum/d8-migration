<?php
/**
 * @file
 * Contains the page callbacks and general settings for content imports.
 */


/**
 * Form builder which generates a form that can trigger
 * a data import from a database to the 
 */
function cdreams_import_data_form($form, &$form_state) {
  require_once 'includes/install.inc';
  
  $drivers = array();
  foreach (drupal_get_database_types() as $key => $obj) {
    $drivers[$key] = $obj->name();
  }

  $baseFolder = drupal_get_path('module', 'cdreams') .'/import';
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import from Drupal 6'),
    '#weight' => -10,
    
    'db' => array(
      '#type' => 'fieldset',
      '#title' => 'Source Database',
      '#tree' => TRUE,
      
      'driver' => array(
        '#type' => 'select',
        '#title' => t('Database type'),
        '#options' => $drivers,
        '#default_value' => 'mysql',
      ),
      'host' => array(
        '#type' => 'textfield',
        '#title' => t('Host'),
        '#size' => 32,
        '#default_value' => 'localhost',
      ),
      'port' => array(
        '#type' => 'textfield',
        '#title' => t('Port'),
        '#size' => 32,
        '#default_value' => 3306,
        '#element_validate' => array('element_validate_integer_positive'),
      ),
      'database' => array(
        '#type' => 'textfield',
        '#title' => t('Database'),
        '#size' => 32,
        '#default_value' => 'cdream_old',
      ),
      'username' => array(
        '#type' => 'textfield',
        '#title' => t('Username'),
        '#size' => 32,
        '#default_value' => 'root',
      ),
      'password' => array(
        '#type' => 'textfield',
        '#title' => t('Password'),
        '#size' => 32,
        '#default_value' => 'admin',
      ),
    
      'import_type' => array(
        '#type' => 'select',
        '#title' => t('Import content'),
        '#parents' => array('import_type'),
        '#options' => array(
          t('Content Resources') => array(
            "$baseFolder/files.inc" => t('Files from "old_files" directory'),
            "$baseFolder/topics.inc" => t('Tags / Topics'),
            "$baseFolder/categories.inc" => t('Category taxonomy from database'),
            "$baseFolder/organization_category.inc" => t('Organization categories from database'),
          ),
          t('Content') => array(
            "$baseFolder/further.inc"  => t('Further'),
            "$baseFolder/headline.inc" => t('Headline articles'),
            "$baseFolder/newswire.inc" => t('Newswire articles'),
            "$baseFolder/views.inc"    => t('Views articles'),
            "$baseFolder/video.inc"    => t('Video articles'),
          ),
        ),
      ),
    ),
    
    // Setup a date filter for making a cutoff for content types.
    'filters' => array(
      '#type' => 'fieldset',
      '#title' => t('Additional filters / conditions'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#tree' => TRUE,
      '#weight' => 5,
      
      'created' => array(
        'op' => array(
          '#type' => 'value',
          '#value' => '>=',
        ),
        'value' => array(
          '#type' => 'date_select',
          '#title' => t('Created after'),
          '#date_format' => 'Y-m-d',
          '#date_label_position' => 'within',
          '#description' => t(
            'Choosing a date will restrict the import to use content created '.
            'after this date. Leaving this blank will import all content.'
          ),
        ),
      ),
    ),
    'actions' => array(
      '#type' => 'actions',
      
      'import_btn' => array(
        '#type' => 'submit',
        '#value' => t('Import'),
        '#submit' => array('cdreams_import_data_import_submit'),
      ),
    ),
  );
  
  // Alter the date selection to a better pop-up widget.
  if (module_exists('date_popup')) {
    $form['import']['filters']['created']['value']['#type'] = 'date_popup';
    $form['import']['filters']['created']['value']['#date_year_range'] = '-10:+0';
  }
  
  $element = &$form['import']['db']['import_type'];
  
  // Add profile updates as an option
  if (module_exists('cdreams_profile')) {
    $element['#options'] = array(
      t('Profiles') => _cdreams_profile_list_imports(),
    ) + $element['#options'];
  }
    
  // Fix redirects if available.
  if (module_exists('redirect')) {
    $element['#options'] = array(
      t('Redirects') => array("$baseFolder/redirects.inc" => 'URL Redirects'),
    ) + $element['#options'];
  }
    
  return $form;
}

/**
 * Form submit callback which builds a batch process
 * for handling the importing of items.
 */
function cdreams_import_data_import_submit($form, &$form_state) {
  $values = $form_state['values'];
  
  $filename = $values['import_type'];
  if (file_exists($filename)) {
    $conds = array();
    $dates = array('created', 'changed');
    
    foreach ($values['filters'] as $cond_name => $info) {
      if (!empty($info['value'])) {
        $conds[$cond_name] = array('field' => $cond_name) + (
          in_array($cond_name, $dates) ? array('value' => strtotime($info['value'])) + $info : $info
        );
      }
    }

    $batch = array(
      'operations' => array(
        array('cdreams_import_batch', array($filename, $values['db'], $conds)),
      ),
      'finished' => 'cdreams_import_batch_finished',
      'progress_message' => t('Processed @current out of @total.'),
      'title' => t('Importing %type content', array('%type' => $values['import_type'])),
      'file' => drupal_get_path('module', 'cdreams') .'/pages/import.inc',
    );
    
    batch_set($batch);
  }
  else {
    drupal_set_message('The requested import does not exist.', 'error');
  }
}

/**
 * Do a batch worth of import items.
 * 
 * @param type $filename
 *  The name of the file to load in order to get the importer
 *   definitions.
 * @param type $db
 *  The source database information.
 * @param type $context
 *  Reference to the current status of this batch operation.
 */
function cdreams_import_batch($filename, $db, $conds, &$context) {    
  if (!isset($context['sandbox']['source'])) {
    $context['sandbox']['completed'] = FALSE;
    $context['sandbox']['source']    = $db;
    $context['sandbox']['count']     = 0;
  }
  
  // Construct and use the import file.
  require $filename;
  
  $def['reader']->conditions = $conds;
  $importer = new iwDataImport($def);
  
  $percent = $importer->batch($context['sandbox'], $context['results']);

  // Check for completion, and report the progress.
  $context['finished'] = ($context['sandbox']['completed']) 
    ? 1 : ($percent < 1 ? $percent : 0.999);

  $context['message']  = t('%count items imported', array('%count' => $context['sandbox']['count']));
}

/**
 * Batch completion summary and final checks.
 */
function cdreams_import_batch_finished($success, $results, $operations) {
  if ($success) {
    // Here we do something meaningful with the results.
    $message = count($results) .' processed.';
    $message .= theme('item_list', $results);  // D6 syntax
    drupal_set_message($message);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
    drupal_set_message($message, 'error');
  }
}

/**
 * Check and create a redirect to the new node.
 */
function _cdreams_import_prefix_nid_uri($nid) {
  return (!empty($nid) && is_numeric($nid)) ? 'node/'. $nid : NULL;
}