/**
 * @file USB/CDC/cpp/callback.cpp
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <avr/io.h>
#if defined(__cplusplus) && defined(USB0)

#include "../../CDC.h"

namespace USB_NAMESPACE {
  /*
   * Callback empty template
   */

  WEAK void cb_bus_event_sof (void) {}
  WEAK void cb_bus_event_suspend (void) {}
  WEAK void cb_bus_event_resume (void) {}
  WEAK void cb_bus_event_reset (void) {}
  WEAK void cb_bus_event_stalled (void) {}
  WEAK void cb_bus_event_overflow (void) {}
  WEAK void cb_bus_event_underflow (void) {}
  WEAK void cb_bus_event_start (void) {}
  WEAK void cb_bus_event_stop (void) {}
  WEAK bool cb_bus_check (void) { return true; }

  WEAK void cb_request_setup_stalled (USB_EP_t* /* EP_REQ */, USB_EP_t* /* EP_RES */) {}
  WEAK bool cb_request_standard_other (USB_EP_t* /* EP_REQ */, USB_EP_t* /* EP_RES */) { return false; }
  WEAK bool cb_request_class (USB_EP_t* /* EP_REQ */, USB_EP_t* /* EP_RES */) { return false; }
  WEAK bool cb_request_class_other (USB_EP_t* /* EP_REQ */, USB_EP_t* /* EP_RES */) { return false; }
  WEAK bool cb_request_vender (USB_EP_t* /* EP_REQ */, USB_EP_t* /* EP_RES */) { return false; }

  WEAK void cb_endpoint_complete (USB_EP_t* /* EP */, uint8_t /* EP_ID */) {}
  WEAK void cb_event_class_sof (void) {}

  WEAK uint8_t cb_get_configuration (void) { return 0; }
  WEAK bool    cb_set_configuration (uint8_t /* _config */) { return false; }
  WEAK bool    cb_clear_feature (void) { return true; }
  WEAK bool    cb_set_feature (void) { return true; }
  WEAK void    cb_clear_state (void) {}

  WEAK uint8_t cb_get_interface (uint8_t /* _interface */) { return 0; }
  WEAK bool    cb_set_interface (uint8_t /* _interface */, uint8_t /* _altsetting */) { return false; }

  /*
   * CDC feature
   */

  WEAK void cb_cdc_set_lineencoding (LineEncoding_t* /* _lineencoding */) {}
  WEAK void cb_cdc_set_linestate (LineState_t* /* _linestate */) {}
  WEAK void cb_cdc_set_sendbreak (uint16_t /* _sendbreak */) {}
  WEAK void cb_cdc_clear_sendbreak (void) {}

}

#endif
// end of code
