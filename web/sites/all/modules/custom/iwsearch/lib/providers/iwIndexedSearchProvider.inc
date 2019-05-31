<?php
/**
 * @file
 * Contains the abstract base class for search providers which
 * have an index on the local database.
 */


/**
 * Interface for search handlers which want to have
 * local index table generated. Implementing the
 * interface will provided all the required methods
 * to facilitate the table.
 */
abstract class iwIndexedSearchProvider extends iwSearchProvider {

  public $last_indexed;
  
  /**
   * Returns the name of the table which can be used for storing
   * and managing the local search index for this instance.
   *
   * @return string
   *  The name of the index table used to provide a local index.
   */
  function getTablename() {
    return db_escape_table('iwsearch_index_'. $this->name);
  }

  /**
   * Compute a hash from a canonical definition of this
   * search provider. This can be used to test for changes
   * to that require re-indexing.
   *
   * @return string
   *  The hashed string with the current index state of
   *  this SearchProvider.
   */
  function computeHash() {
    $hash_content = 'elements:' . serialize($this->canonicalizeProperty('elements'));
    $hash_content .= 'sort:' . serialize($this->canonicalizeProperty('sort'));
    
    // Generate the hash of all values which contribute
    // to the index table definition. This way we can
    // detect relevant changes and kick a table rebuild.
    return md5(serialize($hash_content));
  }

  /**
   * Convert one of the object properties into an array
   * which will be sorted by keys recursively. This way
   * when serialized, items that have the same settings
   * will appear the same, regardless of original ordering.
   * 
   * @param string $propertyName
   *  Name of the property to convert into canonicalized
   *  data. For instance "elements" converts $this->elements
   *  into an array of canonical values.
   * @return array
   *  Array of canonicalize data. Sorted by key name
   *  at every level, so that it serializes the same.
   */
  protected function canonicalizeProperty($propertyName) {
    $data = array();
    $elements = $this->{$propertyName};
    ksort($elements);
    
    // Add all the elements in Alpha order, with sub-fields in alpha order.
    foreach ($elements as $name => $element) {
      $data[$name] = $element->packData();
      ksort($data[$name]);
      
      $stack = array();
      $stack[] = (object)array('key'  => $name, 'data' => &$data[$name]);
      
      // Recursively sort the data array by key name.
      while ($item = array_pop($stack)) {
        ksort($item->data); // Sort at this level.
        
        // Recurse on array data, and sort at each level.
        foreach ($item->data as $sub_name => &$sub_data) {
          if (is_array($sub_data) && !empty($sub_data)) {
            $stack[] = (object)array(
              'key' => $sub_name,
              'data' => &$sub_data,
            );
          }
        }
        unset($sub_data);
      }
    }
    
    return $data;
  }
  
  /**
   * Callback to return the database table schema,
   * that can be used to index these entities.
   *
   * @return array
   *  An array with a table definition of fields
   *  that conform to the Drupal schema API.
   *
   * @see Drupal HOOK_schema().
   */
  abstract function getSchema();

  /**
   * Callback to index an entity into our index
   * tables if needed.
   *
   * @param stdClass|iwEntity $entity
   *  The entity instance to push into the indexing table.
  */
  abstract function indexEntity($entity);

  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::save()
   */
  function save() {
    $original = _iwsearch_instance_load_unchanged($this->name);

    $trans = db_transaction('indexed_search_save');
    try {
      // Call the parent save to make the instance changes
      // to the iwsearch_instance database table.
      parent::save();
  
      // If the hashes have changed, something relevant
      // with the fields needs to be updated.
      if ($orig->getTablename() !== $this->getTablename() || $orig->computeHash() != $this->computeHash()) {
        db_drop_table($original->getTablename());
        
        db_create_table($this->getTablename(), $this->getSchema());
        db_update('iwsearch_instances')
          ->fields(array('last_indexed' => 0))
          ->condition('name', $this->name)
          ->execute();
      }
    }
    catch (Exception $e) {
      $trans->rollback();
      throw $e;
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwSearchProvider::delete()
   */
  function delete() {
    db_delete($this->getTablename()); // remove the index table.
    
    parent::delete();
  }
}
