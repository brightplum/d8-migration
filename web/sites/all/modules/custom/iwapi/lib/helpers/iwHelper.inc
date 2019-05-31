<?php
/**
 * @file iwHelper.inc
 * Base helper class with management for import / export of items,
 * modules (enabling, checking required, etc...) and global items.
 */

abstract class iwHelper {


  
  /**
   * Create a mapper object from an array definition.
   * 
   * @param array $def
   *  The mapper definition and settings in array form.
   * 
   * @return iw_mapper
   *  Return a fully formed mapper object.
   */
  function create_mapper(array $def) {
    if (!(isset($def['mapper']) && ($def['mapper'] instanceof iw_mapper))) {
      if (isset($def['class']) && is_array($def['parameters'])) {
        $rc = new ReflectionClass($def['class']);
        $def['mapper'] = $rc->newInstanceArgs($def['parameters']);
      }
      else {
        throw new iwMapperException(iwMapperException::INVALID_MAPPER, 'Creating mapper');
      }
    }
    $mapper = $def['mapper'];
    if (is_array($def['process'])) {
      foreach ($def['process'] as $func) {
        $mapper->add_process($func);
      }
    }
    return $mapper;
  }
  
  /**
   * Peel off the packaging layer of any import and ensure
   *  that is has everything it needs to complete an install.
   *
   * @param $modules
   * 	List of modules that are assumed to be needed to complete
   *  the package in question.
   *  
   * @return boolean
   *  TRUE if all requirements are met, FALSE if something is
   *  suspected to be missing.
   */
  function check_required($package) {
    $missing = array();
    foreach ($package['requires'] as $module) {
      if (!module_exists($module)) {
        $missing[] = $module;
      }
    }
    
    if (empty($missing)) {
      return TRUE;
    }
    else {
      $msg = 'Importer "%helper:%type" failed: missing (%modules) modules';
      $params = array(
        '%helper' => $package['helper'],
        '%type' => $package['type'],
        '%modules' => implode(', ', $missing),
      );
      drupal_set_message(t($msg, $params), 'error');
      return FALSE;
    }
  }
  
  /**
   * Call used to describe the helper to the system
   *  This will allow the configurations admin screens know
   *  a bit more about what they are dealing with.
   *  
   * - List exportable / importable types
   * 
   * @return array
   * 	An array that describes items provided by this mapper.
   *  This would include the datamaps available, import and export names.
   */
  function get_info() {
    return array();
  }
  
  /**
   * Retrieve a data map definition in array form. If the
   *  map definition does not exist for a given named type
   *  return FALSE.
   * 
   * @param $name
   * 	A string identifier to determine which data map
   * definition is being requested.
   * 
   * @return boolean
   *  The base class provides no data maps, and therefore
   *  returns FALSE for all requested map types.
   */
  function get_map($name) {
    return FALSE;
  }
  
  /**
   * Get a list of currently available exportable items
   * of a specified type. 
   * 
   * Intended to be used to create export options.
   * 
   * @param $name
   *  Export type identifier string.
   * 
   * @return array
   *  Return a list of possible values for the export
   *  for a given export type / style. 
   */
  function get_export_options($name) {
    return array();
  }
  
  /**
   * Attempt to import package information.
   * 
   * @param $name
   *  Identifier name for the type of import we this package belongs to.
   * @param array $package
   *  The actual data and description of information to import.
   *  
   * @return boolean
   *  TRUE if the import is believed to have succeeded, FALSE otherwise.
   */
  function import($name, array $package)  {
    return FALSE;
  }
  
  /**
   * Generate an array of exported items of a specified
   *  type. Will check conditions to determine which items
   *  to include.
   * 
   * @param $name
   *  Identifier for this exportable item.
   * @param array $conditions
   *  Array of items to include for this export type.
   * 
   * @return array
   *  The export data packaged into an array.
   */
  function export($name, array $conditions = array()) {
    $method = 'export_'. $name;
    if (method_exists($this, $method)) {
      return $this->{$method}($conditions);
    }
    return FALSE;
  }
  
  /**
  * Search a directory for all files matching a given pattern.
  * - the order is important here, it will get all files in the
  *    current directories before scanning sub-directories
  * - this allows files in child directories to have dependancies
  *    on items loaded from parent directories!
  *
  * - 'file_scan_directory' function from Drupal might do the job
  *    but there is no comment on the order it includes items,
  *    which could be important for some dependancies.
  *
  * - TODO: 'Add some convention for file weights, to customize the
  * loading ordering of elements'
  *
  * @param $directory
  * 	Directory to search for this set of files.
  *
  * @param $pattern
  *  A regular expression to use to match
  *
  * @return array
  *  A list of filenames relative to starting directory
  */
  public static function get_files($directory, $pattern) {
    $files = array();
    if (is_dir($directory)) {
      $directory = rtrim($directory, '/\\'); // trim trailing slashes and spaces.
      $fstk = array(dir($directory));
      while (($dir = end($fstk)) !== false) {
        while (($file = $dir->read()) !== false) {
          if ($file[0] != '.') {
            // filter out hidden files
            if (is_dir($dir->path.$file)) {
              array_unshift($fstk, dir("$dir->path/$file"));
            }
            elseif (preg_match($pattern, $file)) {
              $files[] = "$dir->path/$file";
            }
          }
        }
        array_pop($fstk); // pop this directory off the stack
        $dir->close();    // close this directory, we're done with it
      }
    }
    elseif (file_exists($directory) && preg_match($pattern, $directory)) {
      $files[] = $directory;
    }
    return $files;  // list of all the files in the starting directory
  }
}
