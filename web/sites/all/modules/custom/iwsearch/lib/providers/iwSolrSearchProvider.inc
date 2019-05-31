<?php
/**
 * @file
 * Contains the base Solr search provider implementation.
 */
    
// Translate the field information into iwsearch data.
require_once dirname(dirname(__DIR__)) .'/includes/field_mappings.inc';


class iwSolrSearchProvider extends iwSearchProvider {    

  /**
   * Get the entity types supported by this provider.
   */
  static function GetEntityTypes() {
    static $types;
    
    if (!isset($types)) {
      $types = array();

      foreach (entity_get_info() as $type => $info) {
        if (!empty($info['apachesolr']['indexable'])) {
          $types[$type] = $info['label'];
        }
      }
    }

    return $types;
  }


  // --------------------------------
  // Instance properties
  // --------------------------------

  protected $solrEnv; // The Solr search environment used for searching.


  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::getSearchType()
   */
  function getSearchType() {
    return 'solr';
  }

  /**
   * Get all the bundles that are available
   * in this search instance and ApacheSolr.
   * 
   * @return array
   *  Get an array of all bundle names, keyed by
   *  the bundle machine name.
   */
  function getAvailBundles() {
    if (!isset($this->allBundles)) {
      $entity_info   = entity_get_info($this->entityType);
      $avail_bundles = apachesolr_get_index_bundles($this->solrEnv, $this->entityType);
     
      $this->allBundles = array_intersect_key($entity_info['bundles'], array_flip($avail_bundles));
      array_walk($this->allBundles, '_iwapi_extract_array_field', 'label');
    }
 
    return $this->allBundles;
  }


  // --------------------------------
  // Search functionality
  // --------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::execute()
   */
  function execute(array $criteria, array $settings) {      
    require_once drupal_get_path('module', 'iwsearch') .'/includes/field_mappings.inc';

    $results = array('count' => 0, 'summary' => array());
    $params  = array(
      'fl'   => empty($settings['fields']) ? array('score') : $settings['fields'],
      'rows' => $settings['pageSize'],
    );
    array_unshift($params['fl'], 'entity_id'); // This is required and will always be added.  
    array_unshift($params['fl'], 'id'); // This is required and will always be added. 

    // Start building the Solr Query
    if ($solr = apachesolr_get_solr($this->solrEnv)) {
      $params += apachesolr_search_basic_params();
      if (!empty($settings['highlight'])) {
        $params['hl'] = 'true';
        $params['hl.fl'] = $settings['highlight'];
      }

      // These are properties of the instance, and will always be added.
      $solrQuery = apachesolr_drupal_query('apachesolr', $params, '', '', $solr);
      $solrQuery->addFilter('entity_type', $this->entityType);
      
      if (!empty($this->bundles)) {
        if (isset($this->elements['bundles']) && !empty($criteria['bundles'])) {
          $element = $this->elements['bundles'];
          $bundles = $element->translateValue($criteria['bundles']);
          unset($criteria['bundles']);
        }
        $bundles = isset($bundles) ? $bundles : $this->bundles;
        $filter = is_array($bundles) ? '(' . implode(' OR ', $bundles) .')' : $bundles;
        $solrQuery->addFilter('bundle', $filter); 
      }
      $solrQuery->page = isset($settings['page']) ? $settings['page'] : pager_find_page();

      // If extension elements added, they get priority, and have their
      // filters applied as if they were a regular part of this provider.
      $elements = $this->elements;
      unset($elements['bundles']);
      
      // Apply the search criteria to the elements.
      $results['summary'] = $this->buildQuery($solrQuery, $elements, $criteria);
      
      if (!empty($settings['filters'])) {
        $this->addFilters($solrQuery, $settings['filters']);
      }
      
      if (!empty($settings['sort'])) {
        $solrQuery->addParam('sort', implode(',', $settings['sort']));
      }

      // Make the final Apache Solr request.
      list($final_query, $resp) = apachesolr_do_query($solrQuery);
      apachesolr_has_searched($this->solrEnv, TRUE);

      // Check the status and return the results.
      if (intval($resp->code) === 200) {
        $results['count']     = $resp->response->numFound;
        $results['entities']  = $resp->response->docs;
        $results['highlight'] = isset($resp->highlighting) ? $resp->highlighting : FALSE;
      }

      $results['summary'] = array_filter($results['summary']);
    }

    return $results;
  }

