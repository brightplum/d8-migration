<?php
/**
 * @file
 * Contains the basic implementation of the search results builder.
 */


abstract class iwSearchResults implements iwIConfigurable {

  public $id;
  public $uri;
  public $label;
  public $title;
  
  public $hasBlock = 0;
  public $pageSize = 10;
  public $sort     = array();
  public $intro    = array('value' => '', 'format' => NULL);


  /**
   * Generate a new results object from
   * configuration settings.
   * 
   * @param iwSearchProvider $provider
   *  The search provider object, that will be used with
   *  this result handler.
   * @param array $data
   *  The settings to use when rendering this result type.
   */
  function __construct(iwSearchProvider $provider, array $data = array()) {
    $this->provider = $provider;
    
    $this->uri      = $data['uri'];
    $this->label    = $data['label'];
    $this->title    = $data['title'];
    $this->hasBlock = $data['has_block'];

    // These are optional / have defaults.
    foreach (array('id', 'pageSize', 'sort', 'intro') as $key) {
      if (isset($data[$key])) {
        $this->{$key} = $data[$key];
      }
    }
  }
  
  /**
   * Tell the system if this search result is
   * able to supply AJAX ready content.
   * 
   * @return boolean
   *  Does this results handler support AJAX?
   */
  function hasAjax() {
    return FALSE;
  }
  
  /**
   * Tell rendering process if this result type
   * uses a pager for large result sets.
   * 
   * @return boolean
   *  Does this results handler have a pager?
   */
  function hasPager() {
    return TRUE;
  }
  
  // -------------------------
  // CRUD handling
  // -------------------------

  /**
   * Save the search results instance.
   */
  function save() {
    $data    = get_object_vars($this);    
    $allowed = get_class_vars(get_called_class());

    $data   = array_intersect_key($data, $allowed);
    $schema = drupal_get_schema('iwsearch_results');
    
    $extra = array_diff_key($data, $schema['fields']);
    $data  = array_intersect_key($data, $schema['fields']);
    
    // Non trivial items.
    $data['name']      = $this->provider->getName();
    $data['handler']   = get_called_class();
    $data['data']      = serialize($extra);
    $data['has_ajax']  = (int)$this->hasAjax();
    $data['has_block'] = empty($this->hasBlock) ? 0 : 1;
    
    if (empty($this->id)) {
      $this->id = db_insert('iwsearch_results', array('return' => Database::RETURN_INSERT_ID))
        ->fields($data)
        ->execute();
    }
    else {
      db_update('iwsearch_results')
        ->fields($data)
        ->condition('id', $this->id)
        ->execute();
    }
 
    drupal_static_reset('iwsearch_result_load');
    menu_rebuild();
  }
  
  /**
   * Delete the search results instance.
   */
  function delete() {
    db_delete('iwsearch_results')
      ->condition('id', $this->id)
      ->execute();
 
    drupal_static_reset('iwsearch_result_load');
    menu_rebuild();
  }


  // ----------------------------------
  // Faciliate sorting
  // ----------------------------------
  
  /**
   * Used to tell the search provider what additional
   * information we might need in addition to the
   * regular fields.
   * 
   * This is useful for database generated fields like
   * distance on geo-location queries.
   * 
   * @return boolean
   */
  function additionalFields() {
    return FALSE;
  }
  
  /**
   * Reports if this result display expects and will
   * utilize text highlighting.
   * 
   * @return boolean
   */
  function highlightFields() {
    return FALSE;
  }

  /**
   * Create the sort string to use for the query.
   * 
   * @param array $elements
   *  Get a list of the available elements. This could
   *  be an extended list, and not just the provider's set.
   * @param string $sort
   *  Suggested sort direction, which is ASC or DESC.
   * @return iwSearchValue
   *  Returns a loaded / built iwSearchValue object. This
   *  will provide search handlers with the ability to 
   *  build the sort string.
   */
  function getSortElement(array $elements, $name, $info) {
    if (isset($elements[$name])) {
      return $elements[$name];
    }
    elseif (empty($info['sortType']) || empty($info['indexKey'])) {
      if (!isset($this->avail)) {
        $this->avail = $this->provider->getAvailSort();
      }

      if (isset($this->avail[$name])) {
        $info = $this->avail[$name];
      }
      else {
        return NULL;
      }
    }
    
    return new iwSearchValue($info['indexKey'], $info['sortType']);
  }
  
