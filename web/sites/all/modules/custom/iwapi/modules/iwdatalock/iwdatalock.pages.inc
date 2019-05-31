<?php
/**
 * @file iwdatalock.pages.inc
 * Contains the page callback and functions for data locks.
 */


/**
 * Page callback to determine the operation and then
 * handle them, if user has permission or can handle it.
 * 
 * @param iwDataLock $lock
 *  Instance of the lock to use to acquire, check
 *  or get status for.
 */
function _iwdatalock_handle_lock_request(iwDataLock $lock) {  
  $op = empty($_GET['oplock']) ? 'ping' : strtolower($_GET['oplock']);

  switch ($op) {
    case 'release':
      $lock->release(user_access(IWDATALOCK_PERM_BREAK_LOCKS));
      break;
    case 'refresh':
      $lock->refresh();
      break;
    case 'ping':
    default:
      // Fall through, return status and return the status
  }

  // Return the command updating the status of the lock.
  return array('#type' => 'ajax', '#commands' => array(
    _iwdatalock_ajax_command_update_lock($lock->stat()),
  ));  
}


// ==============================
// AJAX commands
// ==============================

/**
 * Generates a AJAX command array, which will send
 * updated information to the update lock JS functions.
 * 
 * @param stdClass $data
 *  Lock status data, which will be send with the response.
 * @return array
 *  Return an array with a 'command' index, and data that
 *  should be send to JS for processing clientside.
 */
function _iwdatalock_ajax_command_update_lock($data) {
  global $user;
  
  if ($data) {
    $command = array(
      'command'  => 'update_datalock',
      'remaining' => $data->remaining,
      'captured' => ($user->uid == $data->uid),
    );
  
    // Should we display the username?
    if (variable_get(IWDATALOCK_VAR_DISPLAY_USER, TRUE)) {
      $owner = user_load($data->uid);
      $command['username'] = check_plain($owner->name);
    }
  }
  else {
    $command = array(
      'command'  => 'update_datalock',
      'remaining' => 0,
      'captured' => FALSE,
    );
  }
  return $command;
}
