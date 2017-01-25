<?php

/**
 * System settings for pretty_event_day module.
 * TODO: better form and validation.
 */
function pretty_event_day_admin_form($form, $form_state) {
  $pretty_event_day_settings = variable_get('pretty_event_day_settings', array());
  $i = 0;
  foreach ($pretty_event_day_settings as $setting) {
    //list($entity, $bundle, $field) = explode('|', $setting);
    $form['field_' . $i] = _pretty_event_day_admin_form_setting_template($i, $setting);
    $i++;
  }
  // Empty option for new setting.
  $form['field_' . $i] = _pretty_event_day_admin_form_setting_template($i);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Returns setting template.
 */
function _pretty_event_day_admin_form_setting_template($number, $default_value = '') {
  return array(
    '#type' => 'textfield',
    '#title' => 'Field ' . $number,
    '#default_value' => $default_value,
    '#description' => 'Format: entity_type|bundle|field_name',
  );
}

/**
 * Submit callback for pretty_event_day_admin_form().
 */
function pretty_event_day_admin_form_submit($form, &$form_state) {
  // Build new variable value.
  $old_pretty_event_day_settings = variable_get('pretty_event_day_settings', array());
  $new_pretty_event_day_settings = array();
  $i = 0;
  while (isset($form_state['values']['field_' . $i])) {
    if (!empty($form_state['values']['field_' . $i])) {
      $new_pretty_event_day_settings[] = $form_state['values']['field_' . $i];
    }
    $i++;
  }
  // Delete event_day instance for entities from removed settings.
  $remove_settings = array_diff($old_pretty_event_day_settings, $new_pretty_event_day_settings);
  foreach ($remove_settings as $remove_setting) {
    list($entity_type, $bundle, ) = explode('|', $remove_setting);
    if ($instance = field_info_instance($entity_type, 'event_day', $bundle)) {
      field_delete_instance($instance, FALSE);
    }
  }
  // Add field_event_day instance to entities from new settings.
  $new_settings = array_diff($new_pretty_event_day_settings, $old_pretty_event_day_settings);
  foreach ($new_settings as $new_setting) {
    list($entity_type, $bundle, ) = explode('|', $new_setting);
    if (!field_info_instance($entity_type, 'event_day', $bundle)) {
      $instance = array(
        'entity_type' => $entity_type,
        'bundle' => $bundle,
        'field_name' => 'event_day',
        'default_value' => NULL,
        'deleted' => 0,
        'description' => '',
        'label' => 'Day',
        'required' => FALSE,
        'settings' => array(
          'user_register_form' => FALSE,
        ),
        // TODO: which widget to use?
        'widget' => array(
          'active' => 1,
          'module' => 'field_extrawidgets',
          'settings' => array(
            'display_empty' => 0,
            'formatter' => 'taxonomy_term_reference_link',
            'formatter_settings' => array(),
          ),
          'type' => 'field_extrawidgets_read_only',
          'weight' => 1,
        ),
      );
      field_create_instance($instance);
    }
  }
  // Save settings in variable.
  variable_set('pretty_event_day_settings', $new_pretty_event_day_settings);
}