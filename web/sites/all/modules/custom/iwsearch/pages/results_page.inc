<?php
/**
 * @file
 * The default search results page. Will render a search form with results.
 */


/**
 * Map search fields into abbreviated value names.
 * This was designed to be used with URL parameters,
 * so generated URI query can be dramatically shortened.
 * 
 * @param iwSearchProvider $provider
 *  The search provider, which this map
 *  will be used with.
 */
function _iwsearch_get_param_map(iwSearchProvider $provider) {
  static $maps;
 
  if (!isset($maps[$provider->getName()])) {
    $cid = 'iwsearch_fmap:' . $provider->getName();
    
    if ($cache = cache_get($cid)) {
      $map = $cache->data;  
    }
    else {
      $map = array();
      $cc1 = ord('i');
      $cc2 = ord('a');
      
      // Construct simple abbreviations for parameter values.
      foreach ($provider->elements as $name => $element) {
        switch ($name) {
          case 'keywords':
            $map['k'] = 'keywords';
            break;
          case 'bundles':
            $map['b'] = 'bundles';
            break;
          default:
            $map[ chr($cc1) . chr($cc2++)] = $name;
            if ($cc2 > ord('z')) {
              $cc2 = ord('a');
              ++$cc1;
            }
        }
      }      
      cache_set($cid, $map);
    }
    
    $maps[$provider->getName()] = $map;
  }
  
  // Get the field / datamapping for this search instance.
  return $maps[$provider->getName()];
}

/**
 * Translate the $_GET parameters into values
 * that can be used in a search.
 */
function _iwsearch_parse_search_criteria(iwSearchProvider $provider) {
  static $params;
  
  if (!isset($params[$provider->getName()])) {
    $map = _iwsearch_get_param_map($provider);
    
    $values = array();
    foreach (array_intersect_key($_GET, $map) as $key => $param) {
      $values[$map[$key]] = ($param[0] == '|') 
        ? explode('|', substr($param, 1)) : $param;
    }
    
    $params[$provider->getName()] = $values;
  }
  return $params[$provider->getName()];
}

/**
 * Form builder to generate a search form using an
 * iwSearchProvider object. Facilitates creation of
 * URI parameters and search mechanisms.
 */
function iwsearch_search_form($form, &$form_state, iwSearchResults $results_page) {
  form_load_include($form_state, 'inc', 'iwsearch', 'pages/results');
  
  // Save the results handler for later.
  $form_state['results_page'] = $results_page;
  $provider = $results_page->provider;
  
  // Get the current search values and return
  $values = _iwsearch_parse_search_criteria($provider);
  
  $form['filters'] = $provider->createForm($values, $form_state);
  $form['filters']['#type'] = 'container';
  $form['filters']['#tree'] = TRUE;
  $form['filters']['#attributes']['class'][] = 'search-filters';
  
  if (!empty($form['filters']['#auto_submit'])) {
    $form['#attached']['library'][] = array('iwapi', 'iwapi.ajax');

    foreach ($form['filters']['#auto_submit'] as $fieldname) {
      $form['filters'][$fieldname]['#ajax'] = array(
        'callback' => 'iwsearch_search_form_ajax',
        'trigger_as' => array('name' => 'op', 'value' => t('Search')),
      );
    }
  }
  
  $form['actions'] = array(
    '#type' => 'actions',
    
    'search' => array(
      '#type' => 'submit',
      '#value' => t('Search'),
    ),
  );
  
  return $form;
}

/**
 * Form state callback to submit the search criteria
 * for the page.
 */
