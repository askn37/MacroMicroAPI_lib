/**
 * @file USB/CDC/cpp/interface.cpp
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
/* Compilation with anything other than the AVR-DU series will be rejected. */
#if defined(__cplusplus) && defined(USB0)
#include <string.h>         /* memcpy */
#include "api/macro_api.h"  /* interrupts and ATOMIC_BLOCK */

#include "../../CDC.h"
#include "../constants.h"

/* USB_NAMESPACE is specified by a macro. */
namespace USB_NAMESPACE {
  /*
   * Implementing the communication data class
   */

  /*** Here we must allocate memory and define callback functions to operate on it. ***/

  /* Memory to hold the USB_STATE structure. */
  Interface_State USB_STATE;

  /*
   * Private functions
   */

  /* Returns a pointer to the USB_STATE structure. */
  Interface_State* get_state (void) { return &USB_STATE; }

  /* Reads the current LineState_t structure. */
  LineState_t& get_line_state (void) { return USBSTATE.bmLineState; }

  /* Reads the current LineEncoding_t structure. */
  LineEncoding_t& get_line_encoding (void) { return USBSTATE.LineEncoding; }

  /* Change the timeout for read_bytes. The default is 1000ms. */
  void set_timeout (uint16_t _timeout) { USBSTATE.TIMEOUT = _timeout; }

  /* Clear the USB_STATE structure to its initial values. */
  void cb_clear_state (void) { USBSTATE = (Interface_State){}; }

  /* Callback function that returns the device setting value. */
  uint8_t cb_get_configuration (void) { return USBSTATE.CONFIG; }

  /* This callback operation is called during enumeration. */
  bool cb_clear_feature (void) { return true; }

  void enable_interrupt_sof (void) { USBSTATE.SOF = 10; USB0_INTCTRLA |= USB_SOF_bm; }

  void disable_interrupt_sof (void) { USB0_INTCTRLA &= ~USB_SOF_bm; }

  /*
   * Endpoint check functions
   */

  /* True if the host is waiting to send. */
  bool is_recv_overflow (void) {
    return bit_is_set(USB_EP_RECV.STATUS, USB_UNFOVF_bp);
  }

  /* True if the host is waiting to receive. */
  bool is_send_underflow (void) {
    return bit_is_set(USB_EP_SEND.STATUS, USB_UNFOVF_bp);
  }

  /* True if the receive buffer is readable. */
  bool is_recv_ready (void) {
    return bit_is_set(USB_EP_RECV.STATUS, USB_BUSNAK_bp);
  }

  /* True if the send buffer is writable. */
  bool is_send_ready (void) {
    return bit_is_set(USB_EP_SEND.STATUS, USB_BUSNAK_bp);
  }

  /* True if the receive buffer is read-prohibited. */
  bool is_recv_busy (void) {
    return bit_is_clear(USB_EP_RECV.STATUS, USB_BUSNAK_bp);
  }

  /* True if the send buffer is write protected. */
  bool is_send_busy (void) {
    return bit_is_clear(USB_EP_SEND.STATUS, USB_BUSNAK_bp);
  }

  /* Wait until the interrupt buffer is ready to be written. */
  void ep_interrupt_pending (void) {
    loop_until_bit_is_set(USB_EP_INTR.STATUS, USB_BUSNAK_bp);
  }

  /* Wait until the receive buffer is available to be read. */
  void ep_recv_pending (void) {
    loop_until_bit_is_set(USB_EP_RECV.STATUS, USB_BUSNAK_bp);
  }

  /* Wait until the send buffer is available to be written. */
  void ep_send_pending (void) {
    loop_until_bit_is_set(USB_EP_SEND.STATUS, USB_BUSNAK_bp);
  }

  /* Allow the next transmission. */
  /* Avoid buffer operations until transmission is complete. */
  void ep_interrupt_listen (void) {
    loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
    USB_EP_STATUS_CLR(USB_EP_INTR_IN) = ~USB_TOGGLE_bm;
  }

  /* Allow the next reception. */
  /* Avoid buffer operations until the next receive is complete. */
  void ep_recv_listen (void) {
    loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
    USB_EP_STATUS_CLR(USB_EP_BULK_OUT) = ~USB_TOGGLE_bm;
  }

  /* Allow the next transmission. */
  /* Avoid buffer operations until transmission is complete. */
  void ep_send_listen (void) {
    loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
    USB_EP_STATUS_CLR(USB_EP_BULK_IN) = ~USB_TOGGLE_bm;
  }

