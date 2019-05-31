<?php
/**
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

class iwapi_views_join_revisioned_entity extends views_join {
    
  /**
   * (non-PHPdoc)
   * @see views_join::build_join()
   */
  function build_join($select_query, $table, $view_query) {
    $right_table = empty($this->definition['table formula'])
      ? $this->table : $this->definition['table formula'];
      
    $entity_info = entity_get_info($this->definition['entity_type']);
    $revKey = $entity_info['entity keys']['revision'];
    
    if ($this->left_table) {
      $left = $view_query->get_table_info($this->left_table);
      $left_field = "$left[alias].$this->left_field";
    }
    else {
      $left = $view_query->get_table_info($entity_info['base table']);
      $left_field = $this->left_field;
    }
    
    $args      = array();
    $condition = "$left_field = $table[alias].$this->field"
      ." AND $left[alias].$revKey = $table[alias].rev";
    
    // Tack on the extra.
    if (isset($this->extra)) {
      if (is_array($this->extra)) {
        $extras = array();
        foreach ($this->extra as $info) {
          $extra = '';
          // Figure out the table name. Remember, only use aliases provided
          // if at all possible.
          $join_table = '';
          if (!array_key_exists('table', $info)) {
            $join_table = $table['alias'] . '.';
          }
          elseif (isset($info['table'])) {
            // If we're aware of a table alias for this table, use the table
            // alias instead of the table name.
            if (isset($left) && $left['table'] == $info['table']) {
              $join_table = $left['alias'] . '.';
            }
            else {
              $join_table = $info['table'] . '.';
            }
          }

          // Convert a single-valued array of values to the single-value case,
          // and transform from IN() notation to = notation
          if (is_array($info['value']) && count($info['value']) == 1) {
            if (empty($info['operator'])) {
              $operator = '=';
            }
            else {
              $operator = $info['operator'] == 'NOT IN' ? '!=' : '=';
            }
            $info['value'] = array_shift($info['value']);
          }

          if (is_array($info['value'])) {
            // With an array of values, we need multiple placeholders and the
            // 'IN' operator is implicit.
            foreach ($info['value'] as $value) {
              $placeholder_i = $view_query->placeholder('views_join_condition_');
              $args[$placeholder_i] = $value;
            }

            $operator = !empty($info['operator']) ? $info['operator'] : 'IN';
            $placeholder = '( ' . implode(', ', array_keys($args)) . ' )';
          }
          else {
            // With a single value, the '=' operator is implicit.
            $operator = !empty($info['operator']) ? $info['operator'] : '=';
            $placeholder = $view_query->placeholder('views_join_condition_');
            $args[$placeholder] = $info['value'];
          }
          $extras[] = "$join_table$info[field] $operator $placeholder";
        }

        if ($extras) {
          if (count($extras) == 1) {
            $condition .= ' AND ' . array_shift($extras);
          }
          else {
            $condition .= ' AND (' . implode(' ' . $this->extra_type . ' ', $extras) . ')';
          }
        }
      }
      elseif ($this->extra && is_string($this->extra)) {
        $condition .= " AND ($this->extra)";
      }
    }
    
    // Finally add the relationship to query.
    $select_query->addJoin($this->type, $right_table, $table['alias'], $condition, $args);
  }
}