function iwsearch_search_form_submit($form, &$form_state) {
  $results_page = $form_state['results_page'];
  $provider     = $results_page->provider;
  $filters      = $form_state['values']['filters'];
  $pmap         = array_flip(_iwsearch_get_param_map($provider));
  
  $uri_query = array();
  foreach ($provider->elements as $key => $element) {
    $val = $element->translateValue($filters[$key]);
    $uri_query[$pmap[$key]] = is_array($val) ? '|'. implode('|', $val) : $val;
  }
  
  // Propagate any random search seeds if they exists.
  if (!empty($_GET['ssrnd'])) {
    $uri_query['ssrnd'] = filter_input(INPUT_GET, 'ssrnd', FILTER_SANITIZE_NUMBER_INT);
  }

  // Always push to redirect to the search results page.
  $form_state['redirect'] = array(
    'path'    => $results_page->uri,
    'options' => array('query' => array_filter($uri_query)),
  );
}

/**
 * Form AJAX callback to request a page reload
 * with the new search parameters. This will allow
 * auto submit to occur for some search fields.
 */
function iwsearch_search_form_ajax($form, $form_state) {
  iwapi_include('ajax');
  
  $command = iwapi_ajax_command_redirect($form_state['redirect']);
  return array(
    '#type' => 'ajax',
    '#commands' => array($command),
  );
}


// ======================
// Render page
// ======================

/**
 * The default handler for generating a search results
 * page or and search form.
 * 
 * @param string $name
 *  Name of the search instance to use in
 *  creating the search form.
 */
function iwsearch_default_results_page($id) {
  if (!($handler = iwsearch_result_load($id))) {
    return MENU_NOT_FOUND;
  }
  
  drupal_set_title($handler->title); // Impose the title, incase of other menu quirkiness
  
  $provider = $handler->provider;
  $criteria = _iwsearch_parse_search_criteria($provider);
  
  // Start building the page.
  $page = array();
  if (!empty($handler->intro['value'])) {
    $intro = $handler->intro;
    
    $page['introduction'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('introduction')),
       
      'content' => array(
        '#markup' => check_markup($intro['value'], $intro['format']),
      ),
    );
  }

  // Include the search parameters.
  $page['search'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('search-form')),
      
    'form' => drupal_get_form('iwsearch_search_form', $handler),
  );
  
  try {
    $resp     = $provider->execute($criteria, array(
      'pageSize' => $handler->pageSize,
      'fields' => $handler->additionalFields(),
      'highlight' => $handler->highlightFields(), 
      'sort' => $handler->getSort($provider->elements),
    ));

    $page['content'] = array(
      '#theme'     => 'iwsearch_results',
      '#provider'  => $provider,
      '#count'     => $resp['count'],
      '#summary'   => $resp['summary'],
      '#pageSize'  => $handler->pageSize,
      '#results'   => $handler->formatResults($resp),
      '#has_pager' => $handler->hasPager(),
    );
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    
    $page['content'] = array(
      '#markup' => '<div class="search-error clearfix">'. 
        t('Unable to retrieve search results. Please contact administrator for help.').
        '</div>',
    );
  }

  return $page;
}

/**
 * Provide search results through AJAX callbacks.
 * 
 * @param int $id
 *  Primary key for finding the result handler to
 *  use to run and format the search.
 * @return array
 *  A renderable array in the format of a Drupal
 *  AJAX response.
 */
function iwsearch_default_results_ajax($id) {
  if (!($handler = iwsearch_result_load($id))) {
    return MENU_NOT_FOUND;
  }
  
  $provider = $handler->provider;
  $criteria = _iwsearch_parse_search_criteria($provider);
  
  try {
    $resp = $provider->execute($criteria, array(
      'pageSize' => $handler->pageSize,
      'fields' => $handler->additionalFields(),
      'highlight' => $handler->highlightFields(), 
      'sort' => $handler->getSort($provider->elements),
    ));

    return array(
      '#type' => 'ajax',
      '#commands' => $handler->formatAjax($resp),
    );
  }
  catch (Exception $e) {
    watchdog_exception('iwsearch', $e);
    
    $msg = t('Unable to retreive additional results.');
    return array(
      '#type' => 'ajax',
      '#commands' => array(ajax_command_alert($msg)),
    );
  }
}