  /**
   * Add some additional search parameters that
   * don't have a corresponding filter element.
   * 
   * @param SolrBaseQuery $query
   *  The solr query to alter. Filters will get
   *  added one by one to this search type.
   * @param array $filters
   *  An array with the filters to add. These
   *  are elements that get added without a 
   *  matching search element.
   */
  protected function addFilters(SolrBaseQuery $query, array $filters) {
    // Apply raw filters, from the query extension.
    foreach ($filters as $solrKey => $value) {
      $exclude = FALSE;
    
      // Ensure we handle the add correctly.
      if (preg_match('/^-(.*)/', $solrKey, $matches)) {
        $exclude = TRUE;
        $solrKey = $matches[1];
      }
      $query->addFilter($solrKey, $value, $exclude);
    }
  }


  // ---------------------------------
  // Configure Instance
  // ---------------------------------

  /**
   * Returns the filter / sort information about
   * about the properties available to this entity type.
   * 
   * @staticvar type $propMappings
   * @return array
   *  An array of properties supported by this
   *  search provider, keyed by the property name.
   */
  protected function addPropertyMappings() {
    static $propMappings;
    
    if (!isset($propMappings)) {
      $propMappings = _iwsearch_get_property_mapping();
    }
    
    $properties = array();
    foreach (iwapi_get_entity_properties($this->entityType) as $propName => $property) {
      $classType = get_class($property);
      
      if (($searchInfo = $property->getSearchInfo()) && key_exists($classType, $propMappings)) {
        $properties[$propName] = array(
          'fieldName' => $propName,
          'label' => $property->label,
          'indexKey' => apachesolr_index_key($searchInfo),
          'multiple' => $searchInfo['multiple'],
        ) + $propMappings[$classType];
      }
    }
    
    return $properties;
  }
  
