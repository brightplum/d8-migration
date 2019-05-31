<?php
/**
 * @file lib/writers/iwDatabaseWriter.inc
 * Defines the generic database writer, which bases on the Drupal schema.
 */


class iwDatabaseWriter extends iwWriter {
 
  /**
   * Report available package properties
   */
  static function getPackageProps() {
    return array(
      'mode'     => array('label' => t('Write mode')),
      'table'    => array('label' => t('Database table')),
      'ids'      => array(
        'label' => t('Primary key'),
        'multi' => TRUE,
      ),
      'required' => array(
        'label' => t('Required fields'),
        'multi' => TRUE,
      ),
    );
  }


  // ===============================
  // Instance properties
  // ===============================
  
  protected $table;		// Database table name

  /**
   * Create an instance of writer that writes data
   * to a database table. Writer will try to automatically
   * determine some of its properties based on schema info.
   */
  function __construct(array $values = array()) {
    parent::__construct($values);

    // Start the process of building based on the database schema
    if (!($schema = drupal_get_schema($this->table))) {
      // Second chance, with schema flush / reset.
      $schema = drupal_get_schema($this->table, TRUE);
    }

    if (!$schema) {
      $msg = 'iwDatabaseWriters: Could not locate schema for @table.';
      throw new iwException($msg, array('@table' => $this->table));
    }

    $this->allowed  = array_keys($schema['fields']);
    $this->required = array_intersect($this->allowed, $def['required']);

    // We will try to find unique identities for fields.
    // if not specified by user, we'll try looking to the schema
    $this->ids = $def['ids'];
    if (!empty($this->ids)) {
      $this->ids = array_intersect($this->allowed, $this->ids);
    }
    else {
      if (!empty($schema['primary key'])) {
        $this->ids = $schema['primary key'];
      }
      elseif (!empty($schema['unique keys'])) {
        $this->ids = reset($schema['unique keys']);
      }
    }

    // Make things a little bit easier to use array_diff, array_intersect, etc...
    $this->allowed  = array_combine($this->allowed, $this->allowed);
    $this->ids      = array_combine($this->ids, $this->ids);
    $this->required = empty($this->required) ? $this->ids : array_combine($this->required, $this->required);
  }

  /**
   * (non-PHPdoc)
   * @see iw_writer::exists()
   */
  function exists(array $data) {
    // Missing ID fields, usually this means no serial ID field.
    if (empty($this->ids) || count(array_diff_key($this->ids, $data)) > 0) {
      return FALSE;
    }

    $query = db_select($this->table, $this->table[0]);
    $query->addExpression('COUNT(*)');

    foreach ($this->ids as $field) {
      $query->condition($field, $data[$field]);
    }
    return $query->execute()->fetchField() > 0;
  }

  /**
   * (non-PHPdoc)
   * @see iw_writer::save()
   */
  function save(array &$data) {
    $error = $this->validate($data);
    if (!empty($error)) {
      throw new InvalidArgumentException(t($error));
    }

    if ($this->exists($data)) {
      if (($this->mode & self::ALLOW_UPDATE) !== 0) {
        return drupal_write_record($this->table, $data, $this->ids);
      }
    }
    elseif (($this->mode & self::ALLOW_INSERT) !== 0) {
      return drupal_write_record($this->table, $data);
    }
    return FALSE;
  }
}
