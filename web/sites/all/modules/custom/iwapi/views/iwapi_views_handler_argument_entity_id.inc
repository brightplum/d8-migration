<?php
/**
 * @file
 * Views argument handler for using an entity id for filtering.
 */

class iwapi_views_handler_argument_entity_id extends views_handler_argument_numeric {
  
  /**
   * Override the behavior of title(). Get the title of the node.
   */
  function title_query() {
    $query  = "SELECT ent.{$this->name_field} FROM {{$this->table}} ent WHERE ent.{$this->real_field} IN (:ids)";
    $titles = db_query($query, array(':ids' => $this->value))->fetchCol();
    
    return array_map('check_plain', $titles);
  }
}