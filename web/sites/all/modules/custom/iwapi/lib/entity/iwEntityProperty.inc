<?php
/**
 * @file
 * Contains the common / basic handlers for entity properties
 */


interface iwIEntityProperty {
  
  /**
   * Get the value from the entity. For the most
   * part this will be a very simple access function
   * but part of the interface incase some properties
   * require fancier extractions.
   * 
   * @param object $entity
   *  Entity to extract the value from.
   * @return type
   *  Value of the property for the passed in entity.
   */
  function getValue($entity);

  /**
   * Generate an array which can be used
   * to render the display of this property.
   * 
   * Subclasses should define their own set of options.
   * 
   * @return array
   *  A renderable array of the display content.
   */
  function view($entity, $entity_type);
  
  /**
   * Report back how searches functionality should treat
   * this property. In terms of indexing and sorting.
   * 
   * @return array | FALSE
   *  If the property is searchable, this will return an array
   *  describing the search characteristics of this property.  
   */
  function getSearchInfo();
}


/**
 * Default abstract implementation of the entity property.
 */
abstract class iwEntityProperty implements iwIEntityProperty {

  public $label;
  public $name;           // Name of the entity property
  public $index;          // Index type of entity property, FALSE = not searchable
  public $multi = FALSE;  // Do we expect multiple of this field?


  /**
   * Generate a new iwEntityProperty object.
   * 
   * @param string $propertyName
   *  Name of the entity property that this
   *  item belongs to.
   * @param array $info
   *  Array of information that could set the information.
   */
  function __construct($propertyName, array $info = array()) {
    $this->name = $propertyName;
    
    // Format a nicely defined labely for display;
    $this->label = empty($info['label']) ? $propertyName : $info['label'];

    // Set the default properties
    $this->index = isset($info['index']) ? $info['index'] : FALSE;
    $this->multi = (bool)(isset($info['multiple']) ? $info['multiple'] : FALSE);
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIEntityProperty::getValue()
   */
  function getValue($entity) {
    return $entity->{$this->name};
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIEntityProperty::getSearchInfo()
   */
  function getSearchInfo() {
    if (!empty($this->index)) {
      return array(
        'name' => $this->name,
        'index_type' => $this->index,
        'multiple' => $this->multi,
      );
    }

    return FALSE;
  }
}


// ===============================
// Special Entity property types
// ===============================

class iwAuthorProperty extends iwEntityProperty {

  public $authorKey = 'uid';
 

  function __construct($propertyName, array $info = array()) {
    parent::__construct($propertyName, $info);
    
    if (isset($info['authorKey'])) {
      $this->authorKey = $info['authorKey'];
    }
  }

  function getValue($entity) {
    return format_username(user_load($entity->{$this->authorKey}));
  }
  
  function view($entity, $entity_type) {
    $account = user_load($entity->{$this->authorKey});
    
    return array(
      '#prefix' => '<div class="entity-author">',
      '#suffix' => '</div>',
      '#markup' => 'by '. theme('username', array('account' => $account)),
    ); 
  }
  
  /**
   * (non-PHPDOC)
   * @see iwIEntityProperty::getSearchInfo()
   */
  function getSearchInfo() {
    if (!empty($this->index)) {
      return array(
        'name' => $this->name,
        'index_type' => $this->index,
        'multiple' => $this->multi,
      );
    }
    return FALSE;
  }
}

/**
 * Create an entity property which generates a
 * link to the content, with customized text.
 */
class iwContentLinkProperty extends iwEntityProperty {

  public $text = 'View details';
  
  /**
   * non-PHPDOC
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    $uri = entity_uri($entity_type, $entity);
    return array(
      '#prefix' => '<div class="entity-link">',
      '#suffix' => '</div>',
      '#type'    => 'link',
      '#title'   => $this->text,
      '#href'    => $uri['path'],
      '#options' => $uri['options'],
    ); 
  }
}

/**
 * Entity label property handler
 */
class iwLabelProperty extends iwEntityProperty {

  public $tag    = 'h2';
  public $isLink = TRUE;
  
  /**
   * non-PHPDOC
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    if ($this->isLink) {
      $uri = entity_uri($entity_type, $entity);
      $out = array(
        '#type' => 'link',
        '#title' => $entity->{$this->name},
        '#href' => $uri['path'],
        '#options' => $uri['options'],
      ); 
    }
    else {
      $out = array('#markup' => check_plain($entity->{$this->name}));
    }
    
    $out['#prefix'] = '<'. $this->tag .' class="entity-label">';
    $out['#suffix'] = "</{$this->tag}>";
    return $out;
  }
}


/**
 * Entity date property handler
 */
class iwTimestampProperty extends iwEntityProperty {

  /**
   * (non-PHPDOC)
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    return array(
      '#prefix' => '<div class="'. strtr($this->name, '_', '-') .'-prop">',
      '#suffix' => '</div>',
      '#markup' => date('m-d-Y', $entity->{$this->name}),
    );
  } 
}


/**
 * Entity property which show the bundle name.
 */
class iwBundleProperty extends iwEntityProperty {
  
  /**
   * (non-PHPDOC)
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    $info = entity_get_info($entity_type);
    
    $text = '';
    if (isset($info['entity keys']['bundle'])) {
      $bKey = $info['entity keys']['bundle'];
      $bundle = $entity->{$bKey};
      
      $text = $info['bundles'][$bundle]['label'];
    }
    
    return array(
      '#prefix' => '<div class="bundle-name">',
      '#suffix' => '</div>',
      '#markup' => check_plain($text),
    );
  }
}


// ===============================
// Generic Entity property types
// ===============================

/**
 * Represents an entity property which is only a
 * an integer value.
 */
class iwIntegerProperty extends iwEntityProperty {
    
  /**
   * (non-PHPDOC)
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    if ($this->multi) {
      $render = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(strtr($this->name, '_', '-') .'-prop', 'clearfix'),
        ),
      );
      
      foreach ($entity->{$this->name} as $val) {
        $render[] = array(
          '#prefix' => '<div class="property-item">',
          '#suffix' => '</div>',
          '#markup' => number_format($val, 0),
        );
      }
    }
    else {
      $render = array(
        '#prefix' => '<div class="'. strtr($this->name, '_', '-') .'-prop">',
        '#suffix' => '</div>',
        '#markup' => number_format($entity->{$this->name}, 0),
      );
    }

    return $render;
  }
}

/**
 * Represent a item property which is rendered as a string.
 */
class iwStringProperty extends iwEntityProperty {
    
  /**
   * (non-PHPDOC)
   * @see iwEntityProperty::view().
   */
  function view($entity, $entity_type) {
    if ($this->multi) {
      $render = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array(strtr($this->name, '_', '-') .'-prop', 'clearfix'),
        ),
      );
      
      foreach ($entity->{$this->name} as $val) {
        $render[] = array(
          '#prefix' => '<div class="property-item">',
          '#suffix' => '</div>',
          '#markup' => check_plain($val),
        );
      }
    }
    else {
      $render = array(
        '#prefix' => '<div class="'. strtr($this->name, '_', '-') .'-prop">',
        '#suffix' => '</div>',
        '#markup' => check_plain($entity->{$this->name}),
      );
    }

    return $render;
  }
}