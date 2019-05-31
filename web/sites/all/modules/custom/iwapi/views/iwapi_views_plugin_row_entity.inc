<?php
/**
 * @file
 * Row style plugin for displaying the results as entities.
 */

/**
 * Plugin class for displaying Views results with entity_view.
 */
class iwapi_views_plugin_row_entity extends views_plugin_row {

  protected $entityType;
  protected $entities;

  /**
   * (non-PHPdoc)
   * @see views_plugin_row::init()
   */
  public function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);

    // Record the entity type, and keys for later usage.
    $table_data = views_fetch_data($this->view->base_table);
    $this->entityType = $table_data['table']['entity type'];
    
    if ($info = entity_get_info($this->entityType)) {
      $this->base_table = $info['base table'];
      $this->base_field = $info['entity keys']['id'];
    }
    else {
      throw new InvalidArgumentException('The base table is not a valid entity type.');
    }
  }

  /**
   * (non-PHPdoc)
   * @see views_plugin_row::option_definition()
   */
  public function option_definition() {
    $options = parent::option_definition();
    
    $options['view_mode'] = array('default' => 'full');
    return $options;
  }

  /**
   * (non-PHPdoc)
   * @see views_plugin_row::options_form()
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $info = entity_get_info($this->entityType);
    $options = _iwapi_extract_field($info['view modes']);
    
    if (!empty($options)) {
      $form['view_mode'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('View mode'),
        '#default_value' => $this->options['view_mode'],
      );
    }
    return $form;
  }

  /**
   * (non-PHPdoc)
   * @see views_plugin_row::pre_render()
   */
  public function pre_render($values) {
    if (!empty($values)) {
      $ids = array_map(array($this, 'parseIds'), $values);
      $this->entities = entity_load($this->entityType, $ids);
    }
  }

  /**
   * (non-PHPdoc)
   * @see views_plugin_row::render()
   */
  public function render($values) {
    $id = $values->{$this->field_alias};
    if (isset($this->entities[$id])) {
      $entity = $this->entities[$id];
      $render = $entity->view($this->options['view_mode']);
      return drupal_render($render);
    }
  }
  
  
  // ================================
  // Utility functions.
  // ================================
  
  /**
   * Given the results of the query, parse just the
   * ID's of the provided fields.
   * 
   * @param <mixed> $value
   *  The value passed by to have the ID extracted.
   */
  public function parseIds($value) {
    return $value->{$this->field_alias};
  }
}
