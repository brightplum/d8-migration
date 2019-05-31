<?php
/**
 * @file
 * Contains the writer class which handles creating managed files.
 */


class iwManagedFileWriter extends iwWriter {

  const COPY_FILES = 1;
  const MOVE_FILES = 2;
    
  /**
   * (non-PHPdoc)
   * @see iwPackage::getPackageProps().
   */
  static function getPackageProps() {
    return array(
      'mode'        => array('label' => t('Write mode')),
      'fileMode'    => array('label' => t('Copy or move src files')),
      'regex'       => array('label' => t('File pattern')),
      'destination' => array('label' => t('File destination')),
      'fileType'    => array('label' => t('Type of file')),
    );
  }
  
  
  // -------------------------------
  // Instance properties
  // -------------------------------

  // Protected instance variables.
  protected $regex       = FALSE;
  protected $destination = 'public://';
  protected $fileType    = 'images';
  protected $fileMode    = self::MOVE_FILES;

  // Built once so we don't need to continuously build it.
  protected $extensionRegex;

  /**
   * Build a Managed File writer. This allows writing of file data
   * to a specific location.
   * 
   * @param array $def
   *  Definition array containing the configurable settings
   *  used with a Managed file writer.
   *
   * @throws iwException
   */
  function __construct(array $def = array()) {
    parent::__construct($def);
    
    // Check for if this is a valid destination for the data.
    if (!file_valid_uri($this->destination) || !file_prepare_directory($this->destination, FILE_CREATE_DIRECTORY)) {
      $error = 'The file destination "@destination" is invalid or unwritable. '.
        'This maybe caused by a missing stream wrapper or a badly formatted URI.';
      
      throw new iwException($error, array('@destination' => $this->destination));
    }
    
    // Process the destination to clean things up.
    $destination = trim($this->destination);
    if (preg_match('#^[a-z]+://(.*?)(/?)$#i', $destination, $matches)) {
      if (!empty($matches[2]) && empty($matches[3])) {
        $destination .= '/';
      }
    }
    $this->destination = $destination;
    
    // Check for valid filter types.
    $file_types = $this->getFileTypes();
    if (!isset($file_types[$this->fileType])) {
      $error = 'Selected filter "@filter" is invalid.';
      throw new iwException($error, array('@filter' => $this->fileType));
    }
    $extensions = '\.(' . preg_replace('/ +/', '|', preg_quote($file_types[$this->fileType]['extensions'])) . ')$';

    // Build the extension regular expression filters.
    $this->extensionRegex = "/{$extensions}/i";
    $this->regex = empty($regex) ? FALSE : '/'. str_replace('{{extensions}}', $extensions, $regex) .'/i';

    // Generate the normal writers properties, so it is known what fields we expect.
    $this->ids      = array('filename' => 'filename');
    $this->required = array('filename' => 'filename', 'src' => 'src');
    $this->allowed  = array(
      'fid'       => 'fid',
      'uid'       => 'uid',
      'filepath'  => 'filepath',
      'src'       => 'src',
      'uri'       => 'uri',
      'status'    => 'status',
      'filemime'  => 'filemime',
      'filesize'  => 'filesize',
      'timestamp' => 'timestamp',
    );
  }

  /**
   * Get an array with information on how to
   * validate different file types.
   * 
   * @return array
   *  An array defining the different file types, with
   *  their extensions, and validator functions.
   */
  function getFileTypes() {
    return array(
      'images' => array(
        'extensions' => 'gif jpg jpeg png',
        'validators' => array('isImage'),
      ),
      'documents' => array(
        'extensions' => 'doc docx pdf odt',
        'validators' => array(),
      ),
    );
  }

  /**
   * Confirm that the file being examined 
   * 
   * @param $uri
   *  The source URI of the file to check. 
   * @return array
   *  An array of error message or an empty array if successful.
   */
  function isImage($uri) {
    $errors = array();

    $realpath = drupal_realpath($uri);
    if (empty($realpath)) {
      $realpath = $uri;
    }
    
    // Use the default image toolkit to check file info.
    $info = image_get_info($realpath);
    if (!$info || empty($info['extension'])) {
      $errors[] = t('Only JPEG, PNG and GIF images are allowed.');
    }
    return $errors;
  }

  /**
   * Convert raw path into a safe filename, a final directory and
   * destination URI. The file_munge_filename() function will be run
   * on the filename portion and reapplied to the final URI.
   * 
   * @param $path
   *  The raw path to clean and create a full path from.
   *  
   * @return array
   *  An array containing the filename, directory, and destination.
   */
  protected function processFilepath($path) {
    $types = $this->getFileTypes();
    $extensions = $types[$this->fileType]['extensions'];

    $filename = file_munge_filename(drupal_basename($path), $extensions);
    $path = preg_replace('#^\s*([a-z]+:/|\.*)/|\.+#i', '', dirname($path));
    $path = $this->destination . trim($path, '/ ');
    
    if (substr($path, -1) !== '/') {
      $path .= '/';
    }
    $destination = $path . $filename;
    
    return array(
      'filename' => $filename,
      'directory' => $path,
      'destination' => $destination,
    );
  }

  /**
   * Validate the data array, by checking that all required
   * fields are provided in the data set.
   *
   * @param $data
   *  The data to check for integrity of the fields
   * @return boolean
   *  TRUE - if this data set seems complete and valid
   *  FALSE - if data doesn't validate.
   */
  function validate(array $data) {
    if (isset($data['filepath']) && isset($data['src'])) {
      if (is_string($data['src'])) {
        return file_exists(drupal_realpath($data['src']));
      }
      elseif ($type = get_resource_type($data['src'])) {
        drupal_set_message('Resource Type: '. $type);
        return in_array($type, array('stream', 'socket'));
      }
    }
    return FALSE;
  }

