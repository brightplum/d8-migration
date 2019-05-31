<?php
/**
 * @file iwapi.admin.inc
 * Import/Export forms for settings and configurations.
 */


// --------------------------
// Mutex configurations
// --------------------------

/**
 * Form builder, for the general iwapi systems module to
 * set system wide behavior of utilities and handlers.
 * 
 * @see system_settings_form().
 */
function iwapi_admin_setting_form($form, &$form_state) {
  $form[IWAPI_VAR_MUTEX_TYPE] = array(
    '#type' => 'radios',
    '#title' => t('Underlying mutex'),
    '#ajax' => array(
      'wrapper' => 'configure-memcache',
      'callback' => 'iwapi_admin_setting_mutex_changed_ajax',
    ),
  );
    
  $available_locks = array(
    'iwFlockMutex' => t('flock() (most compatible or for simple testing)'),
  );
 
  // If the APC extension is available, make it a possible option.
  if (function_exists('apc_add')) {
    $available_locks['iwApcMutex'] = t('APC (preferred for single servers)');
  }

  $currMutex = variable_get(IWAPI_VAR_MUTEX_TYPE, 'iwFlockMutex');
  if(isset($form_state['values'][IWAPI_VAR_MUTEX_TYPE])) {
    $currMutex = $form_state['values'][IWAPI_VAR_MUTEX_TYPE];
  }

  if (!array_key_exists($currMutex, $available_locks)) {
    $currMutex = 'iwFlockMutex';
  }

  // Does not check for the memcache server connection, only that
  // the library is available.
  if (class_exists('Memcache')) {
    $available_locks['iwMemcacheMutex'] = t('Memcache (preferred for clusters)');
    
    $host = variable_get(IWAPI_VAR_MEMCACHE_LOCATION, array()) + array(
      'host' => 'localhost', 'port' => '11211',
    );

    $form[IWAPI_VAR_MEMCACHE_LOCATION] = array(
      '#prefix' => '<div id="configure-memcache">',
      '#suffix' => '</div>',
      '#type' => 'fieldset',
      '#title' => t('Memcache Connection Information'),
      '#tree' => TRUE,
      '#description' => t('Tell Drupal where memcache should connect to.'),
      '#access' => $currMutex == 'iwMemcacheMutex',
    
      'host' => array(
        '#type' => 'textfield',
        '#title' => t('Host'),
        '#validate_element' => array(),
        '#default_value' => $host['host'],
        '#description' => t('Use either the IP address or the string "localhost".'),
      ),
      'port' => array(
        '#type' => 'textfield',
        '#title' => t('Port'),
        '#validate_element' => array('element_validate_integer_positive'),
        '#default_value' => $host['port'],
      ),
    );
  }
  
  $form[IWAPI_VAR_MUTEX_TYPE]['#options']       = $available_locks;
  $form[IWAPI_VAR_MUTEX_TYPE]['#default_value'] = $currMutex;
  
  return system_settings_form($form);
}

/**
 * Form AJAX callback which updates the memcache host.
 */
function iwapi_admin_setting_mutex_changed_ajax($form, &$form_state) {
  return $form[IWAPI_VAR_MEMCACHE_LOCATION];
}


// --------------------------
// Utilize packages
// --------------------------

/**
 * 
 * @param string $package
 * @return string
 */
