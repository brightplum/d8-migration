<?php
/**
 * @file
 * Contains the class definition for the note entity.
 */


class NoteEntity extends iwEntity {
  
  static protected $Table = 'entity_notes';
  static protected $Keys  = array(
    'id'     => 'enid',
    'label'  => 'subject',
    'status' => 'status',
  );
  
  /**
   * Convert the encoded thread numbering into a regular
   * integer for number manipulations.
   * 
   * This is the same code used by the comments module to
   * generate vancodes. 
   * 
   * @param string $code
   *  Value which is encoded for a part of the thread ordering.
   * @return int
   *  Return the integer equivelant of the passed value.
   * 
   * @see vancode2int() from comments.module
   */
  static function ThreadToInt($code) {
    return base_convert(substr($code, 1), 36, 10);
  }
  
  /**
   * Convert a number to a string which can be used
   * with entity notes as a part of the thread value.
   * 
   * The first character is the length - 1 of the rest
   * of the string starting with (prefix with string length).
   * 
   * So "01" means string is 1 character long, and "100" means
   * 2 characters long. This helps maintain sorting order. So
   * "10" (thread = 110) is greater than "2" (thread = 02) in
   * string form.
   * 
   * @param int $i
   *  Number value to convert into the threaded equivelent.
   * @return string
   *  The number which is used as part of the thread value
   *  of the entity note.
   * @see int2vancode() form the comments.module
   */
  static function IntToThread($i) {
    $num = base_convert((int) $i, 10, 36);
    $length = strlen($num);
    
    return chr($length + ord('0') - 1) . $num;
  }

  
  // ---------------------------------
  // Instance variables
  // ---------------------------------
  
  // Set defaults for these always available items.
  public $enid;
  public $entity_type;
  public $entity_id;
  public $thread = '';
  public $parent = 0;
  
  public $subject  = '';
  public $uid      = 0;
  public $created  = 0;
  public $changed  = 0;
  public $status   = IWAPI_ACTIVE;
  public $ip;
  
  
  // ---------------------------------
  // Instance methods
  // ---------------------------------
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::entityType()
   */
  function entityType() {
    return 'entity_note'; 
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::getBundle()
   */
  function getBundle() {
    return 'entity_note';
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::isPage()
   */
  function isPage() {
    return FALSE; // As of now, none of these appear on their own page.
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::checkAccess()
   */
  function checkAccess($op, stdClass $account, $only_admin = FALSE) {
    $isAdmin = user_access(ENTITY_NOTE_PERM_ADMIN, $account);
    
    if ($isAdmin || $only_admin) {
      return $isAdmin;
    }
  
    // Check normal permissions.
    switch ($op) {
      case 'view':
        if (empty($this->status)) {
          return FALSE;
        }
        $perm = 'view content';
        break;
      case 'edit':
        $perm = ENTITY_NOTE_PERM_EDIT;
        break;
      case 'delete':
        $perm = ENTITY_NOTE_PERM_DELETE;
        break;
    }
  
    return $this->uid === $account->uid && user_access($perm, $account); 
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::getLabel()
   */
  function getLabel() {
    return $this->subject;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::getUri()
   */
  function getUri() {
    $entities = entity_load($this->entity_type, array($this->entity_id));
    
    // This comment doesn't have its own page, just the one it belongs to.
    if ($entity = reset($entities)) {
      $uri = entity_uri($this->entity_type, $entity);
      $uri['options']['fragment'] = 'entity-note-'. $this->enid;
      return $uri;
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::presave()
   */
  function presave() {
    if (empty($this->thread)) {
      // generate the correct "thread" value for this comment.
      $query = db_select('entity_notes', 'note')
        ->condition('note.entity_type', $this->entity_type)
        ->condition('note.entity_id', $this->entity_id)
        ->condition('note.parent', empty($this->parent) ? 0 : $this->parent);
      
      // Determine the last highest thread number.
      $query->addExpression('MAX(note.thread)', 'max_thread');
      
      if ($curr_thread = $query->execute()->fetchField()) {
        $parts = explode('.', substr($curr_thread, 0, -1));
        $thread = static::ThreadToInt(array_pop($parts)) + 1;
        $parts[] = static::IntToThread($thread);
        
        $this->thread = implode('.', $parts) .'/';
      }
      elseif ($this->parent == 0) {
        $this->thread = '00/';
      }
      else {
        $parent_thread = db_select('entity_notes', 'note')
          ->fields('note', array('thread'))
          ->condition('note.enid', $this->parent)
          ->execute()->fetchField();
        
        $this->thread = substr($parent_thread, 0, -1) .'.00/';
      }
    }
    
    // Set the current posters IP address
    if (empty($this->ip)) {
      // Only track the original IP address, if
      $this->ip = ip_address();
    }
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::buildContent()
   */
  function buildContent($view_mode, $langcode = NULL) {
    global $user;
      
    $content = parent::buildContent($view_mode, $langcode);
    
    // Depending on access, generate the proper links
    if ($this->checkAccess('delete', $user)) {
      $links['delete'] = array(
        'title' => t('delete'),
        'href' => 'entity-note/delete/nojs/'. $this->enid,
        'attributes' => array('class' => array('use-ajax')),
      );
    }

    if ($this->checkAccess('edit', $user)) {
      $links['edit'] = array(
        'title' => t('edit'),
        'href' => 'entity-note/edit/nojs/'.$this->enid,
        'attributes' => array('class' => array('use-ajax')),
      );
    }
    
    if (user_access(ENTITY_NOTE_PERM_CREATE)) {
      $links['reply'] = array(
        'title' => t('reply'),
        'href' => 'entity-note/reply/nojs/'. $this->enid,
        'attributes' => array('class' => array('use-ajax')),
      );
    }

    $content['links'] = array(
      'actions' => array(
        '#theme' => 'links',
        '#links' => $links,
        '#attributes' => array(
          'class' => array('inline', 'comment-actions')
        ),
      ),
    );
    
    // Set our own custom theming for this.
    $content['#theme'] = 'entity_note';
    return $content;
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::delete()
   */
  function delete() {
    // Make sure to delete all the child/reply threads
    $ids = db_select('entity_notes', 'note')
      ->fields('note', array('enid'))
      ->condition('note.parent', $this->enid)
      ->execute()->fetchCol();
    
    foreach (entity_load('entity_note', $ids) as $entity) {
      $entity->delete();
    }
    
    parent::delete();
  }
}
