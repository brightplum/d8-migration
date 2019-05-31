<?php
/**
 * @file
 * Contains the formatter which will format results into a table format.
 */

class iwTabularSearchResults extends iwSearchResults {

  public $header  = array();
  public $columns = array();

  /**
   * Apply previoulsy configured data.
   */  
  function __construct(iwSearchProvider $provider, array $data = array()) {
    parent::__construct($provider, $data);
    
    foreach (array('header', 'columns') as $key) {
      if (!empty($data[$key])) {
        $this->{$key} = $data[$key];
      }
    }
  }

  /**
   * non-PHPDOC
   * @see iwIConfigurable::appendElements().
   */
  public function appendElements(array &$element, array &$form, array &$form_state) {  
    parent::appendElements($element, $form, $form_state);

    $headers = _iwapi_extract_field($this->header);
    $defaults = array(
      'col' => array(
        '#type' => 'select',
        '#title' => t('Column'),
        '#empty_option' => t('- Unused -'),
        '#options' => $headers,
        '#attributes' => array('class' => array('field-parent')),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Order'),
        '#size' => 8,
        '#element_validate' => array('element_validate_integer_positive'),
        '#attributes' => array('class' => array('field-weight')),
      ),
    );
      
    // Gather all the additional add all property fields.
    $eType = $this->provider->getEntityType();
    foreach (iwapi_get_entity_properties($eType) as $name => $property) {
      $form['fields'][$name] = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-draggable')),

        'label' => array(
          '#prefix' => '<div class="item-handle"><span class="drag-icon"></span>',
          '#suffix' => '</div>',
          '#markup' => check_plain($property->label),
        ),
      ) + $defaults;
    }

    $fields = array();
    foreach ($this->provider->getBundles() as $bundle) {
      $fields += field_info_instances($eType, $bundle);
    }

    // Integrate available fields into table display options.
    foreach ($fields as $name => $field_info) {
      $table_data['fields'][$name] = array(
        'label' => $field_info['label'],
        'display' => $field_info['display']['default'],
        'cardinality' => 1,
      );
    }

    // Configure fields which are included.
    foreach ($this->header as $col => $header) {
      $weight = 0;

      // Set field values to the current settings.
      foreach ($this->columns[$col]['children'] as $name => $field) {
        if (isset($table_data['fields'][$name])) {
          if ($table_data['fields'][$name] instanceof iwEntityProperty) {
            $table_data['fields'][$name] = array(
              'title' => $field->property,
              'display' => (array)$field,
              'cardinality' => 1,
            );
            continue;
          }

          $table_data['fields'][$name] += array('column' => $col, 'weight' => $weight++);

            if (!empty($field->title)) {
              $table_data['fields'][$name]['title'] = $field->title;
            }
            if (!empty($field->settings)) {
              $table_data['fields'][$name]['display'] = $field->settings + $table_data['fields'][$name]['display'];
            }
            if (!empty($field->cardinal)) {
              $table_data['fields'][$name]['cardinality'] = $field->cardinal;
            }
          }
        } 
      }
    
    // Assumes that the $form_state is populated with the need
    // state information regarding columns and fields.    
    $element['display']['#parents'] = array_merge($element['#parents'], array('display'));
    $element['display']['#array_parents'] = array_merge($element['#array_parents'], array('display'));
 
    _iwapi_table_builder_form($element['display'], $form_state);
  }
  
  /**
   * non-PHPDOC
   * @see iwIConfigurable::submitElements().
   */
  public function submitElements(array $element, array &$values, array &$form_state) {
    parent::submitElements($element, $values, $form_state);
    
    $this->header  = $form_state['tables']['display']['header'];
    $this->columns = array_fill_keys(
      array_keys($this->header),
      array('children' => array())
    );
    
    // Iterate all the fields and apply their changes
    $eType      = $this->provider->getEntityType();
    $properties = iwapi_get_entity_properties($eType);
    
    uasort($values['setup']['display']['fields'], 'drupal_sort_weight');
    foreach ($values['setup']['display']['fields'] as $name => $info) {
      if (empty($info['column'])) {
        continue;
      }
      
      // Assign values to column.
      $column   = $info['column'];
      $settings = $form_state['tables']['display']['fields'][$name];
      if (isset($properties[$name])) {
        $classname = $properties[$name]['handler'];
        $this->columns[$column]['children'][$name] = new $classname($name, $settings);
      }
      else {
        $this->columns[$column]['children'][$name] = new iwFieldRenderer($name, $settings);
      }
    }
  }
  
  /**
   * non-PHPDoc
   * @see iwSearchResults::getSort().
   */
  public function getSort(array $elements, $sorts = NULL) {
    $output   = array();
    $builders = _iwsearch_get_value_query_builders($this->provider->getSearchType());
 
    $order = tablesort_get_order($this->header);
    
    // Use the default sort if table header doesn't provide anything.
    if (!isset($order['sql'])) {
      return parent::getSort($elements, $sorts);
    }
    else {
      // Sort based on the table header...
      $sort  = $this->getSortElement($elements, $order['sql'], array());
      if (isset($sort) && isset($builders[$sort->getSortType()]['sort'])) {
        $callback = $builders[$sort->getSortType()]['sort'];
        $output[] = $callback($sort, tablesort_get_sort($this->header));
      }

      return $output;
    }
  }
  
  /**
   * Translate a single entity into a single
   * row of content.
   * 
   * @param object $entity
   *  The loaded entity object.
   * @param stdClass $entry
   *  The fields returned by the search query.
   * @return array
   *  A rendered row of content.
   */
  protected function formatRow($entity, $entry) {
    $entityType = $this->provider->getEntityType();
    
    $content = array();
    $props   = iwapi_get_entity_properties($entityType);
    
    foreach ($this->columns as $name => $col) {
      $cell = array(
        '#type' => 'container',
        '#attributes' => array(),
        '#sorted' => TRUE,
      );

      foreach ($col['children'] as $prop_name => $renderer) {
        $cell[] = isset($props[$prop_name])
          ? $props[$prop_name]->view($entity, $entityType)
          : $renderer->view($entity, $entityType);
      }

      $content[] = array(
        'data' => render($cell),
        'class' => array('result-row', strtr($name, '_', '-')),
      );
    }

    return $content;
  }
  
  /**
   * non-PHPDOC
   * @see iwSearchResults::formatResults().
   */
  public function formatResults(array $results) {
    $rows = array();
    
    if (empty($this->columns) || empty($this->header)) {
      return array();
    }

    if (!empty($results['entities'])) {
      // Load the entities from the search results.
      $mapped = array();
      foreach ($results['entities'] as $result) {
        $ids[] = $result->entity_id;
        $mapped[$result->entity_id] = $result;
      }
      $entityType = $this->provider->getEntityType();
      $entities   = entity_load($entityType, $ids);

      entity_prepare_view($entityType, $entities);

      foreach ($entities as $id => $entity) {
        $rows[$id] = $this->formatRow($entity, $mapped[$id]);
      }
    }

    return array(
      '#theme' => 'table',
      '#header' => $this->header,
      '#rows' => $rows,
      '#empty' => t('There are currently no results matching this criteria.'),
    );
  }
}
