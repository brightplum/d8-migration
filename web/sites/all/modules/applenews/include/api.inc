<?php

/**
 * @class
 * Wrap an error response from Apple News in an exception.
 */
class ApplenewsAPIError extends Exception {

  /**
   * Apple News API error response.
   */
  protected $response;

  /**
   * ApplenewsAPIError constructor.
   *
   * @param object $response
   *   Apple News API error response object. Should have a single key, 'error',
   *   which is an indexed array of error objects, each with the following keys:
   *   - code: Required, error code.
   *   - keyPath: Optional, an array of field names that uniquly identifies a
   *     field in the JSON input of the request.
   *   - value: Optional, if applicable the value supplied in the request for
   *     the field specified by keyPath.
   *   - message: Optional, error message.
   * @param string $entity_type
   *   If there is a related entity, it's entity type.
   * @param int $entity_id
   *   If there is a related entity, it's entity id.
   *
   * @see https://developer.apple.com/library/content/documentation/General/Conceptual/News_API_Ref/CommonErrors.html#//apple_ref/doc/uid/TP40015409-CH4-SW1
   */
  public function __construct($response, $entity_type = NULL, $entity_id = NULL) {
    $this->response = $response;
    $messages = [];
    foreach ($response->errors as $error) {
      $message = sprintf('%s %s %s',
        $error->code,
        !empty($error->keyPath) ? $error->keyPath : '',
        !empty($error->message) ? $error->message : (!empty($error->value) ? $error->value : '')
      );
      $message = trim(preg_replace('~\s+~', ' ', $message));
      // Prefix with entity information if available.
      if (!empty($entity_type) && !empty($entity_id) && ($entities = entity_load($entity_type, [$entity_id]))) {
        $entity = reset($entities);
        $label = entity_label($entity_type, $entity);
        $prefix = sprintf('Error encountered when contacting the Apple News API for "%s" (%s): ', $label, $entity_id);
      }
      else {
        $prefix = 'Error encountered when contacting the Apple News API: ';
      }
      $messages[] = $prefix . $message;
    }
    parent::__construct(implode("\n", $messages));
  }

  /**
   * Get the Apple News API error reponse object.
   *
   * @return object
   *   Apple News API error reponse.
   */
  public function getResponse() {
    return $this->response;
  }
}
