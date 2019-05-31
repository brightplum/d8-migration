<?php
/**
 * @file lib/mappers/iwMapper.inc
 * The base core mapper for translating data from one format to another.
 */

/**
 * Base class for mapper objects.
 * Also acts as a pass-thru, process only mapper. For items that don't
 * need to get mapped, but need to be used in a Datamap context.
 */
class iwMapper implements iwIPackage {
 
  const IndexRegex = '/[\s\'"]*\]\[[\s\'"]*/';
  
  protected $multi = FALSE;     // Whether or not the result can be an array
  protected $valid = FALSE;     // Whether this mapper is in a ready state

  protected $setData = array(); // Allow certain values to be constant for this mapper.
  
  protected $result;            // Destination field, where result should be stored
  protected $procs;             // Set of functions to run on the data, including filtering

  /**
   *  The index is the name of the index from the $raw data to pull the input from,
   *  the field affected or internal name of the field is the value of the array.
   *  If the value starts with '@' character, this means it is a reference to another
   *  mapper. References are attached when the owning object calls $this->attachRefs().
   *
   *  This is needed in the cases like iwMapperDB, which has to use a different raw
   *  data index name from it's internal source name to avoid name collisions between
   *  internal mapper names. An example is using multiple database tables in the same
   *  dataset where column names may collide.
   *
   * @var array
   *  Used as a set of inputs formatted as "[raw data index] => [mapper source]"
   *  Where [raw data index] is the index from the raw data to use, and
   *  [mapper source] is either the mapper's internal index or reference to another
   *  mapper provided by the parent object instance.
   */
  protected $inputs;
  
  /**
   * (non-PHPdoc)
   * @see iwIPackage::formatSummary()
   */
  function formatSummary() {
    return array(
      
    );
  }

  /**
   * Default constructor puts the core fields where they belong
   *
   * @param $result
   *  The name of the result field, used as a key for binding the values
   * @param array $inputs
   *  The inputs that are used by mapper.
   */
  function __construct(array $def) {
    if (empty($def['result']) || empty($def['inputs'])) {
      throw new InvalidArgumentException(t(
        'Invalid arguments to iwMapper constructor: Missing result or inputs.'
      ));
    }
    
    $this->valid  = TRUE;  // Valid until proven incomplete.
    $this->multi  = !empty($def['multi']);
    
    // determine if there are locked values for this mapper
    if (!empty($def['setData']) && is_array($def['setData'])) {
      $this->setData = $def['setData'];
    }
    
    // Turn the flat inputs array into a key => handler array. Note that
    // reference inputs are keyed with the reference delimit character
    foreach ($def['inputs'] as $src => $data) {
      $handler = is_array($data) ? $data['handler'] : $data;
      
      if (is_numeric($src)) {
        $src = ($handler[0] === '@') ? substr($handler, 1) : $handler;
      }
      
      // If there are any references, report that the mapper is incomplete.
      if ($handler[0] === '@') {
        $this->valid = FALSE;
        $data = $handler;
      }
      $this->inputs[$src] = $data;
    }
    
    // Add the set data input fields if they need to exist.
    $setKeys = array_keys($this->setData);
    $this->inputs += array_combine($setKeys, $setKeys);
    
    // Run the data through processing functions, and to the output.
    $this->procs  = empty($def['procs']) ? array() : $def['procs'];
    $this->result = $def['result'];
    
    // Mapout the location of the destination field.
    if (preg_match('/(?:\[\w*\])+$/', $this->result, $matches)) {
      if (empty($matches[0])) {
        $this->resultMethod = 'appendResult';
      }
      else {
        $this->resultMethod = 'indexResult';
        $this->dstIndexes   = preg_split(self::IndexRegex, trim($matches[0], '[] \'"'));
      }
      $this->resultBase = substr($this->result, 0, -strlen($matches[0]));
    }
  }
  
  /**
   * Report if this mapper item is ready for use.
   * Fails primarily if there are dangling references.
   *
   * @return boolean
   *  Reports if this mapper is valid and all refs
   *  have been attached.
   */
  function isValid() {
    return $this->valid;
  }

  /**
   * Report if this mapper is expecting arrays of
   * inputs to generate a similar array of results.
   * 
   * @return boolean
   *  Returns TRUE if this mapper is set to
   *  handle multiple values, FALSE otherwise.
   */
  function handlesMultiple() {
    return $this->multi;
  }
  
