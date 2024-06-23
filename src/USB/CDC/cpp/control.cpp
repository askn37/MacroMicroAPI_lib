/**
 * @file USB/CDC/cpp/control.cpp
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
#include <string.h>         /* memcpy */

#include "../../CDC.h"
#include "../constants.h"

namespace USB_NAMESPACE {

  /* Performs a USB bus reset, allowing initial control enumeration to begin. */
  void bus_reset (void) {
    ep_setup_clear();
    cb_clear_state();
    bus_attach();
    USB0_CTRLA = USB_ENABLE_bm | USB_FIFOEN_bm | USB_STFRNUM_bm | (USB_ENDPOINTS_MAX - 1);
  }

  /* Detach the USB bus. */
  void bus_detach (void) { USB0_CTRLB &= ~USB_ATTACH_bm; }

  /* Attach the USB bus. */
  void bus_attach (void) { USB0_CTRLB |= USB_ATTACH_bm; }

  /* Attempt to resume the USB bus. */
  void bus_resume (void) {
    if (bit_is_set(USB0_BUSSTATE, USB_SUSPENDED_bp)) USB0_CTRLB |= USB_URESUME_bm;
  }

  /* Returns true if USB communication is enabled. */
  bool is_ready (void) { return !is_busy(); }

  /* Returns false if USB communication is enabled. */
  bool is_busy (void) { return !(USBSTATE.CONFIG && cb_bus_check()); }

  /* 
   * Standard Request Enumeration Process
   * 
   * Unsupported requests or callback error will be STALLED.
   * 
   * The following callbacks will be called:
   *    GetDescriptor     size  = cb_get_descriptor_data(value, &buffer)
   *    SetConfiguration  bool  = cb_set_configuration(value)
   *    GetConfiguration  value = cb_get_configuration()
   *    SetInterface      bool  = cb_set_interface(value)
   * 
   * Please process class requests with 'bool cb_request_class(*EP_REQ, *EP_RES)'
   */
  bool request_standard (USB_EP_t* EP_REQ, USB_EP_t* EP_RES) {
    bool listen = true;
    uint8_t bRequest = USB_SETUP_DATA.bRequest;
    D1PRINTF("SR=%02X:%04X:%02X:%02X:%04X:%04X:%04X\r\n",
      EP_REQ->STATUS, EP_REQ->CNT, USB_SETUP_DATA.bmRequestType, USB_SETUP_DATA.bRequest,
      USB_SETUP_DATA.wValue, USB_SETUP_DATA.wIndex, USB_SETUP_DATA.wLength);
    if (bRequest == USB_REQ_GetStatus) {              /* 0x00 */
      EP_RES->CNT = 2;
      USB_HEADER_DATA.bLength = 0;
      USB_HEADER_DATA.bDescriptorType = 0;
    }
    else if (bRequest == USB_REQ_GetDescriptor) {     /* 0x06 */
      size_t _length = USB_SETUP_DATA.wLength;
      size_t _size = cb_get_descriptor_data(USB_SETUP_DATA.wValue, &USB_DATA_BUFFER);
      EP_RES->CNT = (_size > _length) ? _length : _size;
      listen = !!_size;
    }
    else if (bRequest == USB_REQ_GetConfiguration) {  /* 0x08 */
      EP_RES->CNT = 1;
      USB_HEADER_DATA.bLength = cb_get_configuration();
    }
    else if (bRequest == USB_REQ_SetConfiguration) {  /* 0x09 */
      EP_RES->CNT = 0;
      listen = cb_set_configuration((uint8_t)USB_SETUP_DATA.wValue);
    }
    else if (bRequest == USB_REQ_GetInterface) {      /* 0x0A */
      EP_RES->CNT = 1;
      USB_HEADER_DATA.bLength = cb_get_interface((uint8_t)USB_SETUP_DATA.wIndex);
    }
    else if (bRequest == USB_REQ_SetInterface) {      /* 0x0B */
      EP_RES->CNT = 0;
      listen = cb_set_interface((uint8_t)USB_SETUP_DATA.wIndex, (uint8_t)USB_SETUP_DATA.wValue);
    }
    else if (bRequest == USB_REQ_SetAddress) {        /* 0x05 */
      /* Initiate EP0_IN and, if there is a response, activate the assigned address. */
      EP_RES->CNT = 0;
      ep_setup_in_listen();
      ep_setup_in_pending();
      USB0_ADDR = USB_SETUP_DATA.wValue & 0x7F;
      D1PRINTF("USB0_ADDR=%02X\r\n", USB0_ADDR);
    }
    else if (bRequest == USB_REQ_ClearFeature) {  /* 0x01 */
      listen = cb_clear_feature();
    }
    else if (bRequest == USB_REQ_SetFeature) {    /* 0x03 */
      listen = cb_set_feature();
    }
    else if (bRequest == USB_REQ_SynchFrame) {    /* 0x0C */
      EP_RES->CNT = 2;
      *((uint16_t*)(&USB_HEADER_DATA)) = get_frame();
    }
    else {
      listen = cb_request_standard_other(EP_REQ, EP_RES);
    }
    return listen;
  }