function _iwapi_import_package_utilize($package = NULL) {	
	global $user;
	
	$bundle_info = field_info_instances('node', 'article');
	$node_info = node_type_get_type('article');
	$field_info = field_info_field('field_regular');
	$values = node_load(23);
	  drupal_set_message("<pre>". print_r($values, TRUE) .'</pre>');
	  drupal_set_message("<pre>". print_r($bundle_info, TRUE) .'</pre>');
	
	$langcode = LANGUAGE_NONE;
	$node = (object) array(
		'title' => 'I have something to say kevin 2',
		'type' => 'article',
		'uid' => $user->uid,
		'language' => $langcode,
		'status' => 1,
    'body' => array(
      $langcode => array(
        array(
          'value' => 'Testing the saving of a node through this body field.',
        	'format' => filter_default_format(),
        ),
      ),
    ),
		'field_image' => array(
			$langcode => array(
				array('fid' => 79, 'alt' => 'alternative', 'title' => 'the title'),
			),
		),
		'field_post_date' => array(
		  $langcode => array(
		  	array('value' => "2012-04-15 00:00:00"),
		  ),
		),
	);
	/*
	node_save($node);
	
	drupal_set_message('<pre>'. print_r($values, TRUE) .'</pre>');
	try {
		_iwapi_load_class($className = $def['className']);
		$datamap = $className::unpackage($def);
	  $datamap->addSource('/home/liem/test.zip');
	  $datamap->execute();
	}
	catch (iwException $e) {
		drupal_set_message('Unable to use importer, or an error has occurred while loading.');
		$e->logToWatchdog();
	}*/
	return 'hello';
}

/**
 * Create a form that exposes iwapi helper export options.
 * 
 * @param $form
 *  Previous form structure from previous iteration
 * @param $form_state
 *  Previous iteration of the form data.
 */
function iwapi_package_export_form($form, &$form_state) {
  if (!empty($form_state['storage']['export'])) {
    $form['output_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Export output'),
      '#rows' => '20',
      '#default_value' => $form_state['storage']['export'],
    ); 
    $form['ret_link'] = array(
      '#markup' => '<div><a href="">Return to export form</a></div>',
    );
    unset($form_state['storage']['export']);
    return $form;
  }
  
  $helper_info = iwapi_list_helper_info();
  foreach ($helper_info as $class => $info) {
    if (is_array($info['export']) && !empty($info['export'])) {
      $form[$class] = array(
      	'#type' => 'fieldset',
        '#title' => $info['title'],
        '#tree' => TRUE,
      	'#collapsed' => TRUE,
        '#collapsible' => TRUE,
      );
 
      iwapi_load_helper($class);
      $inst = new $class();
      foreach ($info['export'] as $type) {
        $form[$class][$type] = array(
          '#type' => 'checkbox',
          '#title' => t(ucfirst($type)),
          '#default_value' => FALSE,
        );
        $opts = $inst->get_export_options($type);
        if (is_array($opts) && !empty($opts)) {
          $form[$class]["$type-opts"] = array(
            '#type' => 'select',
            '#multiple' => TRUE,
            '#options' => array_combine($opts, $opts),
            '#default_value' => $opts,
          );
        }
      }
    }
  }
  
  $form['destination_select'] = array(
    '#type' => 'select',
    '#title' => t('Export results to:'),
    '#options' => array(
      'file' => 'file download',
      'textarea' => 'text area',
    ),
  );
  $form['export_btn'] = array(
    '#type' => 'submit',
    '#value' => 'Export Configuration',
  );
  return $form;
}

/**
 * Determine what items were selected to be exported.
 * - Create the package settings array
 * - Determine the destination, and send the data to it.
 * 
 * @param $form
 *  Current form structure information.
 * @param $form_state
 *  Current form values to be used to build various 
 *  package exports and information.
 */
function iwapi_package_export_form_submit($form, &$form_state) {  
  $packages = array();
  $helper_info = iwapi_list_helper_info();
  $export_list = array_intersect_key($form_state['values'], $helper_info);
  
  // May no longer be needed in Drupal 7 but safety first.
  iwapi_load_helper(array_keys($export_list));
  foreach ($export_list as $class => $values) {
    $helper = new $class();
    $allowed = $helper->get_info();
    $values = array_filter($values);
    $types = array_intersect(array_keys($values), $allowed['export']);

    // Export each of the data types requested.
    foreach ($types as $type) {
      $options = isset($values["$type-opts"]) ? $values["$type-opts"] : NULL;
      $exported = $helper->export($type, $options);

      if (is_array($exported) && !empty($exported)) {
        $packages = array_merge($packages, $exported);
      }
    }
  }
  
  // Convert the array into exportable content, and deliver it.
  switch ($form_state['values']['destination_select']) {
    case 'file':
      $filename = 'configs_'. date('Ymd_his') .'.inc';
      drupal_add_http_header('Content-type', 'text/plain; charset=utf-8');
      drupal_add_http_header('Cache-Control', 'max-age=60, must-revalidate');
      drupal_add_http_header('Content-Disposition', 'attachment; filename="'. $filename .'"');
      
      // TODO: Create a safe format type for exporting just data.
      //  PHP files could potentially contain code, which if blindly included
      //  could be problematic. Mitigated by only allowing admins, but still
      //  I feel uneasy about not protecting this in some way. Ideas?
      print serialize($packages);
      exit();
    case 'textarea':
      $form_state['storage']['export'] = '$iw_package = '. var_export($packages, TRUE);
      $form_state['rebuild'] = TRUE;
      break;
  }
}

