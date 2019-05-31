<?php
/**
 * @file lib/core/exception.inc
 * Defines core exceptions for the IWAPI system.
 */

/**
 * A base exception class, which adds Drupal message handling functionality,
 * Allows logging to the Drupal message queue or to the watchdog.
 */
class iwException extends Exception {
  static protected $friendlyName = 'IWAPI Exception';

  protected $msgParams;
  protected $severity;

  /**
   * Base exception handling classes that are Drupal aware, for the
   * IWAPI module. This allows us to create messages that contain
   * translatable text with replacement placeholders.
   *
   * @param $message
   *  The error message to either get logged or displayed. Message can
   *  contain placeholder strings compatible to t() and should coincide
   *  with items found in the $params argument.
   * @param $params
   *  Array of text replacements for any placeholders found in $message.
   * @param $severity
   *  Set a severity level for this error. This can help provide a user
   *  with more information about the meaning of this exception.
   * @param Exception $prev
   *  Allow chaining of exceptions.
   */
  function __construct($message, $params = array(), $severity = 'error', Exception $prev = NULL) {
    parent::__construct($message, 0, $prev);

    $this->severity = $severity;
    $this->msgParams = (isset($params) && is_array($params)) ? $params : array();
  }

  /**
   * Log the message to the system watchdog.
   */
  public function logToWatchdog() {
    $severity = WATCHDOG_ERROR;
    switch ($this->severity) {
      case 'status':
        $severity = WATCHDOG_INFO;
        break;
      case 'warning':
        $severity = WATCHDOG_WARNING;
        break;
      case 'error':
        $severity = WATCHDOG_ERROR;
        break;
    }

    // Write to the watchdog our formatted message.
    watchdog(static::$friendlyName, $this->message, $this->msgParams, $severity);
  }

  /**
   * Log the message to a Drupal messsage
   */
  public function logToMessages() {
    drupal_set_message($this->__toString(), $this->severity);
  }

  /**
   * (non-PHPdoc)
   * @see Exception::__toString()
   */
  public function __toString() {
    $params = $this->msgParams;
    $params['@exception_name'] = static::$friendlyName;
    return t('@exception_name: '. $this->message, $params);
  }
}