  /* Allow the next reception. */
  /* This is only possible if BUSNAK is set. */
  /* This will block until the host sends data. */
  void ep_recv_flush (void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      USBSTATE.RECVCNT = 0;
      USB_EP_RECV.CNT  = 0;
    }
    ep_recv_listen();
    #if defined(DEBUG)
    ep_recv_pending();
    D1PRINTF("  RECV=%02X/%d\r\n", USB_EP_RECV.STATUS, USB_EP_RECV.CNT);
    #if defined(DEBUG) && (DEBUG == 2)
    Serial.print("  RECV=").printHex((uint8_t*)USB_EP_RECV.DATAPTR, (size_t)USB_EP_RECV.CNT, ':').ln();
    #endif
    #endif
  }

  /* Allow the next reception. */
  /* This is only possible if BUSNAK is set. */
  void ep_send_flush (void) {
    D1PRINTF("  SEND=%02X/%d\r\n", USB_EP_SEND.STATUS, USBSTATE.SENDCNT);
    #if defined(DEBUG) && (DEBUG == 2)
      Serial.print("  SEND=").printHex((uint8_t*)USB_EP_SEND.DATAPTR, (size_t)USBSTATE.SENDCNT, ':').ln();
    #endif
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      USB_EP_SEND.CNT = USBSTATE.SENDCNT;
      USB_EP_SEND.MCNT = 0;
      USBSTATE.SENDCNT = 0;
    }
    ep_send_listen();
  }

  /* The next interrupt will be allowed. */
  /* This is only possible if BUSNAK is set. */
  void ep_interrupt_send (size_t _size = 0) {
    D1PRINTF("  INTR=%02X/%d\r\n", USB_EP_INTR.STATUS, _size);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      USB_EP_INTR.CNT  = _size;
      USB_EP_INTR.MCNT = 0;
    }
    ep_interrupt_listen();
  }

  /* SOF token detected interrupt callback. */
  /* This may occur up to one or more times within 1 millisecond. */
  void cb_event_class_sof (void) {
    /* Buffer read/write delay processing. */
    /* An attempt is made at least every 64 ms. */
    if (0 == (--USBSTATE.SOF)) {
      /* Buffered sends and receives can only be performed */
      /* if the underflow bit is set to avoid blocking from the host side. */
      if (is_send_underflow() && USBSTATE.SENDCNT > 0) ep_send_flush();
      disable_interrupt_sof();
    }
  }

  /*
   * Interface configuration callback functions
   */

  /* Callback function that, when called, prepares the bulk transfer. */
  bool cb_set_configuration (uint8_t _config) {
    if (_config) {
      USBSTATE.SENDCNT = 0;
      USBSTATE.RECVCNT = 0;

      /* USB_EP_NOOP=EP1_OUT is not used. */
      USB_EP_NOOP.CTRL    = USB_TYPE_DISABLE_gc;

      /* USB_EP_INTR=EP1_IN uses the TRNCOMPL interrupt. */
      USB_EP_INTR.STATUS  = USB_BUSNAK_bm;
      USB_EP_INTR.CNT     = 0;
      USB_EP_INTR.DATAPTR = (uint16_t)&USB_INTR_BUFFER;
      USB_EP_INTR.MCNT    = 0;
      USB_EP_INTR.CTRL    = USB_TYPE_BULKINT_gc | USB_MULTIPKT_bm | USB_AZLP_bm | USB_EP_SIZE_gc(USB_INTR_PK_SIZE);

      /* USB_EP_RECV=EP2_OUT does not use the TRNCOMPL interrupt and MULTIPKT. */
      USB_EP_RECV.STATUS  = USB_BUSNAK_bm;
      USB_EP_RECV.CNT     = 0;
      USB_EP_RECV.DATAPTR = (uint16_t)&USB_RECV_BUFFER;
      USB_EP_RECV.MCNT    = USB_BULK_RECV_MAX;
      USB_EP_RECV.CTRL    = USB_TYPE_BULKINT_gc | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_BULK_RECV_MAX);

      /* USB_EP_SEND=EP2_IN does not use the TRNCOMPL interrupt, only MULTIPKT. */
      USB_EP_SEND.STATUS  = USB_BUSNAK_bm;
      USB_EP_SEND.CNT     = 0;
      USB_EP_SEND.DATAPTR = (uint16_t)&USB_SEND_BUFFER;
      USB_EP_SEND.MCNT    = 0;
      USB_EP_SEND.CTRL    = USB_TYPE_BULKINT_gc | USB_MULTIPKT_bm | USB_AZLP_bm | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_BULK_SEND_MAX);

      cb_bus_event_start();
      send_serialstate(USBSTATE.SerialState);
    }
    else cb_bus_event_stop();
    USBSTATE.CONFIG = _config;
    return !!_config;
  }

  /* Sends the current SerialState information to the host. */
  void send_serialstate (SerialState_t &_serialstate) {
    ep_interrupt_pending();
    Notification_SerialState_t *p = (Notification_SerialState_t*)&USB_INTR_BUFFER;
    p->bmRequestType  = USB_REQTYPE_DIRECTION_bm | USB_REQTYPE_CLASS_gc | USB_RECIPIENT_INTERFACE_gc; // 0xA1
    p->bRequest       = CDC_REQ_SerialState;  // 0x20
    p->wValue         = 0;
    p->wIndex         = 0;  // Interface#0
    p->wLength        = 2;
    p->wmSerialState  = _serialstate;
    ep_interrupt_send(sizeof(Notification_SerialState_t));
  }

  /*
   * Class Request Enumeration Process Callback
   */
  /* This is called continuously while the CDC port is open on the host side. */
  bool cb_request_class (USB_EP_t* EP_REQ, USB_EP_t* EP_RES) {
    bool listen = true;
    uint8_t bRequest = USB_SETUP_DATA.bRequest;
    D1PRINTF("CR=%02X:%04X:%02X:%02X:%04X:%04X:%04X\r\n",
      EP_REQ->STATUS, EP_REQ->CNT, USB_SETUP_DATA.bmRequestType, USB_SETUP_DATA.bRequest,
      USB_SETUP_DATA.wValue, USB_SETUP_DATA.wIndex, USB_SETUP_DATA.wLength);
    if (bRequest == CDC_REQ_SetLineEncoding) {        /* 0x20 */
      /* Wait for a DATA phase packet to arrive. */
      ep_setup_out_pending();
      #if defined(DEBUG) && (DEBUG == 2)
        Serial.print("  DATA=").printHex((uint8_t*)EP_REQ->DATAPTR, (size_t)EP_REQ->CNT, ':').ln();
      #endif
      memcpy(&USBSTATE.sLineEncoding, &USB_DATA_BUFFER, sizeof(LineEncoding_t));
      D1PRINTF(" SLE=%ld\r\n", USBSTATE.LineEncoding.dwDTERate);
      cb_cdc_set_lineencoding(&USBSTATE.LineEncoding);
      EP_RES->CNT = 0;
    }
    else if (bRequest == CDC_REQ_GetLineEncoding) {   /* 0x21 */
      EP_RES->CNT = sizeof(LineEncoding_t);
      memcpy(&USB_HEADER_DATA, &USBSTATE.sLineEncoding, sizeof(LineEncoding_t));
      D1PRINTF(" GLE=%ld\r\n", USBSTATE.LineEncoding.dwDTERate);
    }
    else if (bRequest == CDC_REQ_SetLineState) {      /* 0x22 */
      USBSTATE.bLineState = (uint8_t)USB_SETUP_DATA.wValue;
      D1PRINTF(" SLS=%02X\r\n", USBSTATE.bLineState);
      cb_cdc_set_linestate(&USBSTATE.bmLineState);
      EP_RES->CNT = 0;
    }
    else if (bRequest == CDC_REQ_SendBreak) {         /* 0x23 */
      USBSTATE.BREAKCNT = USB_SETUP_DATA.wValue;
      D1PRINTF(" SB=%04X\r\n", USBSTATE.BREAKCNT);
      if (USB_SETUP_DATA.wValue > 0) {
        USBSTATE.SerialState.bBreak = true;
        cb_cdc_set_sendbreak(USB_SETUP_DATA.wValue);
      }
      else {
        USBSTATE.SerialState.bBreak = false;
        cb_cdc_clear_sendbreak();
      }
      send_serialstate(USBSTATE.SerialState);
      EP_RES->CNT = 0;
    }
    else {
      listen = cb_request_class_other(EP_REQ, EP_RES);
    }
    return listen;
  }

  /*
   * A callback to handle the endpoint arrival
   */
  /* This is where we maintain the CDC stream pipes. */
  void cb_endpoint_complete (USB_EP_t* /* EP */, uint8_t EP_ID) {
    if (EP_ID == USB_EP_INTR_IN) {
      ep_interrupt_pending();
      D2PRINTF("  BREAK=%04X\r\n", USBSTATE.BREAKCNT);
      if (1 < (uint16_t)(USBSTATE.BREAKCNT + 1)) {
        USBSTATE.BREAKCNT -= USBSTATE.BREAKCNT < USB_INTR_INTERVAL ? USBSTATE.BREAKCNT : USB_INTR_INTERVAL;
        ep_interrupt_send(0);
      }
      else if (USBSTATE.BREAKCNT == 0 && USBSTATE.SerialState.bBreak) {
        USBSTATE.SerialState.bBreak = false;
        cb_cdc_clear_sendbreak();
        send_serialstate(USBSTATE.SerialState);
      }
    }
  }

  /*
   * Application interface
   */

  /* Start running the application. */
  void start (void) {
    bus_attach();
    USB0_INTCTRLA = USB_SUSPEND_bm | USB_RESUME_bm | USB_RESET_bm | USB_STALLED_bm;
    USB0_INTCTRLB = USB_TRNCOMPL_bm | USB_SETUP_bm;
    bus_reset();
    D1PRINTF("USB0=%02X/S\r\n", USB0_CTRLA);
    interrupts();
  }

  /* Finishes running the application. */
  void stop (void) {
    cb_clear_state();
    bus_detach();
    cb_bus_event_stop();
    USB0_CTRLA = 0;
    D1PRINTF("USB0=%02X/E\r\n", USB0_CTRLA);
  }

  /* Wait until the send buffer is cleared. */
  /* This operation may be blocked if the host is not responding. */
  void write_flush (void) {
    if (is_busy()) return;
    if (USBSTATE.SENDCNT > 0) ep_send_flush();
    ep_send_pending();
  }

  /* Empties the current receive buffer. */
  /* If any data is being received, it will be discarded. */
  /* This operation may be blocked if the host is not responding. */
  void read_clear (void) {
    if (is_ready()) ep_recv_pending();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      USBSTATE.RECVCNT = 0;
      USB_EP_RECV.CNT = 0;
    }
  }

  /*
   * Stream interface
   */

  /* Sends one character. */
  size_t write_byte (const uint8_t _c) {
    if (is_busy()) return 0;
    ep_send_pending();
    /* This is only possible if BUSNAK is set. */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      USB_SEND_BUFFER[USBSTATE.SENDCNT++] = _c;
    }
    D2PRINTF(" WR=%d/%d<%02X\r\n", USBSTATE.SENDCNT, USB_BULK_SEND_MAX, _c);
    if (USBSTATE.SENDCNT >= USB_BULK_SEND_MAX) ep_send_flush();
    else enable_interrupt_sof();
    return 1;
  }

  /* Returns the number of characters available to write. */
  size_t write_available (void) {
    if (is_busy() || is_send_busy()) return 0;
    if (is_send_underflow() && USBSTATE.SENDCNT >= USB_BULK_SEND_MAX) {
      ep_send_flush();
      return 0;
    }
    return USB_BULK_SEND_MAX - USBSTATE.SENDCNT;
  }

  /* Receives one character. */
  int read_byte (void) {
    int _c = -1;
    /* It won't work if the receive buffer is unreadable. */
    if (read_available() > 0) {
      ep_recv_pending();
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _c = USB_RECV_BUFFER[USBSTATE.RECVCNT++];
      }
      /* When it reaches the end, it requests that the buffer be updated. */
      if (USB_EP_RECV.CNT == USBSTATE.RECVCNT) ep_recv_flush();
    }
    return _c;
  }

  /* Returns the number of unread characters. */
  size_t read_available (void) {
    size_t _s = 0;
    if (is_ready() && is_recv_ready()) {
      /* When it reaches the end, it requests that the buffer be updated. */
      _s = USB_EP_RECV.CNT - USBSTATE.RECVCNT;
      if (_s == 0) ep_recv_flush();
    }
    return _s;
  }

  /* Returns the last character of the unread buffer. */
  int peek_byte (void) {
    return USB_EP_RECV.CNT == USBSTATE.RECVCNT ? -1 : USB_RECV_BUFFER[USBSTATE.RECVCNT];
  }

  /* Fills the given receive buffer until either a timeout occurs or the given character arrives. */
  size_t read_bytes (void* _buffer, size_t _limit, char _terminate, uint8_t _swevent) {
    size_t   _length  = 0;
    uint16_t _timeout = 0;
    do {
      if (is_busy()) return _length;
      ep_interrupt_pending();
      _timeout += USB_INTR_INTERVAL;
      if (_timeout >= USBSTATE.TIMEOUT) break;
      EVSYS_SWEVENTA = _swevent;
      _swevent = 0;
      if (read_available()) {
        int _c = read_byte();
        ((uint8_t*)_buffer)[_length++] = (uint8_t)_c;
        if (_terminate == (uint8_t)_c) break;
      }
    } while (_length < _limit);
    return _length;
  }

  /* Returns True if the specified character is in the receive buffer. */
  bool find_byte (const uint8_t _c) {
    bool _r = false;
    size_t _i = USBSTATE.RECVCNT;
    while (!_r && _i < USB_EP_RECV.CNT) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _r = (_c == USB_RECV_BUFFER[_i++]);
      }
    }
    return _r;
  }

} /* USB_CDC */

#endif
// end of code
