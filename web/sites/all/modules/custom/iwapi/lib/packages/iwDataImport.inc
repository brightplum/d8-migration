<?php
/**
 * @file iwDataImporter.inc
 * The class definition for the data importer.
 */

/**
 * Class for handling mapping source data into destinations
 * - Allows for special translation of source data into destination data
 * - Processing for breaking single items into many or vice-versa
 * - Break data processing into batch operations (tracking resume for parsers).
 */
class iwDataImport extends iwPackagedUtility {

  private $compiled;            // indicates if this datamap has already been built.
  
  protected $reader  = NULL;    // The class name of the parser to use.
  protected $mappers = array(); // iwMapper objects that translate raw source data into destination formats.
  protected $writers = array(); // iwWriter objects that write data to a destination.
  
  protected $passThru; // An array that maintains values that should be passed directly to the writers.
  protected $refs;     // Datamapping references, for external mapping links
  
  
  // ===================================
  // Custom data packaging handlers
  // =================================== 
  
  /**
   * (non-PHPdoc)
   * @see iwPackage::getPackageProps()
   */
  static function getPackageProps() {
    return array(
      'reader'  => array(
        'type'  => 'reader',
        'label' => t('File format'),
        'multi' => FALSE,
      ),
      'mappers' => array(
        'type'  => 'mapper',
        'label' => t('Data translators'),
        'multi' => TRUE,
      ),
      'writers' => array(
        'type'  => 'writer',
        'label' => t('Destinations'),
        'multi' => TRUE,
      ),
    );
  }
    
  /**
   * Creates a single import object based an array definition
   * of the reader <em>(source)</em>, mappers <em>(translators)</em>
   * and writers <em>(destinations)</em>.
   * 
   * @param array $def
   *  An array containing information required to build the
   *  importer.  
   * @param array $refs
   *  Reference to an array of possible mappers that can be 
   *  shared with multiple importers. This can allowed shared
   *  caching of already retrieved values.
   */
  function __construct(array $def = array(), &$context = array()) {
    // Set defaults and containers for data
    $this->compiled = FALSE;
    $this->mappers  = array();
    $this->writers  = array();
    $this->refs     = &$context;
    
    // Validate that the parser type implements interface iwReader
    if (!empty($def['reader']) && $def['reader'] instanceof iwReader) {
      $this->reader = $def['reader'];
    }
    
    // Ensure that writers implement the correct interface.
    foreach ($def['writers'] as $name => $writer) {
      $this->addWriter($name, $writer);
    }
    
    // Add an array of mapper items to the process.
    if (!empty($def['mappers'])) {
      foreach ($def['mappers'] as $key => $mapper) {
        $this->addMapper($key, $mapper);
      }
    }
  }
  
  /**
   * Add mappers to the list of available mappers.
   * 
   * @param $key
   *  If key name starts with '@' this means that
   *  this mapper should be stored as a potential
   *  reference for other mappers to make use of.
   * @param iw_mapper $mapper
   *  Additional mapping to add.
   */
  function addMapper($key, iwMapper $mapper) {
    if (is_string($key) && $key[0] === '@') {
      $this->refs[substr($key, 1)] = $mapper;
    }
    $this->mappers[$key] = $mapper;
  }
  
  /**
   * Add additional writers, or output destinations.
   * 
   * @param iwWriter $writer
   *  Writer object to call when we are ready to save an entry.
   */
  function addWriter($name, iwWriter $writer) {
    if (is_int($name)) {
      $name = 'Writer '. $name;
    }
    $this->writers[$name] = $writer;
  }

  /**
   * Execute the data import process on a source item.
   * 
   * @param $source
   *  The source file, or data that will get imported.
   */
  function execute($source) {
    $this->compile();
    $this->reader->setSource($source);
      
    $trans = db_transaction('iwDataImport');
    try {
      foreach ($this->reader as $raw) {
        $entry = array_intersect_key($raw, $this->passThru);
        foreach ($this->mappers as $mapper) {
          $mapper->bind($raw, $entry);
        }
        
        foreach ($this->writers as $writer) {
          $writer->save($entry);
        }
      }
    }
    catch(Exception $e) {
      $trans->rollback();
      throw $e;
    }
  }
  
