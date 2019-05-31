<?php
/**
 * @file iwCSVReader.inc
 * File contains the definition of the iwapi parser that handles CSV files.
 */

/**
 * iwReader class for managing the reading and iteration of CSV files.
 */
class iwCSVReader extends iwReader {
  
  protected $file;     // The original file object passed.
  protected $filepath; // full filepath, if one can be determined.

  protected $src;      // fopen File handle
  protected $keys;     // parsed CSV header

  protected $key;      // current key, the record # (line)
  protected $cur;      // currently loaded entry data
  
  public $delimiter = ',';  // CSV - comma separated.
  public $enclosure = '"';  // Double quotes default
  public $escape    = '"';  // Default to MS Excel convention
  
  /**
   * Check that the source file matches the file format
   *
   * @see iwIReader::checkFormat()
   */
  static function checkFormat($file) {
    return is_object($file) ?
    preg_match('#\.csv$#i', $file->uri) : (preg_match('#\.csv$#i', $file) > 0 && is_readable($file));
  }
  
  /**
   * Return a list of fields that can be used
   * in configurations, and serialization. 
   * 
   * @return array
   *  These are future enhancement items.
   */
  static function getPackageProps() {
    return array(
      'delimiter' => array('label' => t('Delineator')),
      'enclosure' => array('label' => t('Quote character')),
      'escape'    => array('label' => t('Escape character')),
    );
  }

  
  // ===============================
  // Instance methods
  // ===============================
  
  /**
   * Determine if this object is responsible for file clean-up
   * - Mostly used if handling a temporary file in a batch, we can
   *   clean up the file after we're done.
   */
  function __destruct() {
    if (!empty($this->src)) {
      @fclose($this->src);
    }
  }
  
  function setSource($file) {
    $this->key  = -1;
    $this->cur  = FALSE;
    $this->keys = NULL;
 
    $this->file = $file;
 
    // Keep the raw path for reporting, but use the "real" path
    $rawPath = is_object($file) ? $file->uri : $file;
    $this->filepath = drupal_realpath($rawPath);
    
    if (($this->src = @fopen($this->filepath, 'r')) === FALSE) {
      throw new iwReaderException(iwReaderException::INVALID_SOURCE, $rawPath);
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::getProgress()
   */
  function getProgress() {
    return array(
      'key' => $this->key,
      'filename' => $this->file,
      'byte_offset' => ftell($this->src),
    );
  }

  /**
   * Report the percentage that we've completed
   * reading this source file. Makes it a little
   * friendlier than showing a constant number. 
   * 
   * @return float
   *  Returns the percentage that this batch
   *  is completed.
   */
  function percentComplete() {
    return (ftell($this->src) / filesize($this->filepath)); 
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::resumeProgress()
   */
  function resumeProgress(array $progress) {
    if ($progress['byte_offset'] !== FALSE && $progress['byte_offset'] > 0) {
      if ($progress['filename'] == $this->file) {
        fseek($this->src, $progress['byte_offset'], SEEK_SET);
        $this->key = $progress['key'];
        $this->next();
      }
      else {
        // We are trying to resume with the wrong file.
        throw new iwReaderException(iwReaderException::RESUME_FAIL, $progress['filename'], $this->file);
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see iwIReader::getKeys()
   */
  function getKeys() {
    if (empty($this->keys)) {
      // Keys are at the start of the file. Save current
      // position, go to the beginning and come back.
      $pos = ftell($this->src);
      if (fseek($this->src, 0, SEEK_SET) === 0) {
        $this->keys = fgetcsv($this->src, NULL, $this->delimiter, $this->enclosure, $this->escape);
        if ($this->keys === FALSE) {
          throw new iwReaderException(iwReaderException::INVALID_DATA, $this->file);
        }

        // Go back to previous location, before method was called.
        fseek($this->src, $pos, SEEK_SET);
      }

      // Always make sure that we are returning an array
      if (!is_array($this->keys)) {
        $this->keys = array();
      }
    }
    return array_map('trim', $this->keys);
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::rewind()
   */
  function rewind() {
    if (fseek($this->src, 0, SEEK_SET) === 0) {
      if (empty($this->keys)) {
        $this->keys = fgetcsv($this->src);
      }
      else {
        fgets($this->src);
      }
      $this->key = -1;
      $this->next();
    }
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::next()
   */
  function next() {
    $this->cur = FALSE;
    do {
      ++$this->key; // do a line count, even on empty lines so errors are easier to track.
      if (($values = fgetcsv($this->src, NULL, $this->delimiter, $this->enclosure, $this->escape)) !== FALSE) {
        if (isset($values[0])) { // blank lines return null in the first array entry.
          $values    = array_map('trim', $values);
          $this->cur = array_combine($this->getKeys(), $values);
      
          break;
        }
      }
    } while (!feof($this->src));
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::key()
   */
  function key() {
    return $this->key;
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::current()
   */
  function current() {
    return $this->cur;
  }

  /**
   * (non-PHPdoc)
   * @see Iterator::valid()
   */
  function valid() {
    return ($this->cur !== FALSE && is_array($this->cur));
  }
}