/**
 * Build the import form, attempt to select configurations
 *  provided by iwapi_configurations HOOK.
 * 
 * @param $form
 *  Current form structure definition (empty unless form was rebuilt).
 * @param $form_state
 *  Any form settings or storage values useful in building the form.
 */
function iwapi_package_import_form($form, &$form_state) {
  $opts = array(0 => t('-- Select an import --'));
  
  // Determine which configurations are actually valid for this user
  $configs = _iwapi_get_configuration_info();
  foreach ($configs as $key => $info) {
    if (is_numeric($key) || !_iwapi_package_access($info)) {
      unset($configs[$key]);
    }
    else {
      $opts[$key] = "{$info['module']}: {$info['title']}";
    }
  }
  
  if (!empty($opts)) {
    $form['configs'] = array(
      '#type' => 'value',
      '#value' => $configs,
    );
    
    $form['import_configs'] = array(
      '#type' => 'fieldset',
      '#title' => t('Configurations Packages'),
    
    	'import_select' => array(
      	'#type' => 'select',
      	'#title' => t('Module provided configurations'),
      	'#description' => t('Configurations provided by various modules.') .'<br /><sup>*</sup>'. 
          t('Note: %info.', array('%info' => 'Only configurations that you have access to will appear')),
      	'#options' => $opts,
      ),
    	'import_btn' => array(
      	'#type' => 'submit',
      	'#value' => 'Import Configuration',
      ),
    	'manifest_button' => array(
      	'#type' => 'submit',
      	'#value' => 'View Package Manifest',
      ),
    );
  }
  return $form;
}

/**
 * Execute the import process on selected configuration files.
 *  If user just wants to view package manifest, redirect to
 *  that page.
 * 
 * @param $form
 *  Form structure, and configuration file settings.
 * @param $form_state
 *  Values and information about the actions to be performed.
 */
function iwapi_package_import_form_submit($form, &$form_state) {  
  $name = $form_state['values']['import_select'];
  $configs = $form_state['values']['configs'];
  
  if (isset($configs[$name])) {
    if ($form_state['values']['op'] === 'View Package Manifest') {
      $form_state['redirect'] = 'admin/config/system/iwapi-utils/import/'. $name .'/manifest';
      return;
    }
    
    if (is_readable($configs[$name]['file'])) {
      $iw_packages = unserialize(file_get_contents($configs[$name]['file']));
  
      if (isset($iw_packages) && is_array($iw_packages)) {
        foreach ($iw_packages as $package) {
          $msg_params = array(
            '%pack' => $package['helper'],
            '%type' => $package['type'],
            '%data' => implode(', ', array_keys($package['data'])),
          );
          drupal_set_message(t('Importing package "%pack : %type" (%data).', $msg_params));
          
          $helper = $package['helper'];
          $helper = new $helper();
          $helper->import($package['type'], $package);
        }
      }
    }
    else {
      drupal_set_message('Unable to read configuration file.', 'error');
    }
  }
}

/**
 * Build a page with the summary of package contents, so
 *  administrators can see what they're importing. There 
 *  should be a "diff" view to help distinguish change 
 *  from current system settings.
 * 
 * @param $package_id
 *  Name of the package configuration to list information.
 */
function _iwapi_package_manifest_page($package_id) {
  return $package_id;
}


// =====================================
// Package component edit form
// =====================================
