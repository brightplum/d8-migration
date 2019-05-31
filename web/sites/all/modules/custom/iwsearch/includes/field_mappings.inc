<?php
/**
 * @file
 * Contains search mapping functions.
 */


/**
 * 
 */
function _iwsearch_get_property_mapping() {
  $prop_mappings = &drupal_static(__FUNCTION__);
  
  if (!isset($prop_mappings)) {
    $cid = 'iwsearch:property_mappings';
    
    if ($cache = cache_get($cid)) {
      $prop_mappings = $cache->data;
    }
    else {
      $prop_mappings = module_invoke_all('iwsearch_property_mapping') + array(
       /*
        * TODO: Create keyword handler
        * 'iwAuthorProperty' => array(
          'types' => array(),
          'sortType' => 'values',
        ),
        */
        'iwTimestampProperty' => array(
          'types' => array('iwSearchDateValue' => t('Date ranges')),
          'sortType' => 'date_range',
        ),
        'iwIntegerProperty' => array(
          'types' => array(
            'iwSearchNumber' => t('Number input'), 
            'iwSearchRange' => t('Value range'),
          ), 
          'sortType' => 'values',
        ),
      );
      
      drupal_alter('iwsearch_property_mapping', $prop_mappings);
      cache_set($cid, $prop_mappings);
    }
  }

  return $prop_mappings;
}

/**
 * Get a listing of available field search element
 * handlers to build search pages / info.
 * 
 * @return array
 *  A mapping of handlers for different field types and
 *  what SearchElement handlers are available to used
 *  them. It is keyed by field type.
 */
function _iwsearch_get_field_mapping() {
  $field_mappings = &drupal_static(__FUNCTION__);
  
  // If a field mapping has not already been loaded,
  // build the data field mappings based on field type.
  // ----------------------------------------
  if (!isset($field_mappings)) {
    // Common sets of items.
    $dates    = array('iwSearchDate' => t('Date ranges'));
    $list_set = array('iwSearchAllowedList' => t('Use set lists'));
    $numbers  = array(
      'iwSearchNumber' => t('Number input'), 
      'iwSearchRange' => t('Value range'),
    );
  
    $cid = 'iwsearch:field_mappings';
    if ($cache = cache_get($cid)) {
      $field_mappings = $cache->data;
    }
    else {
      $field_mappings = module_invoke_all('iwsearch_field_mapping') + array(
        'list_boolean' => array(
          'types'    => array('iwSearchBoolean' => t('Checkbox')),
          'sortType' => 'bool',
        ),
        
        'list_text'    => array('types' => $list_set, 'sortType' => 'values'),
        'list_float'   => array('types' => $list_set + $numbers, 'sortType' => 'values'),
        'list_integer' => array('types' => $list_set + $numbers, 'sortType' => 'values'),
      
        'number_float'   => array('types' => $numbers, 'sortType' => 'values'),
        'number_integer' => array('types' => $numbers, 'sortType' => 'values'),
        'number_decimal' => array('types' => $numbers, 'sortType' => 'values'),

        'datetime'  => array('types' => $dates, 'sortType' => 'date_range'),
        'date'      => array('types' => $dates, 'sortType' => 'date_range'),
        'datestamp' => array('types' => $dates, 'sortType' => 'date_range'),
        
        // Taxonomy terms are special.
        'taxonomy_term_reference' => array(
          'types' => array('iwSearchTerm' => t('Taxonomy terms')),
          'sortType' => 'taxonomy_term',
        ),
        'entityreference' => array(
          'types' => array('iwSearchEntityRef' => t('Entity select')),
        ),
        'node_reference' => array(
          'types' => array('iwSearchNodeRef' => t('Node select')),
        ),
      );
      
      drupal_alter('iwsearch_field_mapping', $field_mappings);
      cache_set($cid, $field_mappings);
    }
  }
  return $field_mappings;
}

/**
 * Get the value query builders for a given search provider.
 * Every iwSearchElement has a value type, this function
 * will map out what callback to use, to translate that value
 * into the query for a specific provider.
 * 
 * @return array
 *  Information about callbacks which handle building the
 *  search query from the search provider type.
 */
