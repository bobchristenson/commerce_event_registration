<?php
/**
 * @file
 * commerce_event_registration.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_event_registration_default_rules_configuration() {
  $items = array();
  $items['rules_add_event_registration_to_cart'] = entity_import('rules_config', '{ "rules_add_event_registration_to_cart" : {
      "LABEL" : "Add Webform Event Registration to Cart",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "php", "webform_rules" ],
      "ON" : [ "webform_rules_submit" ],
      "IF" : [
        { "node_is_of_type" : {
            "node" : [ "node" ],
            "type" : { "value" : { "paid_event" : "paid_event" } }
          }
        }
      ],
      "DO" : [
        { "drupal_message" : { "message" : "Please checkout before adding any other items to your cart.  Event Registrations must be checked out individually and not combined with other items in the same order" } },
        { "php_eval" : { "code" : "$product_id = $node-\\u003efield_product[\'und\'][0][\'product_id\'];\\r\\n$quantity = [data:quantity-value];\\r\\n$uid = $user-\\u003euid;\\r\\n\\r\\n\\/\\/ Create the new order in checkout; you might also check first to\\r\\n\\/\\/ see if your user already has an order to use instead of a new one.\\r\\n$order = commerce_order_new($uid, \'checkout_checkout\');\\r\\n\\r\\n\\/\\/ Save the order to get its ID.\\r\\ncommerce_order_save($order);\\r\\n\\r\\n\\/\\/ Load whatever product represents the item the customer will be\\r\\n\\/\\/ paying for and create a line item for it.\\r\\n$product = commerce_product_load($product_id);\\r\\n$line_item = commerce_product_line_item_new($product, $quantity, $order-\\u003eorder_id);\\r\\n\\r\\n\\/\\/ Save the line item to get its ID.\\r\\ncommerce_line_item_save($line_item);\\r\\n\\r\\n\\/\\/add the sid to the line item\\r\\n$line_item_wrapper = entity_metadata_wrapper(\'commerce_line_item\', $line_item);\\r\\n$line_item_wrapper-\\u003efield_webform_sid-\\u003eset(\'[data:sid]\');\\r\\n$line_item_wrapper-\\u003efield_webform_nid-\\u003eset(\'[node:nid]\');\\r\\n\\r\\n\\/\\/resave the line item with sid info\\r\\ncommerce_line_item_save($line_item);\\r\\n\\r\\n\\/\\/ Add the line item to the order using fago\'s rockin\' wrapper.\\r\\n$order_wrapper = entity_metadata_wrapper(\'commerce_order\', $order);\\r\\n$order_wrapper-\\u003ecommerce_line_items[] = $line_item;\\r\\n\\r\\n\\/\\/ Save the order again to update its line item reference field.\\r\\ncommerce_order_save($order);\\r\\n\\r\\n\\/\\/ Redirect to the order\'s checkout form. Obviously, if this were a\\r\\n\\/\\/ form submit handler, you\'d just set $form_state[\'redirect\'].\\r\\ndrupal_goto(\'checkout\\/\' . $order-\\u003eorder_id);" } }
      ]
    }
  }');
  $items['rules_auto_create_product_display_node'] = entity_import('rules_config', '{ "rules_auto_create_product_display_node" : {
      "LABEL" : "Auto Create Product Display Node",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "entity" ],
      "ON" : [ "commerce_product_insert" ],
      "IF" : [
        { "data_is" : { "data" : [ "commerce-product:type" ], "value" : "paid_event" } }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "node",
              "param_type" : "paid_event",
              "param_title" : "[commerce-product:title]",
              "param_author" : [ "commerce-product:creator" ]
            },
            "PROVIDE" : { "entity_created" : { "event_reg_display" : "Event Registration Display" } }
          }
        },
        { "data_set" : {
            "data" : [ "event-reg-display:field-product:0" ],
            "value" : [ "commerce-product" ]
          }
        },
        { "redirect" : { "url" : "[event-reg-display:edit-url]" } }
      ]
    }
  }');
  $items['rules_mark_webform_event_registration_paid'] = entity_import('rules_config', '{ "rules_mark_webform_event_registration_paid" : {
      "LABEL" : "Mark Webform Event Registration Paid",
      "PLUGIN" : "reaction rule",
      "WEIGHT" : "-10",
      "REQUIRES" : [ "rules", "php", "commerce_checkout" ],
      "ON" : [ "commerce_checkout_complete" ],
      "DO" : [
        { "php_eval" : { "code" : "$order = commerce_order_load([commerce-order:order-id]);\\r\\n$order_wrapper = entity_metadata_wrapper(\'commerce_order\', $order); \\r\\n$line_items = $order_wrapper-\\u003ecommerce_line_items-\\u003evalue();\\r\\nforeach($line_items as $line_item) { \\r\\n$line_item_wrapper = entity_metadata_wrapper(\'commerce_line_item\', $line_item); \\r\\n$sid = $line_item_wrapper-\\u003efield_webform_sid-\\u003evalue();\\r\\nif($sid \\u003e 0) {\\r\\n$value = db_query(\\"UPDATE {webform_submitted_data} SET data = :data WHERE cid = :cid AND sid = :sid\\", array(\':data\' =\\u003e 1, \':cid\' =\\u003e 2, \':sid\' =\\u003e $sid));\\r\\nreturn $value;\\r\\n};\\r\\n}" } }
      ]
    }
  }');
  return $items;
}
