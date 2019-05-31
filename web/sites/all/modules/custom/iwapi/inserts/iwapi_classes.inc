<?php
/**
 * @file
 * Include information about classes available through the iwapi module.
 */

$t = get_t();
$default_classes = array(
  'utility' => array(
    'iwDataImport' => array(
      'label' => $t('Import parsable data'),
      'configurable' => TRUE,
    ),
  ),

  // Data source parsers
  'reader' => array(
    'iwArrayReader' => array('label' => $t('PHP Array/Object')),
    'iwZipReader' => array('label' => $t('ZIP file archive')),
    'iwDbReader' => array(
      'label' => $t('Database reader'),
      'configurable' => TRUE,
    ),
    'iwCSVReader' => array(
      'label'        => $t('CSV file (comma separated file)'),
      'configurable' => TRUE,
    ),
  ),

  // Data storage / writers
  'writer' => array(
    'iwDatabaseWriter'    => array(
      'label' => $t('Write to a database'),
      'configurable' => TRUE,
    ),
    'iwManagedFileWriter' => array(
      'label'  => $t('Write files to a directory'),
      'configurable' => TRUE,
    ),
    
    // Entity writers, populate fields and other items.
    'iwNodeWriter' => array(
      'label' => $t('Write to a new node'),
      'configurable' => TRUE,
    ),
    'iwUserWriter' => array(
      'label' => $t('Write new users'),
      'configurable' => TRUE,
    ),
    'iwTaxonomyWriter' => array(
      'label' => $t('Create Taxonomy terms'),
      'configurable' => TRUE,
    ),
  //  'iwFormatWriter'      => array(),
  //  'iwRoleWriter'        => array(),
  //  'iwPermissionWriter'  => array(),
    'iwRedirectWriter' => array(
      'label' => $t('Create redirects'),
      'configurable' => TRUE,
    ),
  ),

  // Data field mapping.
  'mapper' => array(
    'iwMapper'            => array('label' => $t('Simple re-arrange or processing'),  'configurable' => TRUE),
    'iwNestedMapper'      => array('label' => $t('Handle nested values'),             'configurable' => TRUE),
    'iwDatabaseMapper'    => array('label' => $t('Map fields from a database table'), 'configurable' => TRUE),
    'iwDelimitedMapper'   => array('label' => $t('Delimited text into an array'),     'configurable' => TRUE),
    'iwTextReplaceMapper' => array('label' => $t('Replace text while mapping data'),  'configurable' => TRUE),
    'iwTaxonomyMapper'    => array('label' => $t('Taxonomy finding with creation'),   'configurable' => TRUE), /*
    'iwMapperBoolToValue' => array('label'  => t('Convert boolean value to text.')),*/
  ),

  // Field API handlers.
  'field_handler' => array(
    'iwFieldHandlerDates'     => array('label' => $t('Date fields')),
    'iwFieldHandlerNumbers'   => array('label' => $t('Numeric fields')),
    'iwFieldHandlerText'      => array('label' => $t('Text fields')),
    'iwFieldHandlerList'      => array('label' => $t('List fields')),
    'iwFieldHandlerFile'      => array('label' => $t('File fields')),
    'iwFieldHandlerTaxonomy'  => array('label' => $t('Taxonomy fields')),
    'iwFieldHandlerLink'      => array('label' => $t('Link fields')),
    'iwFieldHandlerReference' => array('label' => $t('Entity Reference')),
    'iwFieldHandlerFivestar'  => array('label' => $t('Fivestar')),
  ),
  
  'wrapper' => array(
    'iwContentWrapper'  => array('label' => $t('Basic DIV')),
    'iwFieldsetWrapper' => array('label' => $t('Fieldset')),
    'iwPromptWrapper'   => array('label' => $t('Prompted DIV')),
  ),
);
