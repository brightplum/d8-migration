<?php
/**
 * @file
 */

abstract class DiscourseTransaction {
  

  public $server;
  public $apiKey;
  public $user;

  function __construct($server, $apiKey, $user) {
    $this->server = $server;
    $this->apiKey = $apiKey;
    $this->user   = $user;
  }
    
  function getCurlOpts() {
    return array(
      CURLOPT_HEADER => TRUE,
      CURLOPT_SSL_VERIFYPEER => FALSE,
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_HTTPHEADER => array(
        'Cache-Control: private,no-cache,no-store',
        'Pragma: no-cache',
        'Expires: 01-Jan-1990 12:00 GMT',
      ),
    );
  }
  
  /**
   * Build the URL for the Discourse transaction, ensuring
   * that the API key and username are include if available.
   * 
   * @param type $uri
   * @param array $params
   * @return type
   */
  function buildUrl($uri, array $params = array(), $addCredentials = TRUE) {
    $query = !$addCredentials && empty($this->apiKey) ? array() : array(
      'api_key' => $this->apiKey,
      'api_username' => $this->user,
    );
 
    // Set options and ensure that credentials are included.
    return url($this->server .'/'. $uri, array(
      'absolute' => TRUE,
      'query' => $query + $params,
    ));
  }

  /**
   * 
   * @param type $url
   * @param array $data
   * @return \stdClass
   * @throws Exception
   */
  function execute($url, array $data = array(), $usePut = FALSE) {
    $opts = $this->getCurlOpts();

    $ch = curl_init($url);
    curl_setopt_array($ch, $opts);

    if (!empty($data)) {
      if ($usePut) {
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'PUT');
      }
      else {
        curl_setopt($ch, CURLOPT_POST, TRUE);
      }

      $query = empty($this->apiKey) ? array() : array(
        'api_key' => $this->apiKey,
        'api_username' => $this->user,
      );

      curl_setopt($ch, CURLOPT_POSTFIELDS, $query + $data);
    }
    $result = curl_exec($ch);

    // If cURL failed to send our request we need to
    // determine the error before closing the cURL session
    if ($result === FALSE) {
      $errno = curl_errno($ch);
      $error = curl_error($ch);
      curl_close($ch);
 
      // Throw the error we found, after closing cURL session.
      throw new Exception('cURL: '. $error, $errno);
    }
    curl_close($ch); // ensure that the cURL session is properly closed.

    // Generate the response container.
    $resp = new stdClass();

    if (!empty($opts[CURLOPT_HEADER])) {
      do {
        list($textHdr, $result) = explode("\r\n\r\n", $result, 2);
      } while (preg_match('#\s+100\s+continue\s*$#is', $textHdr));

      $resp->header = array();      
      if (preg_match_all('/^\s*([\w\-_\s]+)\s*:\s+(.*)$/m', $textHdr, $matches, PREG_PATTERN_ORDER)) {
        $resp->header = array_combine(array_map('strtolower', $matches[1]), $matches[2]);
        
        if (isset($resp->header['status'])) {
          preg_match('#^\s*(\d+)#', $resp->header['status'], $matches);
        } 
        else {
          preg_match('/^http[^\s]+ (\d+)/i', $textHdr, $matches);
        }

        $resp->code = intval($matches[1]);

        // If we recieved the wrong status code, record the issues.
        if ($resp->code != 200) {
          throw new DiscourseAPIException(
            'Unable to retrieve comments from Discourse.', 
            $resp->code,
            $resp->header,
            $result
          );
        }
      }
    }

    $resp->data = json_decode($result);
    return $resp;
  }

}

/**
 * 
 */
class DiscourseAPIException extends Exception {

  function __construct($message, $code, $header, $body) {
    $allowed = array_fill_keys(array(
      'server', 'date', 'content-type', 
      'transfer-encoding', 'vary', 'status',
    ), TRUE);

    // Check for new items
    $headers = array_intersect_key($header, $allowed);
    
    $message .= " HTTP Status = ${code}\r\n\r\n"
      . implode("\r\n", $headers)
      . "\r\n" . $body;

    parent::__construct($message, $code, NULL);
  }
}
