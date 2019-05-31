<?php
/** 
 * @file
 * Contains the entity definition for a Common Dreams profile.
 */


class cdProfileEntity extends iwAliasedEntity {

  static public $Table = 'cdreams_profiles';
  static public $Keys  = array(
    'id'     => 'profile_id',
    'bundle' => 'type',
    'status' => 'status',
    'label'  => 'name',
  );
  
  
  /**
   * Get the list of embeddable field for a profile type.
   * 
   * @param string $bundle
   *  Machine name of the cdreams_profile bundle to
   *  search for embeddable fields.
   * @return array
   *  Array with fields which appear as embeddable.
   *  Keys are the field names and values are the
   *  field labels for this bundle.
   */
  static function EmbeddableFields($bundle) {
    static $profile_bundles;

    if (empty($profile_bundles[$bundle])) {
      $profile_bundles[$bundle] = array();

      // Find all fields visible in the embedded mode.
      foreach (field_info_instances('cdreams_profile', $bundle) as $pfield => $inst) {
        if ($inst['display']['embed']['type'] != 'hidden') {
          $profile_bundles[$bundle][$pfield] = $inst['label'];
        }
      }
    }

    return $profile_bundles[$bundle];
  }

  /**
   * Create the array of fields which can be embedded
   * by this profile bundle.
   * 
   * @param string $fieldname
   *  Name of field being used as a destination prefix.
   * @param string $bundle
   *  Machine name of the bundle to find embeddable field for.
   * @return array
   *  Information about fields that can get embedded into
   *  the parent / containing entity. Keyed by the profile
   *  field names with values equal to the array(
   *    dst   => 'destination key for containing entity',
   *    label => 'field instance label',
   *  );
   */
  static function GetEmbeds($fieldname, $bundle) {
    $embed  = array();
    $prefix = 'embed_'. preg_replace('#^field_#', '', $fieldname) .':';
 
    foreach (static::EmbeddableFields($bundle) as $field => $label) {
      $embed[$field] = array(
        'dst' => "{$prefix}{$field}",
        'label' => $label,
      );
    }

    return $embed;
  }
  

  // -----------------------
  // Instance variables
  // -----------------------

  public $profile_id;
  public $type;

  public $name;
  public $uid;

  public $status = IWAPI_ACTIVE;
  public $created;
  public $changed;


  // -----------------------
  // Instance methods
  // -----------------------

  /**
   * (non-PHPDOC)
   * @see iwEntity::entityType().
   */
  function entityType() {
    return 'cdreams_profile';
  }

  /**
   * (non-PHPDOC)
   * @see iwEntity::getLabel().
   */
  function getLabel() {
    return $this->name;
  }
  
  /**
   * (non-PHPDOC)
   * @see iwEntity::getUri().
   */
  function getUri() {
    return array('path' => 'profile/nojs/'. $this->profile_id);
  }
  
  /**
   * (non-PHPDOC)
   * @see iwEntity::isPage().
   */
  function isPage() {
    if ($entity = menu_get_object('iwapi_entity_object', 2)) {
      $idKey = static::$Keys['id'];
    
      // check if entity type and ID match up.
      return $entity->entityType() === $this->entityType()
        && $entity->{$idKey} == $this->{$idKey};
    }

    return FALSE;
  }

  /**
   * (non-PHPDOC)
   * @see iwEntity::checkAccess().
   */
  function checkAccess($op, stdClass $account, $only_admin = FALSE) {
    $is_admin = user_access('bypass node access', $account);
    if ($is_admin || $only_admin) {
      return $is_admin;
    }
    
    static $access;
    if (isset($access[$this->profile_id][$account->uid][$op])) {
      return $access[$this->profile_id][$account->uid][$op];
    }

    $own = $this->uid === $account->uid;
    switch ($op) {
      case 'edit':
      case 'delete':
      case 'modify':
       $access[$this->profile_id][$account->uid][$op] = (
         $own && user_access("cdreams_profile $op own profiles", $account)) 
         || user_access("cdreams_profile $op any profiles", $account);
       break;

      case 'view':
        $type = cdreams_profile_type_load($this->type);

        $isPage = preg_match("#^profile/(nojs|ajax)/$this->profile_id#", $_GET['q']);
        $access[$this->profile_id][$account->uid][$op] = $own || (
          $this->status && user_access('access content', $account)
          && (!$isPage || $type->has_page)
        );
        break;

      default:
        return FALSE;
    }
    
    return $access[$this->profile_id][$account->uid][$op];
  }
  