  /**
   * Execute the data mapping as a batch operation
   * - Should be used for very large sets so PHP doesn't timeout
   * - Can be used to span multiple files
   * 
   * @param array $context
   *  The current information pertaining to the state
   *  of the import process. Should contain a data "source"
   *  and "progress" data.
   * @param int $entries
   *  Number of entries to map and write at a time.
   */
  function batch(array &$context, array &$results, $size = 100) {
    $this->compile();
    
    $reader = $this->reader;
    $reader->setSource($context['source']);
 
    if (!empty($context['progress'])) {
      $reader->resumeProgress($context['progress']);
    }
    else {
      $context['count'] = 0;
      $reader->rewind();
    }

    $trans = db_transaction('iwDataImport');
    try {
      while ($reader->valid()) {
        $raw   = $reader->current();
        $entry = array_intersect_key($raw, $this->passThru);

        foreach ($this->mappers as $mapper) {
          $mapper->bind($raw, $entry);
        }

        // Write the data to the database.
        foreach ($this->writers as $writer) {
          $writer->save($entry);
        }
        $results[] = t('Imported item');
        ++$context['count'];

        // This batch has processed it's limit
        if (--$size <= 0) {
          $context['progress']  = $reader->getProgress();
          $context['completed'] = FALSE;
          
          return (method_exists($this->reader, 'percentComplete'))
            ? $this->reader->percentComplete() : 0.01;
        }
  
        $reader->next(); // Go to the next slotted item.
      }
      $context['completed'] = TRUE; // Made it out of the loop without hitting the limit.
    }
    catch(Exception $e) {
      $trans->rollback();
      throw $e;
    }
  }
  
  /**
   * Try to build one-time setup and configurations
   *  before an actual execute() call. This requires that
   *  all mappers and writers used be available, so these
   *  actions are deferred until just before execution.
   */
  protected function compile() {
    if (empty($this->compiled)) {
      $mapped = array(); 
      foreach ($this->mappers as &$mapper) {
        $resultKey = $mapper->getResultKey();
        $mapped[$resultKey] = $resultKey;
        $mapper->attachRefs($this->refs);
      }
      
      $allowed = array();
      foreach ($this->writers as $writer) {
        $allowed += $writer->getAllowed();
      }
      
      // Pass thru = all values allowed, but not provided by a mapper.
      $this->passThru = array_diff_key($allowed, $mapped);
      $this->compiled = TRUE;
    }
  }
  
  // =================================
  // Implement configurable interface
  // =================================
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::appendElements()
   */
  function appendElements(array &$element, array &$form, array &$form_state) {
    iwapi_include('ajax');

    $readers = iwapi_get_class_info('reader');
    $parents = $element['#array_parents'];
    $parents[] = 'reader';
    
    $ajax_setup = array(
      'callback' => '_iwapi_ajax_configurable_callback',
      'wrapper'  => 'package-reader-definition',
      'target_element' => $parents,
    );
    
    $element['reader'] = array(
      '#type' => 'fieldset',
      '#title' => t('Data Reader'),
      '#prefix' => '<div id="package-reader-definition">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#description' => t('The reader is what interperts source data into individual entries that can get mapped and eventually written.'),
    );
    
    if (isset($this->reader)) {
      $readerType = get_class($this->reader);
      $element['reader']['summary'] = array(
        '#markup' => $this->reader->summarize(),
      );
      $element['reader']['remove'] = array(
        '#name' => implode('-', $element['reader']['#parents']) .'[remove]',
        '#type' => 'submit',
        '#value' => t('Remove'),
      );
    }
    else {
      $element['reader']['packageType'] = array(
        '#type' => 'select',
        '#title' => t('Reader type'),
        '#required' => TRUE,
        '#empty_value' => 0,
        '#options' => _iwapi_extract_field($readers),
        '#ajax' => $ajax_setup,
      );
      
      $readerType = empty($state['reader']['packageType']) ? FALSE : $state['reader']['packageType'];
      if ($readerType && array_key_exists($readerType, $readers)) {
        $reader = new $readerType(array());
        if ($reader instanceof iwIConfigurable) {
          $reader->appendElements($element['reader'], $state['reader'], $form);
        }
      }
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::validateElements()
   */
  function validateElements(array $element, array &$values, array &$form_state) {
    
  }
  
  /**
   * (non-PHPdoc)
   * @see iwIConfigurable::submitElements()
   */
  function submitElements(array $element, array &$values, array &$form_state) {
    
  } 
}