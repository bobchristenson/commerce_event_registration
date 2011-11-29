<?php
/**
 * @file
 * commerce_event_registration.features.inc
 */

/**
 * Implements hook_commerce_product_default_types().
 */
function commerce_event_registration_commerce_product_default_types() {
  $items = array(
    'paid_event' => array(
      'type' => 'paid_event',
      'name' => 'Paid Event',
      'description' => 'A paid event is an event which is paid for via e-commerce when an event webform is filled out.',
      'help' => '',
      'module' => 'commerce_product_ui',
    ),
  );
  return $items;
}

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_event_registration_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function commerce_event_registration_node_info() {
  $items = array(
    'paid_event' => array(
      'name' => t('Paid Event'),
      'base' => 'node_content',
      'description' => t('Event with paid registration'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
