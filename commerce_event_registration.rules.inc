<?php

/**
 * @file
 * Rules integration for commerce event registration.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_event_registration_rules_action_info() {
  $actions = array();
  $actions['commerce_event_registration_order_create'] = array(
    'label' => t('Create an order from Event Registration'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
      ),
      
      'data' => array(
        'type' => 'webform',
        'label' => t('Data'),
      ),
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
      ),
    ),
    'group' => t('Commerce Event Registration'),
    'base' => 'commerce_event_registration_order_create_action',
  );
  
$actions['commerce_event_registration_webform_setup'] = array(
    'label' => t('Setup basics for Commerce Event Registration Webform'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Node'),
      ),
    ),
    'group' => t('Commerce Event Registration'),
    'base' => 'commerce_event_registration_webform_setup', 
  );
  
$actions['commerce_event_registration_mark_paid'] = array(
    'label' => t('Mark the Event Registration Webform Paid'),
    'parameter' => array(
      'node' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'group' => t('Commerce Event Registration'),
    'base' => 'commerce_event_registration_mark_paid', 
  );

  
return $actions;
}

/**
 * Rules action: adds the event registration to a new order
 *   order state.
 */
function commerce_event_registration_order_create_action($order, $node, $data, $user) {
  $product_id = $node->field_product['und'][0]['product_id'];
  $quantity = $data['components']['commerce_event_quantity']['value'][0];
  $uid = $user->uid;

  // Create the new order in checkout; you might also check first to
  // see if your user already has an order to use instead of a new one.
  $order = commerce_order_new($uid, 'checkout_checkout');

  // Save the order to get its ID.
  commerce_order_save($order);

  // Load whatever product represents the item the customer will be
  // paying for and create a line item for it.
  $product = commerce_product_load($product_id);
  $line_item = commerce_product_line_item_new($product, $quantity, $order->order_id);

  // Save the line item to get its ID.
  commerce_line_item_save($line_item);

  //add the sid to the line item
  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $line_item_wrapper->field_webform_sid->set($data['sid']);
  $line_item_wrapper->field_webform_nid->set($node->nid);

  //resave the line item with sid info
  commerce_line_item_save($line_item);

  // Add the line item to the order using fago's rockin' wrapper.
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $order_wrapper->commerce_line_items[] = $line_item;

  // Save the order again to update its line item reference field.
  commerce_order_save($order);

  // Redirect to the order's checkout form. Obviously, if this were a
  // form submit handler, you'd just set $form_state['redirect'].
  drupal_goto('checkout/' . $order->order_id);
}

/**
 * Rules action: define webform defaults on commerce event registration node
 */
function commerce_event_registration_webform_setup($node) {
$node->webform['components'][1] = array(
                'nid' => $node->nid,
                'cid' => 1,
                'pid' => 0,
                'weight' => 0,
                'form_key' => 'commerce_event_quantity',
                'name' => 'Quantity',
                'type' => 'textfield',
                'value' => 1,
                'mandatory' => 1,
              );
    
    $node->webform['components'][2] = array(
              'nid' => $node->nid,
              'cid' => 2,
              'pid' => 0,
             'weight' => 0,
              'form_key' => 'commerce_event_payment_status',
              'name' => 'Payment Status',
              'type' => 'select',
              'value' => '0',    
              'extra' => array(
                'items' =>  '1|Paid' . "\n" .
                                  '0|Unpaid',
                'multiple' => 0,
                'private' => 1,
              ),  //extra
              'mandatory' => 1,
      );
}


/**
 * Rules action: mark event registration paid 
 */
function commerce_event_registration_mark_paid($order) {
module_load_include('inc', 'webform', 'includes/webform.submissions');
  $loaded_order = commerce_order_load($order->order_number);
  $order_wrapper = entity_metadata_wrapper('commerce_order', $loaded_order); 
  $line_items = $order_wrapper->commerce_line_items->value();
  foreach($line_items as $line_item) { 
    $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item); 
    $sid = $line_item_wrapper->field_webform_sid->value();
    $nid = $line_item_wrapper->field_webform_nid->value();
    if($sid > 0) {
      $node = node_load($nid);
      $submission = webform_get_submission($nid, $sid);
      //change field to paid status
      dsm($submission);
      $submission->data[2]['value'] = '1';
      webform_submission_update($node, $submission);
    };
  }
}