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
      "REQUIRES" : [ "rules", "commerce_event_registration", "webform_rules" ],
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
        { "commerce_event_registration_order_create" : {
            "order" : [ "site:current-cart-order" ],
            "node" : [ "node" ],
            "data" : [ "data" ],
            "user" : [ "user" ]
          }
        }
      ]
    }
  }');
  $items['rules_auto_create_product_display_node'] = entity_import('rules_config', '{ "rules_auto_create_product_display_node" : {
      "LABEL" : "Auto Create Product Display Node",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "commerce_event_registration", "entity" ],
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
            "PROVIDE" : { "entity_created" : { "event_reg_display_node" : "Event Registration Display Node" } }
          }
        },
        { "data_set" : {
            "data" : [ "event-reg-display-node:field-product:0" ],
            "value" : [ "commerce-product" ]
          }
        },
        { "entity_save" : { "data" : [ "event-reg-display-node" ], "immediate" : 1 } },
        { "commerce_event_registration_webform_setup" : { "node" : [ "event-reg-display-node" ] } },
        { "entity_save" : { "data" : [ "event-reg-display-node" ], "immediate" : 1 } },
        { "redirect" : { "url" : "[event-reg-display-node:edit-url]" } }
      ]
    }
  }');
  $items['rules_mark_webform_event_registration_paid'] = entity_import('rules_config', '{ "rules_mark_webform_event_registration_paid" : {
      "LABEL" : "Mark Webform Event Registration Paid",
      "PLUGIN" : "reaction rule",
      "WEIGHT" : "-10",
      "REQUIRES" : [ "commerce_event_registration", "commerce_checkout" ],
      "ON" : [ "commerce_checkout_complete" ],
      "DO" : [
        { "commerce_event_registration_mark_paid" : { "node" : [ "commerce-order" ] } }
      ]
    }
  }');
  return $items;
}