  /**
   * (non-PHPdoc)
   * @see iwEntity::view()
   */
  function buildContent($view_mode, $langcode = NULL) {
    $display = parent::buildContent($view_mode, $langcode);
    $type = cdreams_profile_type_load($this->type);
    
    // Add the title as a field if available
    if ($type->title_display === CDREAMS_PROFILE_TITLE_EMBED) {
      $properties = iwapi_get_entity_properties('cdreams_profile');

      $properties['name']->isLink = !$this->isPage();  
      $display['name'] = $properties['name']->view($this, 'cdreams_profile');
    }
    
    // Display the an embedded view? Only available for page views.
    if (strtolower(arg(1)) !== 'ajax' && $this->isPage() && !empty($type->view)) {
      $extra_fields = field_extra_fields_get_display($this->entityType(), $this->type, $view_mode);
      
      if (!empty($extra_fields['integrated_view']['visible'])) {
        $view = views_get_view($type->view);

        if ($view && $view->access('default')) {
          $view->set_arguments(array($this->profile_id));
          $view->pre_execute();
          $view->execute();
          
          if (!empty($view->result[0])) {
            $display['integrated_view'] = array(
              '#type' => 'container',
              '#attributes' => array('class' => array('clearfix')),

              'view_title' => array(
                '#type' => 'html_tag',
                '#tag' => 'h3',
                '#value' => $view->get_title(),
                '#attributes' => array('class' => array('integrated-views-title')),
              ),
              'view_content' => array('#markup' => $view->preview()),
            );
          }
        }
      }
    }

    return $display;
  }

  /**
   * Embed the fields of this profile into the
   * parent entity.
   * 
   * @param object $parent
   *  Parent entity, which is used to embed the field.
   * @param string $contKey
   *  String name of the property which contains the renderable array.
   */
  function embedFields($field, $parent, $contKey = 'content') {
    static $cachedInfo = NULL;
    static $overridden = array(); // This of fields already overridden by bundle.
    
    if (!isset($cachedInfo[$field['field_name']])) {
      if ($cache = cache_get('cdreams_profile:embedded_fields')) {
        $cachedInfo = $cache->data;
      }
      else {
        $cachedInfo[$field['field_name']] = array();
        foreach ($field['settings']['handler_settings']['target_bundles'] as $bundle) {
          $cachedInfo[$field['field_name']] += static::GetEmbeds($field['field_name'], $bundle);
        }
      }
    }
    
    $refInfo = $cachedInfo[$field['field_name']];
    $or      = cdreams_profile_type_load($this->type)->overrides;
    
    // Generate the embeddable view fields.
    $display = $this->view('embed');
    foreach (array_keys($display + $or) as $refKey) {
      if ($refKey[0] == '#') {
        continue;
      }

      if (!empty($overridden[$refKey])) {
        continue; // already overridden for this bundle.
      }
      elseif (key_exists($refKey, $or) && !empty($parent->content[$or[$refKey]])) {
        $overridden[$refKey] = TRUE; // mark it as overridden;
        $parent->content[$refInfo[$refKey]['dst']][] = $parent->content[$or[$refKey]];
      }
      elseif (key_exists($refKey, $refInfo) && !empty($display[$refKey])) {
        $parent->content[$refInfo[$refKey]['dst']][] = $display[$refKey];
      }
    }
  }
}
