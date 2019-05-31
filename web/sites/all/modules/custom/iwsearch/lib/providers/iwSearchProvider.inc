<?php
/**
 * @file
 * Base class for the standard search provider class.
 */


/**
 * Base class for managing a search configuration
 * and execution.
 * 
 * It also implements iwPackage which means that it
 * can be stored and prepared for repeated use.
 */
abstract class iwSearchProvider implements iwIConfigurable {
  
  /**
   * Get the entity types supported by this provider.
   */
  static function GetEntityTypes() {
    static $types;
    
    if (!isset($types)) {
      $entity_info = entity_get_info();
      $types['node'] = $entity_info['node'];  // Always provide at least 'node' types.
      
      // Find all the types that are included by the iwsearch module.
      $types += array_intersect_key($entity_info, _iwsearch_get_search_entities());
      $types = _iwapi_extract_field($types);
    }
    
    return $types;
  }
  
  /**
   * Transform the full database table row 
   * into a provider instance.
   * 
   * @param array $row
   *  An array populated with all the data used
   *  to construct a new build provider item.
   * @return iwSearchProvider
   *  Return a fully loaded search provider.
   */
  static function BuildProvider(array $row) {
    if (class_exists($row['provider'], TRUE)) {
      $className = $row['provider'];
      $row += unserialize($row['data']);

      return new $className($row);
    }

    return NULL;
  }
  

  // --------------------------
  // Instance variables
  // --------------------------

  // General properties.
  protected $name;
  protected $entityType;
  protected $bundles = array();  // An array of bundles searched.
  
  public $label = '';      // Friendly name of the instance.
  
  // Handling of search elements.
  public $elements = array();


  // --------------------------
  // Instance methods
  // --------------------------
  
  /**
   * Initialize and create a new instance
   * of this search provider.
   * 
   * @param array $data
   *  Loadable data definitions to pull all
   *  the provider information from.
   */
  function __construct(array $data = array()) {
    $vars = get_class_vars(get_called_class());
    
    // populate the instance variables.
    foreach (array_keys($vars) as $key) {
      if (isset($data[$key])) {
        $this->{$key} = $data[$key]; // populate local values.
      }
    }
  }
  
  /**
   * Report back what type of search backend is
   * supported by this provider. Will help to
   * determine which sort and mappings are available.
   */
  abstract function getSearchType();
  
  /**
   * Getter function to get the name of the
   * search instance.
   * 
   * @return string
   *  Machine name of this search instance.
   */
  function getName() {
    return $this->name;
  }
  
  /**
   * Getter function to get the machine
   * name of the entity type provided by
   * this search instance.
   * 
   * @return string
   *  Machine name of the entity type.
   */
  function getEntityType() {
    return $this->entityType;
  }
  
  /**
   * Return the list of bundles that are to be
   * included within these search results.
   * 
   * @return array
   *  The machine name of bundles used by 
   *  this search instance.
   */
  function getBundles() {
    return $this->bundles;
  }
  
  /**
   * Indicate if this provider requires some local 
   * index tracking. Will be stored so CRON can know
   * which providers need to be checked for updates.
   * 
   * @return boolean
   *  TRUE if this search provider needs to
   *  update and maintain local search indexes.
   */
  function hasLocalIndex() {
    return $this instanceof iwIndexedSearchProvider;
  }
  
  /**
   * Do any house keeping required before working
   * with this search provider.
   */
  function init() {
    foreach ($this->elements as $element) {
      $element->provider = $this;
    }
  }
  
  /**
   * Save the current search instance to the database.
   * 
   * This will convert class properties as needed,
   * and generate some missing information.
   */
  function save() {
    $provider = get_called_class();
    
    $data = array_intersect_key(get_object_vars($this), get_class_vars($provider));
    unset($data['name'], $data['label']);
    
    $values = array(
      'name' => $this->name,
      'label' => $this->label,
      'provider' => get_class($this),
      'local_index' => (int)$this->hasLocalIndex(),
      'last_indexed' => 0,
    );
    $values['data'] = serialize(array_diff_key($data, $values));
    
    // Save the instance information, insert or update.
    db_merge('iwsearch_instances')
      ->key(array('name' => $this->name))
      ->fields($values)
      ->execute();

    $this->resetCache();
  }
  
  /**
   * Delete the this search instance from the database.
   */
  function delete() {
    $trans = db_transaction();
    
    try {
      // Remove from the database.
      db_delete('iwsearch_instances')
        ->condition('name', $this->name)
        ->execute();
    
      db_delete('iwsearch_results')
        ->condition('name', $this->name)
        ->execute();

      $this->resetCache();
    }
    catch(Exception $e) {
      $trans->rollback();
      throw $e;
    }
  }


  // -------------------------------
  // Protected utilities.
  // -------------------------------
  
  /**
   * Function to reset the iwsearch instance and related
   * items so that they can be regenerated later if needed.
   */
  protected function resetCache() { 
    // Reset the instances cache.
    $instances = &drupal_static('iwsearch_instance_load');
    unset($instances[$this->name]);
    
    // Clear any cached field mappings.
    cache_clear_all("iwsearch_fmaps:{$this->name}", 'cache');
  }