  /**
   * The default method for getting the sort
   * 
   * @param array $elements
   *  An array of iwSearchElements used to construct the
   *  search. This is passed instead of using the
   *  $this->provider->elements because there can be extensions
   *  for the stored version for customized usage.
   * @return array
   *  An array of built sort strings. Usually combining them
   *  with a comma is all that is required but is kept separated
   *  here in case there are special needs based on implemenation.
   */
  function getSort(array $elements, $sorts = NULL) {
    $output   = array();
    $sorts    = isset($sorts) ? $sorts : $this->sort;
    $builders = _iwsearch_get_value_query_builders($this->provider->getSearchType());

    foreach ($sorts as $name => $info) {
      $sort = $this->getSortElement($elements, $name, $info);
      
      $info += array('dir' => 'ASC');
      if (isset($sort) && isset($builders[$sort->getSortType()]['sort'])) {
        $callback = $builders[$sort->getSortType()]['sort'];
        $output[] = $callback($sort, $info['dir']);
      }
    }

    return $output;
  }
  
  
  // ---------------------------------
  // Formatting results
  // ---------------------------------
  
  /**
   * Transform the results of the search into formatted
   * entities. All sub classes will configure and
   * display the results a bit differently.
   * 
   * @param array $results
   *  The results of a search, should at least
   *  contain the entity ID.
   * @result array
   *  An array of renderable arrays. Each will be
   *  used to render the view of a result.
   */
  abstract function formatResults(array $results);
  
  
  // ----------------------------------
  // iwIConfigurable
  // ----------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $element['uri'] = array(
      '#type' => 'textfield',
      '#title' => t('URI'),
      '#field_prefix' => 'http://'. $_SERVER['HTTP_HOST'] .'/',
      '#required' => TRUE,
      '#element_validate' => array('iwsearch_element_validate_result_path'),
      '#default_value' => $this->uri,
      '#weight' => -20,
    );
    
    $element['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Page title'),
      '#required' => TRUE,
      '#default_value' => $this->title,
      '#description' => t('This will be used as the results page title.'),
      '#weight' => -10,
    );
    
    $element['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#required' => TRUE,
      '#default_value' => $this->label,
      '#description' => t('Internal label used for identifying this search result.'),
      '#weight' => -10,
    );
      
    $element['intro'] = array(
      '#type' => 'text_format',
      '#title' => t('Introduction text'),
      '#format' => $this->intro['format'],
      '#default_value' => $this->intro['value'],
    );
    
    $element['pageSize'] = array(
      '#type' => 'textfield',
      '#title' => t('Page size'),
      '#size' => 8,
      '#field_suffix' => t('items/page'),
      '#default_value' => $this->pageSize,
      '#element_validate' => array('element_validate_integer_positive'),
      '#attributes' => array('class' => array('number-value')),
      '#weight' => -5,
    );
    
    $element['hasBlock'] = array(
      '#type' => 'checkbox',
      '#title' => t('Has search block'),
      '#default_value' => $this->hasBlock,
      '#weight' => -4,
    );
    
    $element['sort'] = array(
      '#type' => 'table_orderable',
      '#title' => t('Default sorting'),
      '#prefix' => '<div id="config-sort-fields">',
      '#suffix' => '</div>',
      '#header' => array(t('Field'), t('Sort'), t('Actions')),
      '#description' => t('Configure how the search results are ordered.'),
    );
    
    $ajax = array(
      'wrapper' => 'config-sort-fields',
      'callback' => '_iwapi_ajax_configurable_callback',
      'ref_element' => array(
        'type' => 'root',
        'value' => array_merge($element['#array_parents'], array('sort'))
      ),
    );
    
    $avail = $this->provider->getAvailSort();
    foreach (array_intersect_key($this->sort, $avail) as $name => $info) {
      $element['sort'][$name] = array(
        'field' => array(
          '#markup' => check_plain($info['label']),
        ),
        'dir' => array(
          '#type' => 'select',
          '#options' => array(
            'ASC'  => t('ascending'),
            'DESC' => t('descending'),
          ),
          '#default_value' => $info['dir'],
        ),
        'actions' => array(
          '#type' => 'actions',
          
          'remove' => array(
            '#type'  => 'submit',
            '#name'  => 'rm-sort-'. strtr($name, '_', '-'),
            '#value' => t('Remove'),
            '#ajax'  => $ajax,
          ),
        ),
      );
    }
    
    $diff = array_diff_key($avail, $this->sort);
    if (!empty($diff)) {
      unset($form_state['input']['sort']['__add']);
      
      $element['sort']['__add'] = array(
        'field' => array(
          '#type' => 'select',
          '#empty_option' => t('- Select a field -'),
          '#options' => _iwapi_extract_field($diff),
        ),
        'dir' => array(
          '#type' => 'select',
          '#options' => array(
            'ASC'  => t('ascending'),
            'DESC' => t('descending'),
          ),          
        ),
        'actions' => array(
          '#type' => 'actions',
          
          'add' => array(
            '#type'  => 'submit',
            '#name'  => 'add-sort-field',
            '#value' => t('Add Sort'),
            '#ajax'  => $ajax,
          ),
        ),
      );
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
    $trigger = $form_state['triggering_element'];

    if ($trigger['#name'] == 'add-sort-field') {
      if (empty($values['sort']['field'])) {
        form_error($element['sort']['__add'], t('Select a field value to add.'));
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    $trigger = $form_state['triggering_element'];
 
    switch ($trigger['#value']) {
      case t('Remove'):
        if (preg_match('#^rm-sort-(.*)$#', $trigger['#name'], $matches)) {
          $key = strtr($matches[1], '-', '_');
          unset($this->sort[$key]);
          
          $form_state['rebuild'] = TRUE;
        }
        return; // Done, rebuild form.
        
      case t('Add Sort'):
        $form_state['rebuild'] = TRUE;
        break;
      default:
        $this->uri      = $values['uri'];
        $this->title    = $values['title'];
        $this->label    = $values['label'];
        $this->intro    = $values['intro'];
        $this->pageSize = $values['pageSize'];
        $this->hasBlock = $values['hasBlock'];
    }
    
    $sort  = array();
    $avail = $this->provider->getAvailSort();
    foreach ($values['sort'] as $name => $data) {
      if ($name == '__add' && !empty($data['field'])) {
        $name = $data['field'];
      }
      
      if (isset($avail[$name])) {
        $sort[$name] = $avail[$name];
        $sort[$name]['dir'] = $data['dir'];
      }
    }
    
    // Save the built sort array back into the sort property.
    $this->sort = $sort;
  }
}


/**
 * Results are shown as a rendered entity, with a specified view mode.
 */
class iwEntityViewResults extends iwSearchResults {

  public $viewMode = 'teaser';
  
  /**
   * Initiate a results handler.
   * 
   * @param iwSearchProvider $provider
   *  Search provider object which can create the search query.
   * @param array $data
   *  Array of values to get the properties of this
   *  results handler.
   */
  function __construct(iwSearchProvider $provider, array $data = array()) {
    parent::__construct($provider, $data);
    
    if (isset($data['viewMode'])) {
      $this->viewMode = $data['viewMode'];
    }
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::appendElements().
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $entityType  = $this->provider->getEntityType();
    $entity_info = entity_get_info($entityType);
    
    $element['viewMode'] = array(
      '#type' => 'select',
      '#title' => t('View mode'),
      '#required' => TRUE,
      '#options' => _iwapi_extract_field($entity_info['view modes']),
      '#default_value' => $this->viewMode,
    );
    
    parent::appendElements($element, $form, $form_state);
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::submitElements().
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
    
    $this->viewMode = $values['viewMode'];
  }
  
  /**
   * Format the results using a basic view mode
   * available for the entity in question.
   * 
   * @see iwSearchResults::formatResults()
   */
  function formatResults(array $results) {
    $ids = _iwapi_extract_field($results['entities'], 'entity_id');
    
    $formatted  = array();
    $entityType = $this->provider->getEntityType();
    $entities   = entity_load($entityType, $ids);
    $first      = reset($entities);
    
    // Check if this entity has a view function. This will be supported
    // by either iwEntities or entities used by the Entity API module.
    if (method_exists($first, 'view')) {
      foreach ($entities as $id => $entity) {
      	$formatted[$id] = $entity->view($this->viewMode);
      }
    }
    else {
      $callback = $entityType .'_view';
      
      // Following a general convention for entity view functions.
      if (function_exists($callback)) {
        foreach ($entities as $id => $entity) {
          $formatted[$id] = $callback($entity, $this->viewMode);
        }
      }
    }

    $formatted['#sorted'] = TRUE;
    return $formatted;
  }
}
