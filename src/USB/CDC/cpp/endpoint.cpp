/**
 * @file USB/CDC/cpp/endpoint.cpp
 * @author askn (K.Sato) multix.jp
 * @brief Implementation sample for the USB-CDC-ACM class.
 * @version 0.4.12
 * @date 2026-11-13
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#include <avr/io.h>
/* Compilation with anything other than the AVR-DU series will be rejected. */
#if defined(__cplusplus) && defined(USB0)
#include "../../CDC.h"
#include "../constants.h"

namespace USB_NAMESPACE {
  /*
   * Implementing a USB endpoint
   */

  /*** Here we must allocate memory and define callback functions to operate on it. ***/
  alignas(2) NOINIT USB_EP_TABLE_t EP_TABLE;
  alignas(2) NOINIT USB_WM_TABLE_t EP_BUFFER;

  WEAK USB_EP_TABLE_t* get_endpointer_ptr (void) { return &EP_TABLE; }
  WEAK USB_WM_TABLE_t* get_workmem_ptr (void) { return &EP_BUFFER; }

  /* Returns the USB frame number. */
  uint16_t get_frame (void) { return EP_TABLE.FRAMENUM & 0x7FF; }

  WEAK void ep_setup_clear (void) {
    USB0_ADDR = 0;
    USB0_FIFOWP = 0;
    USB0_EPPTR = (uint16_t)&EP_TABLE.EP;

    /* USB_EP_REQ=EP0_OUT is TYPE_CONTROL and does not use the TRNCOMPL interrupt. */
    USB_EP_REQ.CTRL = USB_TYPE_CONTROL_gc | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_SETUP_PK_SIZE);
    USB_EP_REQ.DATAPTR = (uint16_t)&USB_SETUP_DATA;
    USB_EP_REQ.MCNT = USB_DATA_BUFFER_SIZE;
    USB_EP_REQ.STATUS = 0;

    /* USB_EP_RES=EP0_IN is TYPE_CONTROL and does not use the TRNCOMPL interrupt. */
    USB_EP_RES.CTRL = USB_TYPE_CONTROL_gc | USB_MULTIPKT_bm | USB_AZLP_bm | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_SETUP_PK_SIZE);
    USB_EP_RES.DATAPTR = (uint16_t)&USB_DATA_BUFFER;
    USB_EP_RES.MCNT = 0;
    USB_EP_RES.STATUS = USB_BUSNAK_bm;
  }

  WEAK void ep_setup_in_listen (void) {
    USB_EP_RES.MCNT = 0;
    loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
    USB_EP_STATUS_CLR(USB_EP_SETUP_IN) = ~USB_TOGGLE_bm;
  }

  WEAK void ep_setup_out_listen (void) {
    USB_EP_REQ.CNT = 0;
    loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
    USB_EP_STATUS_CLR(USB_EP_SETUP_OUT) = ~USB_TOGGLE_bm;
  }

  WEAK void ep_setup_in_pending (void) {
    loop_until_bit_is_set(USB_EP_RES.STATUS, USB_BUSNAK_bp);
  }

  WEAK void ep_setup_out_pending (void) {
    loop_until_bit_is_set(USB_EP_REQ.STATUS, USB_BUSNAK_bp);
  }

  WEAK void ep_setup_stalled (void) {
    bit_set(USB_EP_RES.CTRL, USB_DOSTALL_bp);
  }

} /* USB_CDC */

#endif
// end of code
