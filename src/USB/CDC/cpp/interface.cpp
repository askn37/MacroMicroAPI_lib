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
#if defined(__cplusplus) && defined(USB0)
#include <string.h>         /* memcpy */
#include "api/macro_api.h"  /* interrupts ATOMIC_BLOCK */
#include "api/String.h"     /* PGM_t */

#include "../../CDC.h"
#include "../constants.h"

namespace USB_NAMESPACE {
  /*
   * Implementing the communication data class
   */

  /*** Here we must allocate memory and define callback functions to operate on it. ***/

  Interface_State USB_STATE;
  Interface_State* get_state (void) { return &USB_STATE; }

  /*
   * Private functions
   */

  /* Wait for the interrupt to complete. */
  void ep_interrupt_pending (void) {
    loop_until_bit_is_set(USB_EP_INTR.STATUS, USB_BUSNAK_bp);
  }

  /* Wait for reception to complete. */
  void ep_recv_pending (void) {
    loop_until_bit_is_set(USB_EP_RECV.STATUS, USB_BUSNAK_bp);
  }

  /* Wait for the transmission to complete. */
  void ep_send_pending (void) {
    loop_until_bit_is_set(USB_EP_SEND.STATUS, USB_BUSNAK_bp);
  }

  /* The next interrupt will be allowed. */
  void ep_interrupt_listen (size_t _size = 0) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      USB_EP_INTR.CNT = _size;
      USB_EP_INTR.MCNT = 0;
      loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
      USB_EP_STATUS_CLR(USB_EP_INTR_IN) = ~USB_TOGGLE_bm;
    }
  }

  /* Allow the next reception. */
  void ep_recv_listen (void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      USBSTATE.RECVCNT = 0;
      USB_EP_RECV.CNT = 0;
      loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
      USB_EP_STATUS_CLR(USB_EP_BULK_OUT) = ~USB_TOGGLE_bm;
    }
  }

  /* Allow the next transmission. */
  void ep_send_listen (void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      USB_EP_SEND.CNT = USBSTATE.SENDCNT;
      USB_EP_SEND.MCNT = 0;
      USBSTATE.SENDCNT = 0;
      loop_until_bit_is_clear(USB0_INTFLAGSB, USB_RMWBUSY_bp);
      USB_EP_STATUS_CLR(USB_EP_BULK_IN) = ~USB_TOGGLE_bm;
    }
  }

  /*
   * Callback override functions
   */

  /* Returns the device setting value. */
  uint8_t cb_get_configuration (void) { return USBSTATE.CONFIG; }

  /* This operation is called during enumeration. */
  bool cb_clear_feature (void) {
    // if (USBSTATE.SENDCNT) ep_send_listen();
    return true;
  }

  /* Prepare the device when called from enumeration. */
  /* Prepare the stream pipe for CDC here. */
  bool cb_set_configuration (uint8_t _config) {
    if (_config) {
      /* USB_EP_INTR=EP1_IN uses the TRNCOMPL interrupt. */
      USB_EP_INTR.CTRL = USB_TYPE_BULKINT_gc | USB_MULTIPKT_bm | USB_AZLP_bm | USB_EP_SIZE_gc(USB_INTR_PK_SIZE);
      USB_EP_INTR.CNT = 0;
      USB_EP_INTR.DATAPTR = (uint16_t)&USB_INTR_BUFFER;
      USB_EP_INTR.MCNT = 0;
      USB_EP_INTR.STATUS = 0;

      /* USB_EP_RECV=EP2_OUT does not use the TRNCOMPL interrupt and MULTIPKT. */
      USB_EP_RECV.CTRL = USB_TYPE_BULKINT_gc | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_BULK_RECV_MAX);
      USB_EP_RECV.CNT = 0;
      USB_EP_RECV.DATAPTR = (uint16_t)&USB_RECV_BUFFER;
      USB_EP_RECV.MCNT = USB_BULK_RECV_MAX;
      USB_EP_RECV.STATUS = 0;

      /* USB_EP_SEND=EP2_IN does not use the TRNCOMPL interrupt, only MULTIPKT. */
      USB_EP_SEND.CTRL = USB_TYPE_BULKINT_gc | USB_MULTIPKT_bm | USB_AZLP_bm | USB_TCDSBL_bm | USB_EP_SIZE_gc(USB_BULK_SEND_MAX);
      USB_EP_SEND.CNT = 0;
      USB_EP_SEND.DATAPTR = (uint16_t)&USB_SEND_BUFFER;
      USB_EP_SEND.MCNT = 0;
      USB_EP_SEND.STATUS = 0;

      USBSTATE.SENDCNT = 0;
      USBSTATE.RECVCNT = 0;
    }
    USBSTATE.CONFIG = _config;
    return !!_config;
  }

  /*
   * Class Request Enumeration Process Callback
   */
  /* This is called continuously while the CDC port is open on the host side. */
  bool cb_request_class (USB_EP_t* EP_REQ, USB_EP_t* EP_RES) {
    // uint16_t _cnt = EP_RES->CNT;
    bool listen = true;
    uint8_t bRequest = USB_SETUP_DATA.bRequest;
    D1PRINTF("CR=%02X:%02X:%04X:%04X:%04X\r\n", USB_SETUP_DATA.bmRequestType, USB_SETUP_DATA.bRequest, USB_SETUP_DATA.wValue, USB_SETUP_DATA.wIndex, USB_SETUP_DATA.wLength);
    if (bRequest == CDC_REQ_SetLineEncoding) {
      ep_setup_out_listen();
      ep_setup_out_pending();
      if (EP_REQ->CNT == USB_SETUP_DATA.wLength) {
        memcpy(&USBSTATE.sLineEncoding, &USB_HEADER_DATA, sizeof(LineEncoding_t));
        D2PRINTF("  SLE=%ld\r\n", USBSTATE.LineEncoding.dwDTERate);
        cb_cdc_set_lineencoding(&USBSTATE.LineEncoding);
      }
      EP_RES->CNT = 0;
    }
    else if (bRequest == CDC_REQ_GetLineEncoding) {
      EP_RES->CNT = sizeof(LineEncoding_t);
      memcpy(&USB_HEADER_DATA, &USBSTATE.sLineEncoding, sizeof(LineEncoding_t));
      D2PRINTF("  GLE=%ld\r\n", USBSTATE.LineEncoding.dwDTERate);
    }
    else if (bRequest == CDC_REQ_SetLineState) {
      USBSTATE.bLineState = (uint8_t)USB_SETUP_DATA.wValue;
      D2PRINTF("  SLS=%02X\r\n", USBSTATE.bLineState);
      cb_cdc_set_linestate(&USBSTATE.bmLineState);
      EP_RES->CNT = 0;
    }
    else if (bRequest == CDC_REQ_SendBreak) {
      USBSTATE.BREAKCNT = USB_SETUP_DATA.wValue;
      D2PRINTF("  SB=%04X\r\n", USBSTATE.BREAKCNT);
      if (USB_SETUP_DATA.wValue) {
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
      /* If there are any outstanding buffers, they will be sent. */
      if (USBSTATE.SENDCNT) ep_send_listen();
      /* Should I reduce sendbreak count? */
      if (0xFFFF > (uint16_t)(USBSTATE.BREAKCNT - 1)) {
        USBSTATE.BREAKCNT = USBSTATE.BREAKCNT < USB_INTR_INTERVAL ? 0 : USB_INTR_INTERVAL;
        /* The interrupt will be repeated during the countdown. */
        ep_interrupt_listen(0);
      }
      else if (USBSTATE.BREAKCNT == 0 && USBSTATE.SerialState.bBreak) {
        /* When the count reaches zero, the process is complete. */
        USBSTATE.SerialState.bBreak = false;
        send_serialstate(USBSTATE.SerialState);
        cb_cdc_clear_sendbreak();
      }
    }
  }

  /*
   * Application interface
   */

  bool is_ready (void) { return !!USBSTATE.CONFIG; }
  bool is_busy (void) { return !USBSTATE.CONFIG; }
  void cb_clear_state (void) { USBSTATE = (Interface_State){}; }

  /* Start running the application. */
  void start (void) {
    bus_attach();
    cb_clear_state();
    USB0_INTCTRLA = USB_SUSPEND_bm | USB_RESUME_bm | USB_RESET_bm | USB_STALLED_bm;
    USB0_INTCTRLB = USB_TRNCOMPL_bm | USB_SETUP_bm;
    bus_reset();
    D1PRINTF("USB0=%02x\r\n", USB0_CTRLA);
    interrupts();
  }

  /* Finishes running the application. */
  void stop (void) {
    cb_clear_state();
    bus_detach();
    USB0_CTRLA = 0;
    D1PRINTF("USB0=%02x\r\n", USB0_CTRLA);
  }

  /* Wait until the send buffer is cleared. */
  void flush (void) {
    if (is_busy()) return;
    if (USBSTATE.SENDCNT && bit_is_set(USB_EP_SEND.STATUS, USB_BUSNAK_bp)) {
      ep_send_listen();
    }
    ep_send_pending();
  }

  /* Discard the receive buffer. */
  void clear (void) {
    ep_recv_pending();
    /* Clears the current receive buffer. */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      USB_EP_RECV.CNT  = 0;
      USBSTATE.RECVCNT = 0;
    }
  }

  /*
   * Stream interface
   */

  /* Sends one character. */
  size_t put_byte (const uint8_t _c) {
    if (is_busy()) return 0;
    /* Wait until NAK is set before writing data. */
    ep_send_pending();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      USB_SEND_BUFFER[USBSTATE.SENDCNT++] = _c;
    }
    /* Once the buffer is filled to the end, it is sent immediately. */
    if (USBSTATE.SENDCNT >= USB_BULK_SEND_MAX) ep_send_listen();
    /* If not, it will send with a timer delay. */
    else ep_interrupt_listen(0);
    return 1;
  }

  /* Receives one character. */
  int get_byte (void) {
    int _c = -1;
    if (is_busy()) return _c;
    /* Are there any characters you haven't read about yet? */
    if (USBSTATE.RECVCNT < _MIN(USB_EP_RECV.CNT, USB_BULK_RECV_MAX)) {
      /* Wait until NAK is set before reading the data. */
      ep_recv_pending();
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        /* This operation must be performed with interrupts disabled. */
        _c = USB_RECV_BUFFER[USBSTATE.RECVCNT++];
      }
    }
    /* If there is no unread data, the next packet can be received. */
    if (USBSTATE.RECVCNT >= _MIN(USB_EP_RECV.CNT, USB_BULK_RECV_MAX)) ep_recv_listen();
    return _c;
  }

  /* Returns the number of unread characters. */
  size_t get_available (void) {
    if (is_busy()) return 0;
    uint16_t CNT, RECVCNT;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      CNT     = USB_EP_RECV.CNT;
      RECVCNT = USBSTATE.RECVCNT;
    }
    return _MIN(CNT, USB_BULK_RECV_MAX) - RECVCNT;
  }

  /* Returns the number of characters available to write. */
  size_t put_available (void) {
    if (is_busy()) return 0;
    uint16_t SENDCNT;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      SENDCNT = USBSTATE.SENDCNT;
    }
    return USB_BULK_SEND_MAX - SENDCNT;
  }

  /* Returns the last character of the unread buffer. */
  int peek_byte (void) {
    int _c = -1;
    if (is_busy()) return _c;
    uint16_t CNT, RECVCNT;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      CNT     = USB_EP_RECV.CNT;
      RECVCNT = USBSTATE.RECVCNT;
    }
    if (CNT != RECVCNT) {
      /* Wait until NAK is set before reading the data. */
      ep_recv_pending();
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        /* This operation must be performed with interrupts disabled. */
        _c = USB_RECV_BUFFER[USBSTATE.RECVCNT - 1];
      }
    }
    return _c;
  }

  /* Returns True if the specified character is in the receive buffer. */
  bool find_byte (const uint8_t _c) {
    bool _r = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      size_t _i = USBSTATE.RECVCNT;
      size_t _e = USB_EP_RECV.CNT;
      while (_i < USB_BULK_RECV_MAX && _i < _e) {
        if (_c == USB_RECV_BUFFER[_i++]) {
          _r = true;
          break;
        }
      }
    }
    return _r;
  }

  /* Sends the specified amount from the specified buffer. */
  /* The buffer address must not point to anything other than SRAM. */
  size_t write_bytes (const void* _buffer, size_t _length) {
    if (is_busy() || _length == 0) return 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      /* If the pending buffer is not empty, it will be sent first. */
      /* If you don't want this, run clear() first. */
      if (USBSTATE.SENDCNT) ep_send_listen();
      ep_send_pending();
      /* Change the pointer and send everything with MULTIPKT. */
      uint16_t _q = USB_EP_SEND.DATAPTR;
      USB_EP_SEND.DATAPTR = (register16_t)_buffer;
      USB_EP_SEND.CNT = _length;
      USB_EP_SEND.MCNT = 0;
      ep_send_listen();
      /* Wait for the transmission to complete and restore the pointer. */
      ep_send_pending();
      USB_EP_SEND.DATAPTR = _q;
    }
    /* The return value of this function is a dummy. */
    return _length;
  }

  /* Sends the specified amount from PROGMEM. */
  size_t write_bytes (const PGM_t* _buffer, size_t _length) {
    size_t _n = 0;
    PGM_P _p = reinterpret_cast<PGM_P>(_buffer);
    while (_length--) {
      if (is_busy()) break;
      ep_send_pending();
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        /* This operation must be performed with interrupts disabled. */
        USB_SEND_BUFFER[USBSTATE.SENDCNT++] = pgm_read_byte(_p++);
      }
      _n++;
      if (USBSTATE.SENDCNT >= USB_BULK_SEND_MAX) {
        /* Once the buffer is filled to the end, it is sent immediately. */
        ep_send_listen();
      }
    }
    if (USBSTATE.SENDCNT) ep_send_listen();
    return _n;
  }

  /* Sends the current SerialState information to the host. */
  void send_serialstate (SerialState_t &_serialstate) {
    ep_interrupt_pending();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* This operation must be performed with interrupts disabled. */
      Notification_SerialState_t *p = (Notification_SerialState_t*)&USB_INTR_BUFFER;
      p->bmRequestType  = USB_REQTYPE_DIRECTION_gm | USB_REQTYPE_CLASS_gc | USB_RECIPIENT_INTERFACE_gc; // 0xA1
      p->bRequest       = CDC_NOTIF_SerialState;  // 0x20
      p->wValue         = 0;
      p->wIndex         = 0;  // Interface#0
      p->wLength        = 2;
      p->data           = _serialstate;
      ep_interrupt_listen(sizeof(Notification_SerialState_t));
    }
  }

  /* Change the timeout for read_bytes. The default is 1000ms. */
  void set_timeout (uint16_t _timeout) { USBSTATE.TIMEOUT = _timeout; }

  /* Fills the given receive buffer until either a timeout occurs or the given character arrives. */
  size_t read_bytes (void* _buffer, size_t _limit, char _terminate, uint8_t _swevent) {
    size_t _length = 0;
    uint16_t _timeout = 0;
    do {
      if (is_busy()) return _length;
      ep_interrupt_pending();
      _timeout += USB_INTR_INTERVAL;
      if (_timeout >= USBSTATE.TIMEOUT) break;
      EVSYS_SWEVENTA = _swevent;
      _swevent = 0;
      int _c = get_byte();
      if (_c >= 0) {
        ((uint8_t*)_buffer)[_length++] = (uint8_t)_c;
        if (_terminate == (uint8_t)_c) break;
      }
    } while (_length < _limit);
    return _length;
  }

  /* Reads the current LineState_t structure. */
  LineState_t& get_line_state (void) { return USBSTATE.bmLineState; }

  /* Reads the current LineEncoding_t structure. */
  LineEncoding_t& get_line_encoding (void) { return USBSTATE.LineEncoding; }

} /* USB_CDC */

#endif
// end of code