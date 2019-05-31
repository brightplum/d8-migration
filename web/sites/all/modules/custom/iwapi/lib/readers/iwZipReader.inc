<?php
/**
 * @file iwParserZip.inc 
 * Parser class which manages exploration of a ZIP archive.
 */


/**
 * Reader call implementation which reads data
 * from a ZIP archive, and allows iteration of
 * files and file information. 
 */
class iwZipReader extends iwReader {
  
  /**
   * Check the format of this item. This function only checks the file extension.
   *
   * @param $src
   *  The source file being checked as a zip file.
   *  
   * @return boolean
   *  True if the file has a "*.zip" file extension. False otherwise.
   */
  static function checkFormat($file) {
    return is_object($file) ?
      preg_match('#\.zip$#i', $file->uri) : (preg_match('#\.zip$#i', $file) > 0 && is_readable($file));
  }
  
  
  // ================================
  // Instance properties.
  // ================================

  protected $archive;
  
  // Protected status / state information
  protected $index   = 0;
  protected $current = NULL;
  
  public $filter = FALSE;
  
  
  // ================================
  // Instance methods
  // ================================
  
  /**
   * Create an instance
   * 
   * @param $src
   * 
   * @throws iwException
   * @throws iwZipException
   */
  function __construct($src, $filter = FALSE) {    
    // If this class is not available, we cannot proceed with using this class.
    if (!class_exists('ZipArchive', TRUE)) {
      throw new iwException('Zip archive support is not supported on this system');
    }
  }
  