function _iwsearch_get_value_query_builders($provider) {
  $builders = &drupal_static(__FUNCTION__);
  
  $default['solr'] = array(
    'label' => array(
      'sort'  => '_iwsearch_sort_solr_label',
    ),
    'keywords' => array(
      'query' => '_iwsearch_build_solr_keyword_query',
      'sort'  => '_iwsearch_sort_solr_score',
    ),
    'bundle' => array(
      'query' => '_iwsearch_build_solr_value_query',
      'sort' => '_iwsearch_sort_solr_bundle',
    ),
    'bool' => array(
      'query' => '_iwsearch_build_solr_value_query',
      'sort' => '_iwsearch_sort_solr_bool',
    ),
    'values' => array(
      'query' => '_iwsearch_build_solr_value_query',
      'sort' => '_iwsearch_sort_solr_value',
    ),
    'date_range' => array(
      'query' => '_iwsearch_build_solr_date_range_query',
      'sort' => '_iwsearch_sort_solr_value',
    ),
    'value_range' => array(
      'query' => '_iwsearch_build_solr_range_query',
      'sort' => '_iwsearch_sort_solr_value',
    ),
    'taxonomy_term' => array(
      'query' => '_iwsearch_build_solr_value_query',
      'sort' => '_iwsearch_sort_solr_taxonomy',
    ),
    'entityref' => array(
      'query' => '_iwsearch_build_solr_entityref_query',
    ),
    'node_reference' => array(
      'query' => '_iwsearch_build_solr_value_query',
    ),
    'random' => array(
      'sort' => '_iwsearch_sort_solr_random',
    ),
  );
  
  // See if a cached version is available.
  if (isset($default[$provider]) && !isset($builders[$provider])) {
    $cid = "iwsearch:qbuilders:$provider";
    
    if ($cache = cache_get($cid)) {
      $builders[$provider] = $cache->data;
    }
    else {
      $builders[$provider] = module_invoke_all('iwsearch_query_builder_info', $provider);
      $builders[$provider] += $default[$provider];
      
      cache_set($cid, $builders[$provider]);
    }
  }

  return $builders[$provider];
}


// ==================================
// Solr query builders
// ==================================

/**
 * Add the keywords search criteria in $value to the 
 * $query object. The $query is modified directly.
 * 
 * @param SolrBaseQuery $query
 *  The Solr search query instance to build the search
 *  parameters into.
 * @param iwSearchElement $element
 *  The element which handles this keyword field.
 * @param string
 *  The value (keywords) to add to the Solr search query.
 */
function _iwsearch_build_solr_keyword_query(SolrBaseQuery $query, iwSearchElement $element, $value) {
  $value .= ' '. $query->getParam('q');
  $query->addParam('q', $value);

  // If keywords were added, then add the spell check params.
  apachesolr_search_add_spellcheck_params($query);
  apachesolr_search_add_boost_params($query);
}

/**
 * A simple value query filter. Add the value as is.
 * 
 * @param SolrBaseQuery $query
 *  The Solr search query instance to build the search
 *  parameters into.
 * @param iwSearchElement $element
 *  The element which handles this keyword field.
 * @param string $value
 *  The value to use add to the search.
 */
function _iwsearch_build_solr_value_query(SolrBaseQuery $query, iwSearchElement $element, $value) {
  if ($element->multi) {
    switch (count($value)) {
      case 0: return; // no values, nothing to filter on.
      case 1:
        $value = '"'. reset($value) .'"'; // single value, don't complicate things for Solr
        break;
      default:
        $value = '("'. implode('" OR "', $value) .'")'; // Series of OR'd values.
    }
  }
  
  // Pass the value as a filter to the query.
  $query->addFilter($element->getIndexKey(), $value);
}

/**
 * Handles adding search criteria for value ranges.
 * 
 * @param SolrBaseQuery $query
 *  The query to build the Solr search request with.
 * @param iwSearchElement $element
 *  The element which represents the field being searched on.
 * @param array $value
 *  The range value to use in the search.
 */
function _iwsearch_build_solr_range_query(SolrBaseQuery $query, iwSearchElement $element, array $value) {
  if (empty($value[0])) {
    $value[0] = '*';
  }
    
  if (empty($value[1])) {
    $value[1] = '*';
  }

  $query->addFilter($element->getIndexKey(), "[$value[0] TO $value[1]]");
}

/**
 * Handles adding the date ranges into a Solr search query.
 * 
 * @param SolrBaseQuery $query
 *  The curren Solr query handler.
 * @param iwSearchElement $element
 *  The search element which handles this
 *  date element.
 * @param array $value
 *  The set of MIN and MAX values to use when constructing
 *  the query range.
 */
function _iwsearch_build_solr_date_range_query(SolrBaseQuery $query, iwSearchElement $element, array $value) {
  $value['min'] = empty($value[0]) ? '*' : apachesolr_date_iso(strtotime($value[0]));
  $value['max'] = empty($value[1]) ? '*' : apachesolr_date_iso(strtotime($value[1]));
  
  $query->addFilter($element->getIndexKey(), "[$value[min] TO $value[max]]");
}

