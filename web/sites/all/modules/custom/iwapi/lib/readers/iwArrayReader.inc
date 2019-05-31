<?php
/**
 * @file iwParsers.inc
 *  Define the parsers used by the iwDatamap class to read in source data.
 *  
 *  These are basically wrappers for content or file types so that they can
 *  be accessed an a homogeneous way by the iwDatamap class. Most are just
 *  simple wrappers for content.
 */

/**
 * Wrapper for PHP arrays so they can be used with iwapi system.
 */
class iwArrayReader extends iwReader {
  protected $src;
    
  /**
   * Just ensure that we have a valid array
   * 
   * @see iwIReader::checkFormat()
   */
  static function checkFormat($src) {
    return is_array($src);
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::getProgress()
   */
  function getProgress() {
    return array('index' => key($this->src));
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::resumeProgress()
   */
  function resumeProgress(array $progress) {
    if (isset($progress['index'])) {
      reset($this->src);
      
      // Try to move the internal pointer until we find the position.
      while (!(key($this->src) === $progress['index'] || key($this->src) === NULL)) {
        next($this->src);
      }
      next($this->src);
    }
  }
  
  /**
   * wrapper for array function reset()
   * 
   * @see Iterator::rewind()
   */
  function rewind() {
    reset($this->src);
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::key()
   */
  function key() {
    return key($this->src);
  }
  
  /**
   * Get the current item pointed to by the interal pointer.
   *
   * @see Iterator::current()
   */
  function current() {
    return current($this->src);
  }
  
  /**
   * Move the internal array pointer to the next element
   * 
   * @see Iterator::next()
   */
  function next() {
    next($this->src);
  }
  
  /**
   * Ensure that the current element is an array.
   * 
   * @see Iterator::valid()
   */
  function valid() {
    return is_array(current($this->src));
  }
  
  /**
   * Get the array list of keys found here.
   * 
   * @see iwIParser::getKeys()
   */
  function getKeys() {
    $entry = $this->current();
    if (!is_array($entry)) {
      $entry = reset($this->src);
    }
    return array_keys($entry);
  }
}