  /**
   * Ensure that any open streams are closed when the object is destroyed.
   */ 
  function __destruct() {
    // Ensure that we close any open streams.
    if (isset($this->current['handle'])) {
      fclose($this->current['handle']);
      unset($this->current['handle']);
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwReader::setSource()
   */
  function setSource($src) {
    // Keep the raw path for reporting, but use the "real" path
    $rawPath = is_object($src) ? $src->uri : $src;
    $path = drupal_realpath($rawPath);
    
    $this->archive = new ZipArchive();
    if (empty($path) || ($result = $this->archive->open($path, 0)) !== TRUE) {
      throw new iwZipException($result, $rawPath, 'open');
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::getProgress()
   */
  function getProgress() {
    return array(
      'filepath' => $this->archive->filename,
      'index'    => $this->index,
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::resumeProgress()
   */
  function resumeProgress(array $progress) {
    if ($progress['filepath'] == $this->archive->filename && $progress['index'] >= 0) {
      $this->index = $progress['index'];
      $this->next();
    }
    else {
      // We are trying to resume with the wrong file name.
      throw new iwReaderException(iwReaderException::RESUME_FAIL, $progress['filepage'], $this->archive->filename);
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIReader::getKeys()
   */
  function getKeys() {
    return array(
      'filename', 'filesize',
      'timestamp', 'crc', 'src',
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::rewind()
   */
  function rewind() {
    $this->index = -1;
    $this->next();
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::next()
   */
  function next() {
    if (isset($this->current['src'])) {
      fclose($this->current['src']);
      unset($this->current);
    }

    do {
      ++$this->index; // Jump to the next entry position.
 
      if (($info = $this->archive->statIndex($this->index)) === FALSE) {
        break; // We have reached the end of the archive, time to bail.
      }

      // If this file was not a folder then build an entry for it.
      if (substr($info['name'], -1) !== '/') {
        // We want to add a stream handle, and convert the 
        $this->current = array(
          'filepath'  => $info['name'],
          'filesize'  => $info['size'],
          'crc'       => $info['crc'],
          'timestamp' => $info['mtime'],
          'src'       => $this->archive->getStream($info['name']),
        );
      }
    } while (empty($this->current['src']));
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::key()
   */
  function key() {
    return $this->index;
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::current()
   */
  function current() {
    return $this->current;
  }
  
  /**
   * (non-PHPdoc)
   * @see Iterator::valid()
   */
  function valid() {
    return isset($this->current) && $this->current !== FALSE;
  }
}


/**
 * Reader exception specifically to handle zip library
 * errors. It translates the error code into a message.
 */
class iwZipException extends iwReaderException {

  const ZIP_ER_OK          = 0;   /* N No error */
  const ZIP_ER_MULTIDISK   = 1;   /* N Multi-disk zip archives not supported */
  const ZIP_ER_RENAME      = 2;   /* S Renaming temporary file failed */
  const ZIP_ER_CLOSE       = 3;   /* S Closing zip archive failed */
  const ZIP_ER_SEEK        = 4;   /* S Seek error */
  const ZIP_ER_READ        = 5;   /* S Read error */
  const ZIP_ER_WRITE       = 6;   /* S Write error */
  const ZIP_ER_CRC         = 7;   /* N CRC error */
  const ZIP_ER_ZIPCLOSED   = 8;   /* N Containing zip archive was closed */
  const ZIP_ER_NOENT       = 9;   /* N No such file */
  const ZIP_ER_EXISTS      = 10;  /* N File already exists */
  const ZIP_ER_OPEN        = 11;  /* S Can't open file */
  const ZIP_ER_TMPOPEN     = 12;  /* S Failure to create temporary file */
  const ZIP_ER_ZLIB        = 13;  /* Z Zlib error */
  const ZIP_ER_MEMORY      = 14;  /* N Malloc failure */
  const ZIP_ER_CHANGED     = 15;  /* N Entry has been changed */
  const ZIP_ER_COMPNOTSUPP = 16;  /* N Compression method not supported */
  const ZIP_ER_EOF         = 17;  /* N Premature EOF */
  const ZIP_ER_INVAL       = 18;  /* N Invalid argument */
  const ZIP_ER_NOZIP       = 19;  /* N Not a zip archive */
  const ZIP_ER_INTERNAL    = 20;  /* N Internal error */
  const ZIP_ER_INCONS      = 21;  /* N Zip archive inconsistent */
  const ZIP_ER_REMOVE      = 22;  /* S Can't remove file */
  const ZIP_ER_DELETED     = 23;  /* N Entry has been deleted */


  /**
   * Translate and error code to a reader friendly message.
   *
   * @param $code
   *  Integer ZIP library error code to transform.
   * @return string
   *  A friendly message to display for this error code.
   */
  static function getCodeMessage($code) {
    switch ($code) {
      case self::ZIP_ER_MULTIDISK:   return 'Multi-disk zip archives not supported: %src';
      case self::ZIP_ER_RENAME:      return 'Renaming temporary file failed';
      case self::ZIP_ER_CLOSE:       return 'Closing zip archive failed';
      case self::ZIP_ER_SEEK:        return 'Seek error: file(%src) in %place';
      case self::ZIP_ER_READ:        return 'Read error: file(%src) in %place';
      case self::ZIP_ER_WRITE:       return 'Write error';
      case self::ZIP_ER_CRC:         return 'CRC error: file(%src)';
      case self::ZIP_ER_ZIPCLOSED:   return 'Containing zip archive was closed';
      case self::ZIP_ER_NOENT:       return 'No such file(%src)';
      case self::ZIP_ER_EXISTS:      return 'File already exists';
      case self::ZIP_ER_OPEN:        return 'Can not open file(%src)';
      case self::ZIP_ER_TMPOPEN:     return 'Failure to create temporary file';
      case self::ZIP_ER_ZLIB:        return 'Zlib error';
      case self::ZIP_ER_MEMORY:      return 'Malloc failure';
      case self::ZIP_ER_CHANGED:     return 'Entry has been changed';
      case self::ZIP_ER_COMPNOTSUPP: return 'Compression method not supported';
      case self::ZIP_ER_EOF:         return 'Premature EOF';
      case self::ZIP_ER_INVAL:       return 'Invalid argument';
      case self::ZIP_ER_NOZIP:       return 'Not a zip archive: file(%src)';
      case self::ZIP_ER_INTERNAL:    return 'Internal error';
      case self::ZIP_ER_INCONS:      return 'Zip archive inconsistent: file(%src)';
      case self::ZIP_ER_REMOVE:      return 'Can not remove file: file(%src)';
      case self::ZIP_ER_DELETED:     return 'Entry has been deleted';
      default:
        return 'Unknown ZIP error';
    }
  }
}