  // -------------------------------
  // Form generation and handling
  // -------------------------------
  
  /**
   * Generate the field filters and core elements of
   * the search form. 
   * 
   * @param array $values
   *  The current values of the search form.
   * @param array $form_state
   *  The full form state of the current form.
   * @return array
   *  The form structure definition for the search
   *  criteria fields and data.
   */
  function createForm(array $values, array &$form_state) {
    $form = array('#auto_submit' => array());
    
    $this->init();
    foreach ($this->elements as $name => $element) {
      $default = isset($values[$name]) ? $values[$name] : NULL;
      $form[$name] = $element->attachForm($form_state, $default);
      
      // Record the list of items which auto submit to the calling function,
      // it will be responsible for managing auto submit behavoir.
      if (!empty($element->autoSubmit)) {
        $form['#auto_submit'][] = $name;
      }
    }
    return $form;
  }
  
  /**
   * Validate the search form to ensure no invalid values get in.
   * 
   * @param array $form
   *  The form structure and elements containing the form elements.
   * @param array $values
   *  Reference to the values and current status of the form.
   * @param array $state
   *  The full $form_state array, so that we are able to change
   *  or do some special handling.
   */
  function attachValidate(array $form, array &$values, array &$state) {
    // Nothing to validate at this layer.
  }
  
  /**
   * Transform the search criteria into valid usable search query.
   * 
   * @param object $query
   *  Query object, which is being built to execute the search.
   * @param array $elements
   *  array of iwSearch elements which will be used to build
   *  the search. This can be a temporary extended set, which
   *  is why we don't use $this->elements.
   * @param array $criteria
   *  The search values to use and apply to the search.
   * @return array
   *  An array of search summary string values. These
   *  can be used to render the search criteria used.
   */
  protected function buildQuery($query, array $elements, array $criteria) {
    $summary  = array();
    $builders = _iwsearch_get_value_query_builders($this->getSearchType());
    
    $this->init();
    foreach ($elements as $key => $element) {
      $filterVal = isset($criteria[$key]) ? $criteria[$key] : NULL;
      $filterVal = $element->translateValue($filterVal);
    
      // Only apply the filter if the value is not NULL and there is a
      // proper value builder for this search provider type.
      if (isset($filterVal) && isset($builders[$element->getValueType()])) {
        $summary[] = $element->filterString($filterVal);
    
        $func = $builders[$element->getValueType()]['query'];
        $func($query, $element, $filterVal);
      }
    }
    
    return $summary;
  }
  
  /**
   * Execute the search with provided search criteria.
   * 
   * @param array $criteria
   *  Filter criteria that should be used when
   *  executing the underlying search handler.
   * @param array $settings
   *  Provides settings relating to how the results should
   *  be displayed. This is the number of items per page,
   *  the page, field projection and sorting information.
   * 
   *  This allows the separation from search provider and
   *  results page settings. Format should follow
   *  iwSearchResults::getSearchSettings().
   * 
   * @return array
   *  Contains indexes for summary and entities returned
   *  by the search. The summary is an array of criteria
   *  conditions used in the search. The entities index
   *  is an array of objects with a property entity_id.
   * @see iwSearchResults::getSearchSettings()
   */
  abstract function execute(array $criteria, array $settings);
  

  // -------------------------------
  // iwIConfigurable
  // -------------------------------
  
  /**
   * Get the search fields that are available. It
   * is the responsibility of the provider to determine
   * what fields / properties it is able to use.
   *  
   * @return array
   *  Return an array keyed by the field name, and contains
   *  information used by the provider to construct
   *  new search field elements of these types.
   */
  abstract protected function getAvailFields();
  
  /**
   * Get an array containing all the filtering element
   * options available for this search provider and
   * set of bundles passed in the $bundles parameter.
   * 
   * @return array
   *  An array keyed by field name and containing
   *  the field element information.
   */
  function getAvailFilters() {
    return array(
      'keywords' => array(
        'fieldName' => t('Keywords'),
        'label'     => t('Keywords'),
        'types'     => array('iwSearchKeyword' => t('Textbox entry')),
      ),
      'bundles' => array(
        'fieldName' => t('Bundles'),
        'label'     => t('Bundles'),
        'types'     => array('iwSearchBundles' => t('Bundle options')),
        'allowedBundles' => $this->bundles,
      ),
    ) + $this->getAvailFields();
  }
  
  /**
   * Get information about how fields can be sorted.
   * 
   * @return array
   *  Information about fields which are 
   *  available for sorting.
   */
  function getAvailSort() {
    $fields = $this->getAvailFields();
    
    foreach ($fields as $fieldname => $info) {
      if (empty($info['sortType']) || $info['multiple']) {
        unset($fields[$fieldname]);
      }
    }
    
    return array(
      'label' => array(
        'indexKey' => 'label',
        'label'    => t('Entity label (title)'),
        'sortType' => 'label',
      ),
      'bundle' => array(
        'indexKey' => 'bundle',
        'label'    => t('Bundle name'),
        'sortType' => 'bundle',
      ),
    ) + $fields;
  }
  