  /**
   * Generate a field mapping entry. This is used to 
   * build information needed by the Solr provider to 
   * faciliate searching with this field. Usually
   * called by iwSolrSearchProvider::getAvailFields().
   * 
   * @param array $field
   *  Field information as returned by field_info_field().
   * @param array $solr_info
   *  Information about Solr field mapping, this data is
   *  generated by HOOK_apachesolr_field_mappings().
   * @param array $fieldData
   *  Current master list of built field data to add this
   *  entry to. If a valid mapping is found, it will be
   *  added to this data array.
   */
  protected function addFieldMapping(array $field, array $instance, array $solr_info, array &$fieldData) {    
    static $fieldMapping;

    if (!isset($fieldMapping)) {
      $fieldMapping = _iwsearch_get_field_mapping();
    }
    
    $fieldname = $field['field_name'];
    $solr_info['name'] = $fieldname;
    $solr_info += array(
      'dependency plugins' => array('bundle', 'role'), 
      'map callback' => FALSE, 
      'name callback' => '', 
      'hierarchy callback' => FALSE, 
      'indexing_callback' => '', 
      'index_type' => 'string', 
      'facets' => FALSE, 
      'facet missing allowed' => FALSE, 
      'facet mincount allowed' => FALSE,
      'multiple' => TRUE, // Field API allows any field to be multi-valued.
    );
 
    if (!empty($fieldMapping[$field['type']]['types'])) {
      $fieldData[$fieldname] = array(
        'fieldName' => $fieldname,
        'label'     => $instance['label'],
        'indexKey'  => apachesolr_index_key($solr_info),
        'multiple'  => $solr_info['multiple'],
      ) + $fieldMapping[$field['type']];
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::getAvailableElements()
   */
  function getAvailFields() {
    static $solrMap = NULL;
    static $cache = array();
    static $fieldData = array();
    
    $available = $this->addPropertyMappings();
    $bundles   = empty($bundles) ? $this->bundles : $this->getAvailBundles();
    
    // Fields are handled by bundle so that other searches can benefit from this caching.
    foreach (array_keys($bundles) as $bundle) {
      if (!isset($cache[$bundle]) && $fields = field_info_instances($this->entityType, $bundle)) {
        $include = array();
        $cache[$bundle] = array();
  
        // Only find fields not already found and generated.
        $fields = array_diff_key($fields, $fieldData);
        
        if (!isset($solrMap)) {
          $solrMap = apachesolr_get_field_mappings($this->entityType);
        }
  
        // These fields are recognized by name.
        if (!empty($solrMap['per-field'])) {
          foreach (array_intersect_key($fields, $solrMap['per-field']) as $fieldname => $field_inst) {
            $field = field_info_field_by_id($field_inst['field_id']);
            
            $this->addFieldMapping($field, $field_inst, $solrMap['per-field'][$fieldname], $fieldData);
          }
        }

        // Find the rest of the available fields by field type.
        foreach (array_diff_key($fields, $include, $fieldData) as $fieldname => $field_inst) {
          $field = field_info_field_by_id($field_inst['field_id']);
          
          if (isset($solrMap[$field['type']])) {
            $this->addFieldMapping($field, $field_inst, $solrMap[$field['type']], $fieldData);
          }
        }
        
        $cache[$bundle] = array_intersect_key($fieldData, $fields);
      }

      $available += $cache[$bundle];
    }

    return $available;
  }
  
  /**
   * non-PHPDOC
   * @see iwSearchProvider::getAvailSort().
   */
  function getAvailSort() {
    return array(
      'random' => array(
        'indexKey'  => 'random_1234',
        'label'     => t('Random'),
        'sortType' => 'random',
      ),
    ) + parent::getAvailSort();
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    $values  = array();
    $parents = $element['#parents'];
      
    // Current state / values set for the form.
    $values = isset($form_state['values']) ? drupal_array_get_nested_value($form_state['values'], $parents) : array();
    
    // Apply default values to fill in the blanks.
    $values += array(
      'solrEnv'    => $this->solrEnv,
      'entityType' => $this->entityType,
      'bundles'    => $this->bundles,
    );

    $ajax = array(
      'wrapper' => 'iwsearch-bundles-select',
      'callback' => '_iwapi_ajax_configurable_callback',
      'ref_element' => array('type' => 'sibling', 'value' => array('bundles')),  
    );
    
    // Get available Solr Environments as a search source.
    $env = db_select('apachesolr_environment', 'env')
      ->fields('env', array('env_id', 'name'))
      ->execute()->fetchAllKeyed();
    
    $element['solrEnv'] = array(
      '#type' => 'select',
      '#title' => t('Solr Environment'),
      '#required' => TRUE, 
      '#empty_option' => t('- Select Solr Environment -'),
      '#empty_value' => '',
      '#options' => $env,
      '#default_value' => $values['solrEnv'],
      '#parents' => array_merge($parents, array('solrEnv')),
      '#ajax' => $ajax,
    );
    
    $element['entityType'] = array(
      '#type' => 'select',
      '#title' => t('Entity type'),
      '#required' => TRUE,
      '#empty_option' => t('- Select a type -'),
      '#empty_value' => '',
      '#options' => static::GetEntityTypes(),
      '#default_value' => $values['entityType'],
      '#ajax' => $ajax,
    );
    
    $element['bundles'] = array(
      '#type' => 'container',
      '#attributes' => array('id' => 'iwsearch-bundles-select'),
    );

    // If an environment was selected, we can determine what bundles
    // and therefore what fields are available for searching.
    if (!(empty($values['solrEnv']) || empty($values['entityType']))) {
      $this->solrEnv    = $values['solrEnv'];
      $this->entityType = $values['entityType'];

      $element['bundles'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Bundles'),
        '#prefix' => '<div id="iwsearch-bundles-select">',
        '#suffix' => '</div>',
        '#options' => $this->getAvailBundles(),
        '#default_value' => $values['bundles'],
        '#description' => t(
          'Select bundles that should be included in the results. If no '.
          'bundles are selected then all listed bundles will be included.'
        ),
      );
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    $this->label   = $values['label'];
    $this->name    = $values['name'];
    $this->solrEnv = $values['solrEnv'];
    $this->entityType = $values['entityType'];
    $this->bundles = array_filter($values['bundles']);
    
    $bundles = $this->bundles;
    if (empty($bundles)) {
      $bundles = array_keys($values['bundles']);
      $bundles = array_combine($bundles, $bundles);
    }

    // Update to only include available search elements.
    $this->elements = array_intersect_key($this->elements, $this->getAvailFilters());
  }
}