  /*
   * Interrupt handling
   */

  /* Implementing BUS interrupts handling. */
  void handling_bus_events (void) {
    uint8_t busstate = USB0_INTFLAGSA;
    /*** Do not allow unnecessary interrupts. ***/
    if (bit_is_set(busstate, USB_SOF_bp)) {
      cb_event_class_sof();
      cb_bus_event_sof();
      USB0_INTFLAGSA |= USB_SOF_bm;
    }
    if (bit_is_set(busstate, USB_STALLED_bp)) {
      D2PRINTF("<STALLED>\r\n");
      cb_bus_event_stalled();
      USB0_INTFLAGSA |= USB_STALLED_bm;
    }
    if (bit_is_set(busstate, USB_RESET_bp)) {
      D2PRINTF("<RESET>\r\n");
      /* A USB bus reset occurred. */
      cb_bus_event_reset();
      /* The current USB address must be discarded and a setup transaction must be awaited. */
      USB0_INTFLAGSA |= USB_RESET_bm;
    }
    if (bit_is_set(busstate, USB_SUSPEND_bp)) {
      D2PRINTF("<SUSPEND>\r\n");
      /* Suspend requested. */
      cb_bus_event_suspend();
      /* Bus-powered devices must transition quickly to a lower power state. */
      USB0_INTFLAGSA |= USB_SUSPEND_bm;
    }
    if (bit_is_set(busstate, USB_RESUME_bp)) {
      D2PRINTF("<RESUME>\r\n");
      // /* Resume requested. */
      // bus_attach();
      // bus_resume();
      cb_bus_event_resume();
      /* The system is then allowed to return to a normal power state. */
      USB0_INTFLAGSA |= USB_RESUME_bm;
    }
    if (bit_is_set(busstate, USB_UNF_bp)) {
      /* Executed if the IN packet is empty. */
      D2PRINTF("<UNF>\r\n");
      cb_bus_event_underflow();
      USB0_INTFLAGSA |= USB_UNF_bm;
    }
    if (bit_is_set(busstate, USB_OVF_bp)) {
      /* Executed if an OUT packet is not received. */
      D2PRINTF("<OVF>\r\n");
      cb_bus_event_overflow();
      USB0_INTFLAGSA |= USB_OVF_bm;
    }
    if (busstate & (USB_RESUME_bm | USB_RESET_bm)) {
      bus_reset();
    }
  }

  /* Implementing transaction interrupts handling. */
  void handling_transaction_events (void) {
    if (bit_is_set(USB0_INTFLAGSB, USB_GNDONE_bp)) {
      D2PRINTF("<GNDONE>\r\n");
      USB0_INTFLAGSB |= USB_GNDONE_bm;
    }
    USB_EP_t* EP_REQ = &USB_EP_REQ;
    if (bit_is_set(EP_REQ->STATUS, USB_EPSETUP_bp)) {
      USB_EP_t* EP_RES = &USB_EP_RES;
      bool listen = true;
      uint8_t bmRequestType = USB_SETUP_DATA.bmRequestType;
      /* This condition indicates that there is a DATA phase packet to be received. */
      if (bit_is_clear(bmRequestType, USB_REQTYPE_DIRECTION_bp)) ep_setup_out_listen();
      bmRequestType &= USB_REQTYPE_TYPE_gm;
      if (bmRequestType == USB_REQTYPE_STANDARD_gc) {
        listen = request_standard(EP_REQ, EP_RES);
      }
      else if (bmRequestType == USB_REQTYPE_CLASS_gc) {
        listen = cb_request_class(EP_REQ, EP_RES);
      }
      /*** If vendor extensions are required ***/
      /*
      else if (bmRequestType == USB_REQTYPE_VENDOR_gc) {
        listen = cb_request_vender(EP_REQ, EP_RES);
      }
      */
      if (listen) {
        ep_setup_in_listen();
        ep_setup_out_listen();
      }
      else {
        D1PRINTF("[STALLED]\r\n");
        ep_setup_stalled();
        cb_request_setup_stalled(EP_REQ, EP_RES);
      }
      USB0_INTFLAGSB |= USB_SETUP_bm;
    }
    while (bit_is_set(USB0_INTFLAGSB, USB_TRNCOMPL_bp)) {
      uint8_t EP_ID = ((uint8_t*)((uint16_t)&get_endpointer_ptr()->EP - 256))[USB0_FIFORP];
      USB_EP_t* EP = &USB_EP(EP_ID);
      D1PRINTF("EP=%02X:%02X<%d\r\n", EP_ID, EP->STATUS, EP->CNT);
      cb_endpoint_complete(EP, EP_ID);
    }
  }

} /* USB_CDC */

#endif
// end of code
