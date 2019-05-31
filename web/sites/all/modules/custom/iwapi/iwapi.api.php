<?php
/**
 * @file
 * Contains examples and information about HOOKs implements by iwapi module.
 */


/**
 * Reports back to the caller, what entity attachers
 * are provided by this module.
 *
 * @return array
 *  An array of information that explains the mapping
 *  of fields in the template to the entity attacher
 *  which handles them. Keyed by the template index
 *  that will contain the data for the attacher.
 */
function hook_entity_attacher_info() {
  return array(
    // array key is the name of the index where
    // data will be stored in the template. 
    'template field' => array(
      'label'   => t('Example attacher'), // Friendly name to display, could be used in configuration form.
      'handler' => 'iwFieldAttacher',     // Class name of handler, should implement iwIEntityAttacher.
    ),
  );
}


/**
 * Allows other modules to provide handlers for 
 * iwapi management, imports, exports, etc...
 * 
 * @return array
 *  Return an array with information regarding what
 *  classes are available, if they are configurable,
 *  and what the friendly name for them is.
 */
function hook_iwapi_class_info() {
  return array(
    // Category of the handler class ('mapper', 'writer', 'reader', etc...)
    'category' => array(
      // Name of the handler class.            
      'classname 1' => array(            
        'label' => t('Friendly name'), // Friendly label to display in form select elements.
        'configurable' => TRUE,        // Does this class support iwIConfigurable interface?
      ),
      'classname 2' => array(
        'label' => t('Friendly name'),
        'configurable' => TRUE,
      ),
    ),
  );
}