/**
 * Add the entity reference filters to the Solr query.
 * 
 * @param SolrBaseQuery $query
 *  The current Solr query object, which will execute data.
 * @param iwSearchEntityRef $element
 *  The entity reference object used to manage
 *  this search field.
 * @param array $value
 *  The value being passed into the $query filter.
 */
function _iwsearch_build_solr_entityref_query(SolrBaseQuery $query, iwSearchEntityRef $element, $value) {
  $type = $element->getTargetType();
  
  if ($element->multi) {
    array_walk($value, '_iwapi_prefix_value', $type .':');
  }
  else {
    $value = $type .':'. $value;
  }
  
  _iwsearch_build_solr_value_query($query, $element, $value);
}


// ==================================
// Include sorting into the query
// ==================================

/**
 * Sort based on the keyword / the score value.
 * 
 * @param iwSearchElement $element
 *  The search element provider to handling sorting.
 * @param string $sort
 *  The sort direction to use with this sort type.
 * @return string
 *  The string used to sort based on this field.
 */
function _iwsearch_sort_solr_score(iwSearchValue $element, $sort) {
  return 'score '. $sort;
}

/**
 * The labelfield uses a different sort parameter
 * than most other sorts. It isn't the same as
 * the standard $solrKey.
 * 
 * @param iwSearchBundles $element
 *  The element used to build for the search.
 * @param string $sort
 *  Either 'asc' or 'desc' to indicate the sort.
 * @return string
 *  The sort string to use as the sort string.
 */
function _iwsearch_sort_solr_label(iwSearchValue $element, $sort) {
  return 'sort_label '. $sort;
}

/**
 * Sort based on the value. This will use the Solr index key to
 * determine what we are sorting on.
 * 
 * @param iwSearchElement $element
 *  The search element provider to handling sorting.
 * @param string $sort
 *  The sort direction to use with this sort type.
 * @return string
 *  The string used to sort based on this field.
 */
function _iwsearch_sort_solr_value(iwSearchValue $element, $sort) {
  // Remove the 'm' representing multiple, when sorting.
  $indexKey = preg_replace('/^(\w+)m_/', '$1s_', $element->getIndexKey());
  
  return $indexKey .' '. $sort;
}

/**
 * The bundle field uses a different sort parameter
 * than most other sorts. It isn't the same as
 * the standard $solrKey.
 * 
 * @param iwSearchBundles $element
 *  The element used to build for the search.
 * @param string $sort
 *  Either 'asc' or 'desc' to indicate the sort.
 * @return string
 *  The sort string to use as the sort string.
 */
function _iwsearch_sort_solr_bundle(iwSearchValue $element, $sort) {
  return 'bundle_name '. $sort;
}

/**
 * Sort based on the text names of the taxonomy names.
 * Taxonomy fields are a little interesting to sort,
 * since the index is different for matching and
 * the key specified by apachesolr_index_key().
 * 
 * @param iwSearchElement $element
 *  The search element provider to handling sorting.
 * @param string $sort
 *  The sort direction to use with this sort type.
 * @return string
 *  The string used to sort based on this field.
 */
function _iwsearch_sort_solr_taxonomy(iwSearchTermCheckboxes $element, $sort) {
  return 'ts_vid_'. $element->getTaxonomyVid() ."_names $sort";
}

/**
 * Mostly ignore the provided parameters and build
 * around own random sort variable. Parameters were
 * needed for uniformity of function prototypes.
 * 
 * @param iwSearchElement $element
 *  The search element provider to handling sorting.
 * @param string $sort
 *  The sort direction to use with this sort type.
 * @return string
 *  The string used to sort based on this field.
 */
function _iwsearch_sort_solr_random(iwSearchValue $element, $sort) {
  $seed = filter_input(INPUT_GET, 'ssrnd', FILTER_SANITIZE_NUMBER_INT);
  
  if (empty($seed)) {
    $params = $_GET;
    $params['ssrnd'] = rand(1, 20000);
    unset($params['q']);
    
    drupal_goto(current_path(), array('query' => $params));
  }
  
  return "random_{$seed} asc";  
}

/**
 * Apply a sort on a boolean value. The 'bm_' prefix
 * will cause an error, so convert it to a 'bs_'.
 * 
 * @param iwSearchBoolean $element
 *  The boolean element used with this search field.
 * @param string $sort
 *  The sorting direction to use ('asc' or 'desc').
 */
function _iwsearch_sort_solr_bool(iwSearchValue $element, $sort) {
  $solrKey = $element->getIndexKey();
  
  return preg_replace('/^bm_/', 'bs_', $solrKey) .' '. $sort;
}
