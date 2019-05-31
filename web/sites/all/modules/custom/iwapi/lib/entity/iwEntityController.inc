<?php
/**
 * @file class/entity/iwMultiTypeEntityController.inc
 * Contains the class definition for the entity controller implemented to handle multiple bundle types.
 */

/**
 * Implementation of an entity controller specifically designed to
 * handle entity types which have multiple bundle types.
 */
class iwEntityController extends DrupalDefaultEntityController {
  
  protected $entityClass;
  
  protected $bundleTable;
  protected $bundleKey;
  
  /**
   * Generate a new entity controller objects.
   * 
   * @param string $entityType
   *  Name of the entity type which will be used with this controller instance.
   * 
   * @throws InvalidArgumentException
   */
  function __construct($entityType) {
    parent::__construct($entityType);
    
    $this->entityClass = $this->entityInfo['entity class'];
    
    /*
    $this->bundleTable = $this->entityInfo['bundle table'];
    $this->bundleKey   = $this->entityInfo['bundle keys']['bundle'];
    
    if (empty($this->entityClass) || empty($this->bundleTable) || empty($this->bundleKey)) {
      throw new InvalidArgumentException(t(
        'Entity definition is missing bundle information or entity class for %type',
        array('%type' => $this->entityType)
      ));
    }
    */
  }
  
  /**
   * (non-PHPdoc)
   * @see DrupalDefaultEntityController::load()
   */
  function load($ids = array(), $conditions = array()) {
    $entities = array();
  
    // Create a new variable which is either a prepared version of the $ids
    // array for later comparison with the entity cache, or FALSE if no $ids
    // were passed. The $ids array is reduced as items are loaded from cache,
    // and we need to know if it's empty for this reason to avoid querying the
    // database when all requested entities are loaded from cache.
    $passed_ids = !empty($ids) ? array_flip($ids) : FALSE;
    
    // Try to load entities from the static cache, if the entity type supports
    // static caching.
    if ($this->cache) {
      $entities += $this->cacheGet($ids, $conditions);
      
      // If any entities were loaded, remove them from the ids still to load.
      if ($passed_ids) {
        $ids = array_keys(array_diff_key($passed_ids, $entities));
      }
    }
  
    // Load any remaining entities from the database. This is the case if $ids
    // is set to FALSE (so we load all entities), if there are any ids left to
    // load, if loading a revision, or if $conditions was passed without $ids.
    if ($ids === FALSE || $ids || ($conditions && !$passed_ids)) {
      // Build the query.
      $query = $this->buildQuery($ids, $conditions);
      $resultset = $query->execute();
      $resultset->setFetchMode(PDO::FETCH_CLASS, $this->entityClass, array());
  
      $queried_entities = $resultset->fetchAllAssoc($this->idKey);
      
      // Pass all entities loaded from the database through $this->attachLoad(),
      // which attaches fields (if supported by the entity type) and calls the
      // entity type specific load callback, for example hook_node_load().
      if (!empty($queried_entities)) {
        $this->attachLoad($queried_entities);
        $entities += $queried_entities;
        
        if ($this->cache) {
          $this->cacheSet($queried_entities);
        }
      }
    }

    // Ensure that the returned array is ordered the same as the original
    // $ids array if this was passed in and remove any invalid ids.
    if ($passed_ids) {
      // Remove any invalid ids from the array.
      $passed_ids = array_intersect_key($passed_ids, $entities);
      foreach ($entities as $entity) {
        $passed_ids[$entity->{$this->idKey}] = $entity;
      }
      $entities = $passed_ids;
    }
  
    return $entities;
  }
}


/**
 * The entity controller that handles has additional handling
 * for entity types that have revisioning.
 */
class iwEntityRevisionController extends DrupalDefaultEntityController {

  /**
   * Generate a new entity controller objects.
   *
   * @param string $entityType
   *  Name of the entity type which will be used with this controller instance.
   *
   * @throws InvalidArgumentException
   */
  function __construct($entityType) {
    parent::__construct($entityType);
  }
  
}
