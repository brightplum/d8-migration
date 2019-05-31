<?php
/**
 * @file
 * Contains the trackable version of the file writer.
 */


class TrackedFileWriter extends iwManagedFileWriter {
  
  function __construct(array $def = array()) {
    parent::__construct($def);
    
    $this->allowed['orig_fid'] = 'orig_fid';
  }
    
  /**
   * (non-PHPdoc)
   * @see iwManagedFileWriter::save()
   */
  function save(array &$data) {
    global $user;

    // Get filters ready so we can help determine validation of the file.
    $filters = $this->getFileTypes();
    $filter = $filters[$this->fileType];
    extract($this->processFilepath($data['filepath']));

    if ((!empty($this->regex) && preg_match($this->regex, $filename)) || !preg_match($this->extensionRegex, $filename)) {
      return FALSE;
    }

    // If insertion is not allowed, and the file doesn't exist, error.
    $exists = file_exists(drupal_realpath($destination));
    if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
      drupal_set_message('Unable to create or write to requested directory');
      return FALSE;
    }
    elseif (!$exists && ($this->mode & self::ALLOW_INSERT) === 0) {
      drupal_set_message('File does not exist and inserts are disabled.');
      return FALSE;
    }

    // Consolidate the source resource into a filename, pointing to an actual file.
    // If source is a string, this would already be the case, but a file handle
    // needs to get copied to a temporary file to be worked on.
    if (is_string($data['src'])) {
      $source = $data['src'];
    }
    elseif (in_array(get_resource_type($data['src']), array('stream', 'socket'))) {
      $source = drupal_tempnam('temporary://', 'file');
      if ($wr_handle = fopen($source, 'cb')) {
        while (!feof($data['src'])) {
          // We choose the buffer size of 8kB because it works well
          // for OS's that buffer stream @ 4kB and 8kB.
          fwrite($wr_handle, fread($data['src'], 8192));
        }
        fclose($wr_handle);
      }
    }
    else {
      drupal_set_message(
        t(
          'Invalid file source specified %src.',
          array('%src' => $data['src'])
        ), 'error'
      );
      return FALSE;
    }

    // Check validation for the this file resource.
    foreach ($filter['validators'] as $func) {
      $errors = $this->{$func}($source);
      if (!empty($errors)) {
        drupal_set_message(check_plain($filename) .'<br />'. implode('<br />', $errors));
        return FALSE;
      }
    }
    
    // Prepare to save the file to the correct location, and create a {file_managed} table record.
    $replace = ($this->mode & self::ALLOW_UPDATE === 0) ? FILE_EXISTS_ERROR : FILE_EXISTS_REPLACE;
    if ($uri = file_unmanaged_copy($source, $destination, $replace)) {
      $file = (object)array(
        'fid' => NULL,
        'uri' => $uri,
        'filename' => $filename,
        'filemime' => file_get_mimetype($uri),
        'uid'      => $user->uid,
        'status'   => FILE_STATUS_PERMANENT,
      );

      // If we are replacing an existing file re-use its database record.
      if ($replace == FILE_EXISTS_REPLACE) {
        $existing_files = file_load_multiple(array(), array('uri' => $uri));
        if (count($existing_files)) {
          $existing  = reset($existing_files);
          $file->fid = $existing->fid;
        }
      }
      file_save($file);
      
      if (!empty($data['orig_fid'])) {
        // Save a lookup between the old and new file ID.
        db_merge('d6_importer_lookup')
          ->key(array('old' => $data['orig_fid'], 'data_type' => 'file'))
          ->fields(array(
            'data_type' => 'file',
            'old'  => $data['orig_fid'],
            'entity_type' => 'file',
            'id'   => $file->fid,
          ))->execute();
      }

      return $file;
    }

    return FALSE;    
  }
}