  /**
   * Add processing function the mapper. These functions
   * are post processing only, and meant to handle a single
   * result element. Preprocessors can be simulated by adding
   * post processing on referenced handlers. Their processing
   * would occur before becoming input for this mapper.
   *
   * @param $func
   *  A callable function that takes a reference
   *  to an array of values. It should modify
   *  the array appropriately (filter, split, etc).
   */
  function addProcessCallback($func) {
    if ((is_array($func) && method_exists($func[0], $func[1])) || function_exists($func)) {
      $this->procs[] = $func;
    }
  }

  /**
   * Find inputs that are specified as references and attaches
   * them from the pool of available references. References are
   * built and managed by the owning object.
   *
   * @param array $refs
   *  Pool of reference objects to attach.
   */
  function attachRefs(array &$refs) {
    $this->valid = TRUE; // Assume all references will be attached
  
    foreach ($this->inputs as $key => $handler) {
      if (is_string($handler) && $handler[0] === '@') {
        if (!isset($refs[$key])) {
          $this->valid = FALSE; // Miss reference, blast our optimism!
          continue;
        }
        $this->inputs[$key] = &$refs[$key];
      }
    }
  }
  
  /**
   * Get an array of the destination key fields.
   *
   * @return string
   *  The resulting field identity. Where the field
   *  will be bound to.
   */
  function getResultKey() {
    return $this->result;
  }

  /**
   * Get an array of input names used by this mapper.
   *
   * @return array
   *  The names of all the input fields used by this mapper.
   */
  function getInputs() {
    return array_keys($this->inputs);
  }

  /**
   * Run through the gambit of set processing functions
   * on an array of data. Data will be processed in order.
   *
   * @param $data
   *  The dataset to be processed, this data
   *  will be modified by the set of process
   *  functions assigned to this mapper.
   */
  protected function process(&$data) {
    foreach ($this->procs as $func) {
      if (is_array($func)) {
        $args = $func[1];
        array_unshift($args, $data);
  
        $data = call_user_func_array($func[0], $args);
      }
      else {
        $data = $func($data);
      }
    }
  }

  /**
   * Do the mapping from the raw data to the destination data
   *
   * @param array $raw
   *  The dataset to get mapped, this set will remain unchanged
   * @param array $entry
   *  Mapped and processed data should be placed back into this array.
   *  Previous results of other mappings will appear here as well, but
   *  probably should not be counted on since they rely on call order,
   *  which won't be guaranteed.
   */
  function bind(array $raw, array &$entry) {
    $raw = $this->setData + $raw;
  
    $value = $this->multi ? $this->translateMulti($raw) : $this->translate($raw);
  
    if (!empty($this->procs)) {
      $this->process($value);
    }
  
    if (isset($this->resultMethod) && method_exists($this, $this->resultMethod)) {
      $method = $this->resultMethod;
      $this->{$method}($value, $entry);
    }
    else {
      $entry[$this->result] = $value;
    }
  }
  
  /**
   * Append a new result to the $this->resultBase
   * key of the entry data.
   * 
   * @param <mixed> $value
   *  The value to set in the result field.
   * @param array $entry
   *  Reference to entire array of data for which to alter.
   */
  protected function appendResult($value, array &$entry) {
    $entry[$this->resultBase][] = $value;
  }
  
  /**
   * Add a new result value to the $this->resultBase
   * key of the entry data at the specified key.
   * 
   * @param <mixed> $value
   *  The value to set in the result field.
   * @param array $entry
   *  Reference to entire array of data for which to alter.
   */
  function indexResult($value, array &$entry) {
    if (empty($entry[$this->resultBase])) {
      $entry[$this->resultBase] = array();
    }
    
    drupal_array_set_nested_value($entry[$this->resultBase], $this->dstIndexes, $value, TRUE);
  }
  
  /**
   * Given the original set of data, find all the
   * values which correspond to the mapper $inputs.
   * 
   * @param array $raw
   *  The original data to parse through for values.
   * @return array
   *  Of the values extracted from the $raw which
   *  apply to the $inputs of this instance.
   */
  function extractValues(array $raw) {
    $values = array();
    
    // Pull the required parameters from the raw input data.
    foreach ($this->inputs as $key => $handler) {
      if ($handler instanceof iwMapper) {
        $values[$handler->result] = $handler->translate($raw);
      }
      elseif (isset($raw[$key])) {
        if (is_array($handler)) {
          $handler = $handler['handler'];
        }
        $values[$handler] = $raw[$key];
      }
    }
    return $values;
  }