  /**
   * Generate the elements which will dictate how
   * the search will behave.
   * 
   * @param array $fields
   *  Reference to the form elements to insert the
   *  additions and make modifications to.
   * @param array $values
   *  The current form / state values.
   * @param array $form
   *  The full form definition.
   */
  protected function configureElements(array &$elements, array $values, array &$form) {
    $available = $this->getAvailFilters($values['bundles']);
    $uri_base  = 'admin/config/search/iwsearch/'. strtr($this->name, '_', '-') .'/elements/nojs';
    
    // Bundles are special and always need to get updated.
    if (isset($this->elements['bundles'])) {
      $this->elements['bundles']->allowedBundles = $available['bundles']['allowedBundles'];
    }
    
    foreach ($this->elements as $fieldname => $element) {
      $field_uri = strtr($fieldname, '_', '-');
      
      // Build some helpful variable information.
      $field_parents = $elements['#parents'];
      $field_parents[] = $fieldname;
      $element_name = 'update-'. implode('-', $field_parents);
      
      $elements[$fieldname] = array(
        'field' => array('#markup' => $fieldname),
        'type-wrapper' => array(
          'type' => array(
            '#type' => 'select',
            '#required' => TRUE,
            '#options' => $available[$fieldname]['types'],
            '#default_value' => get_class($element),
            '#parents' => array_merge($field_parents, array('type')),
            '#ajax' => array(
              'callback' => '_iwsearch_update_element_field_type_ajax',
              'trigger_as' => array('name' => $element_name),
              'progress' => 'none',
            ),
          ),
          'update' => array(
            '#type' => 'submit',
            '#value' => t('Update'),
            '#submit' => array('_iwsearch_update_element_field_type'),
            '#name' => $element_name,
            '#limit_validation_errors' => array($field_parents),
            '#attributes' => array('class' => array('element-invisible')),
            '#ajax' => array(
              'callback' => '_iwsearch_update_element_field_type_ajax',
              'progress' => 'none',
            ),
          ),
        ),
        'summary' => array(
          '#cell_attributes' => array('class' => array('iwpackage-summary')),
          'data' => $element->formatSummary(),
        ),
        'actions' => array(
          '#theme' => 'links',
          '#attributes' => array('class' => array('inline')),
          '#links' => array(
            'edit' => array(
              'title' => t('Edit'),
              'href' => "$uri_base/$field_uri/edit",
              'attributes' => array('class' => array('use-ajax')),
            ),
            'delete' => array(
              'title' => t('Delete'),
              'href' => "$uri_base/$field_uri/delete",
              'attributes' => array('class' => array('use-ajax')),
            ),
          ),
        ),
      );
    }

    // Allow the addition of new fields. If any are available.
    $remaining = array_diff_key($available, $elements);
    if (!empty($remaining)) {
      $widgets = array();
    
      if (isset($values['elements']['__add_new']['field'])) {
        $fieldname = $values['elements']['__add_new']['field'];
    
        // What widget / element type handles this.
        if (isset($remaining[$fieldname]['types'])) {
          $widgets = $remaining[$fieldname]['types'];
        }
      }
    
      // Provide the option to add more elements.
      $elements['__add_new'] = array(
        'field' => array(
          '#type' => 'select',
          '#prefix' => '<div class="add-new-placeholder">'. t('Add search field') .'</div>',
          '#empty_option' => t('- Select a field -'),
          '#options' => _iwapi_extract_field($remaining),
          '#ajax' => array(
            'wrapper' => 'iwsearch-add-field-type',
            'callback' => '_iwapi_ajax_configurable_callback',
            'ref_element' => array('type' => 'sibling', 'value' => array('type')),
            'progress' => 'none',
          ),
        ),
        'type' => array(
          '#type' => 'select',
          '#prefix' => '<div id="iwsearch-add-field-type"><div class="add-new-placeholder">&nbsp</div>',
          '#suffix' => '</div>',
          '#empty_option' => t('- Select the widget -'),
          '#required' => !empty($widgets),
          '#disabled' => empty($widgets),
          '#options' => $widgets,
        ),
        'summary' => array(
          '#prefix' => '<div class="add-new-placeholder">&nbsp</div>',
          '#markup' => t('Add a new search field by selecting a field and widget type.'),
        ),
        'action' => array('#markup' => '&nbsp;'),
      );
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
    // Provide a default validate which does nothing. Most
    // resulting edit forms are very locked down and won't
    // need additional validation (outside of elements).
  }
  
  /**
   * Alter a field element handler type.
   * 
   * @param string $key
   *  Name of the field element which is being modified.
   * @param array $data
   *  Data used to update the field handler with.
   */
  function updateFieldElement($key, array $data) {
    $className = $data['type'];
    
    if (get_class($this->elements[$key]) !== $className) {
      $data += $this->elements[$key]->packData();
      $this->elements[$key] = new $className($data);
    }
  }
}
