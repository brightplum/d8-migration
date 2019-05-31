<?php
/**
 * @file
 * Stash manager for the iwSearchProviders.
 */


class iwSearchStashController extends iwStashController {

  static $cache = array();
  
  /**
   * non-PHPDOC
   * @see iwStashController::extractId()
   */
  public function extractId($obj) {
    if ($obj instanceof iwSearchProvider) {
      return $obj->getName();
    }
 
    throw new InvalidArgumentException('Object is not of type iwSearchProvider.');
  }

  /**
   * non-PHPDOC
   * @see iwStashController::getType()
   */
  public function getType() {
    return 'iwsearch';
  }
}