  /**
   * Attempt to translate data that is specified as multiples,
   * by peeling out array entries to form single entries and
   * translating them as single entries.
   *
   * Each of the single items are then combined into an array of results.
   *
   * @param $raw
   *  The raw source data to get mapped.
   * @return array
   *  An array of items, individually mapped.
   */
  function translateMulti($raw) {
    $max = 0;
    foreach ($this->inputs as $src => $handler) {
      if (!isset($raw[$src])) {
        $raw[$src] = array();
      } elseif (!is_array($raw[$src])) {
        $raw[$src] = array($raw[$src]);
      }
      $max = max($max, count($raw[$src]));
      reset($raw[$src]);
    }

    // Build individual arrays to pretend data is singular
    $results = array();
    for ($i = 0; $i < $max; ++$i) {
      $data = array();
      foreach ($this->inputs as $src => $handler) {
        if (is_array($raw[$src])) {
          if (current($raw[$src]) !== FALSE) {
            $data[$src] = current($raw[$src]);
            next($raw[$src]);
          }
        }
        else {
          $data[$src] = $raw[$src];
        }
      }
      $results[] = $this->translate($data);
    }
    return array_filter($results);
  }

  /**
   * Take the known input values and translate them
   * into a single result value. If $this->multi is
   * TRUE, the value is an array of data entries.
   *
   * @param array $raw
   *  Values used as raw input to get translated.
   */
  function translate(array $raw) {
    if (!$this->isValid()) {
      throw new iwMapperException(iwMapperException::UNBUILT_MAPPER);
    }
    
    $values = $this->extractValues($raw);
    return reset($values);
  }
  

  // -------------------------------
  // Serialization and storage
  // -------------------------------

  /**
   * (non-PHPdoc)
   * @see iwIPackage::packData()
   */
  function packData() {
    $data['multi']   = $this->multi;
    $data['procs']   = $this->procs;
    $data['result']  = $this->result;
    
    if (!empty($this->setData)) {
      $data['setData'] = $this->setData;
    }
  
    // Iterate through each of the inputs and record them
    // We can't use the standard iwPackage here because we
    // don't want to recursively enter the reference pool.
    foreach ($this->inputs as $name => $handler) {
      $data['inputs'][$name] = ($handler instanceof iwMapper)
        ? '@' . get_class($handler) : $handler;
    }
    
    return $data;
  }
  
  /**
   * (non-PHPdoc)
   * @see Serializable::serialize()
   */
  function serialize() {
    return serialize($this->packData());
  }
  
  /**
   * (non-PHPdoc)
   * @see Serializable::unserialize()
   */
  function unserialize($data) {
    if ($values = unserialize($data)) {
      $this->__construct($values);
    }
    else {
      // Use a fallback, so we can still try to re-configure.
      $this->__construct();
    }
  }
}


// ===============================
// Mapper exceptions
// ===============================

/**
 * Exceptions possibly thrown by mapper objects.
 */
class iwMapperException extends iwException {
  const INPUT_MISMATCH = 1;
  const UNBUILT_MAPPER = 2;
  const INVALID_MAPPER = 3;

  static function GetCodeMessage($code) {
    switch ($code) {
      case static::INPUT_MISMATCH:
        $msg = 'The number of inputs expected do not match with the inputs recieved %file: %line';
        break;
      case static::UNBUILT_MAPPER:
        $msg = 'This mapper is not completely built and is missing some references.';
        break;
      case static::INVALID_MAPPER:
        $msg = 'This mapper is missing referenced inputs on %file: %line';
        break;
      default:
        $msg = 'Unknown data mapper issue was encountered %file: %line';
    }
    
    return $msg;
  }
  
  /**
   * 
   * @param unknown $code
   * @param Exception $prev
   */
  function __construct($code, Exception $prev = NULL) {
    $args = array(
      '%line' => $this->getLine(),
      '%file' => $this->getFile(),
    );
    $message = static::GetCodeMessage($code);

    parent::__construct($message, $args, 'error', $prev);
  }
}