  /**
   * (non-PHPdoc)
   * @see iwWriter::exists()
   */
  function exists(array $data) {
    $path = $this->processFilepath($data['filepath']);
    return file_exists(drupal_realpath($path['destination']));
  }

  /**
   * (non-PHPdoc)
   * @see iwWriter::save()
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

    
    $fileMode = $this->fileMode;
   
    // Consolidate the source resource into a filename, pointing to an actual file.
    // If source is a string, this would already be the case, but a file handle
    // needs to get copied to a temporary file to be worked on.
    if (is_string($data['src'])) {
      $source = $data['src'];
    }
    elseif (in_array(get_resource_type($data['src']), array('stream', 'socket'))) {
      $fileMode = self::MOVE_FILES; // Always move from temp storage.
      $source   = drupal_tempnam('temporary://', 'file');
      
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
        drupal_set_message(check_plain($filename) .': <br />'. implode('<br />', $errors));
      }
    }
    
    // Prepare to save the file to the correct location, and create a {file_managed} table record.
    $replace = ($this->mode & self::ALLOW_UPDATE === 0) ? FILE_EXISTS_ERROR : FILE_EXISTS_REPLACE;
    switch ($fileMode) {
      case self::COPY_FILES:
        $func = 'file_unmanaged_copy';
        break;
    
      case self::MOVE_FILES:
      default:
        $func = 'file_unmanaged_move';
    }  
    
    if ($uri = $func($source, $destination, $replace)) {
      // Create a file object.
      $file = new stdClass();
      $file->fid      = NULL;
      $file->uri      = $uri;
      $file->filename = $filename;
      $file->filemime = file_get_mimetype($uri);
      $file->uid      = $user->uid;
      $file->status   = FILE_STATUS_PERMANENT;

      // If we are replacing an existing file re-use its database record.
      if ($replace == FILE_EXISTS_REPLACE) {
        $existing_files = file_load_multiple(array(), array('uri' => $uri));
        if (count($existing_files)) {
          $existing = reset($existing_files);
          $file->fid = $existing->fid;
        }
      }
      drupal_set_message('Saved:'. $uri);
      file_save($file);
      
      $data['fid'] = $file->fid;
      return $file;
    }
    return FALSE;
  }


  // ==========================
  // Implement iwIConfigurable
  // ==========================

  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendFormElements()
   */
  static function appendFormElements(&$form, $callbacks, &$validation_scope = NULL) {
    // Append defaults and at the same time, snapshot the current definition.
    $def = $form['#definition'] += array(
      'destination' => file_default_scheme() . '://',
      'fileType' => 'images',
      'regex' => FALSE,
    );

    $form['regex'] = array(
      '#type'  => 'textfield',
      '#title' => t('Regular expression'),
      '#default_value' => empty($def['regex']) ? '' : $def['regex'],
      '#description' => t(
        'Items matching this expression will be filtered and excluded. '.
        'Leave this blank to accept all filenames. Use {{extensions}} token'.
        'to be represent with the file extensions.'
      ),
    );

    $form['fileType'] = array(
      '#type' => 'select',
      '#title' => t('Allowed file types'),
      '#options' => array(
        'images'    => 'Images (*.gif, *.jpg, *.png, ...)',
        'documents' => 'Documents (*.doc, *.pdf, *.odt, ...)',
      ),
      '#default_value' => $def['fileType'],
    );

    // find the available file schemes that can be used.
    $schemes = file_get_stream_wrappers(STREAM_WRAPPERS_VISIBLE | STREAM_WRAPPERS_WRITE_VISIBLE);
    $schemes = array_keys($schemes);

    // Allow the user to select which tables this writer is connected to.
    $form['destination'] = array(
      '#type' => 'fieldset',
      '#title' => t('Enter the destination folder'),
      '#tree' => TRUE,
      '#description' => t('Enter the URI scheme and target of the destination folder to write to.'),

      'scheme' => array(
        '#type' => 'select',
        '#theme_wrappers' => array(),
        '#options' => array_combine($schemes, $schemes),
        '#default_value' => file_uri_scheme($def['destination']),
      ),
      'path' => array(
        '#type' => 'textfield',
        '#theme_wrappers' => array(),
        '#prefix' => '&nbsp;://&nbsp;&nbsp;',
        '#default_value' => file_uri_target($def['destination']),
      ),
    );
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateFormElements()
   */
  static function validateFormElements(&$form, &$form_state, $trigger) {
    $destination = implode('://', $form_state['destination']);
    if (!file_prepare_directory($destination, FILE_CREATE_DIRECTORY)) {
      form_error($form['destination'], t('Specified destination cannot be accessed or written to.'));
    }

    // TODO: Add a check for the regular expression. We should only accept valid expressions that aren't dangerous.
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitFormElements()
   */
  static function submitFormElements(&$form, &$form_state, $trigger) {
    $form['#definition']['fileType'] = $form_state['fileType'];
    $form['#definition']['regex'] = empty($form_state['regex']) ? FALSE : $form_state['regex'];

    $destination = implode('://', $form_state['destination']);
    $form['#definition']['destination'] = trim($destination, "\\/\r\n\t ");

    // Report that these settings have been updated successful and expect to be updated.
    return array('save' => TRUE);
  }
}
