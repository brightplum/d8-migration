<?php
/**
 * @file
 * The interface which implements functions to handle field widgets.
 */


interface iwIFieldWidget {

  /**
   * Generate the form elements which are used to help
   * configure this field widget.
   * 
   * @return array
   *  Array containing form elements used to configure
   *  how this widget works.
   */
  function getSettingsForm();

  /**
   * Generate the widget form used in content entry
   * pages to inject data into this field. 
   */
  function getWidgetForm(array &$form, array &$form_state, $langcode, array $items, $delta, $element);
}