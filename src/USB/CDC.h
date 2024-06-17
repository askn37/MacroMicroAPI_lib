/**
 * @file USB/CDC.h
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once
#include <avr/io.h>
#if defined(__cplusplus) && defined(USB0)
#include <stddef.h>
#include "api/String.h"

#define USB_NAMESPACE USB::CDC

#include "CDC/define.h"

namespace USB_NAMESPACE {

  /*
   * Application interface
   */

  void bus_reset (void);
  void bus_detach (void);
  void bus_attach (void);

  /*** Application functions ***/

  bool is_ready (void);
  bool is_busy (void);

  void start (void);
  void stop (void);
  void flush (void);
  void clear (void);

  size_t put_byte (const uint8_t _c);
  int get_byte (void);
  size_t get_available (void);
  size_t put_available (void);
  int peek_byte (void);
  bool find_byte (const uint8_t _c = '\n');
  size_t write_bytes (const void* _buffer, size_t _length);
  size_t write_bytes (const PGM_t* _buffer, size_t _length);
  void set_timeout (uint16_t _timeout);
  size_t read_bytes (void* _buffer, size_t _limit, char _terminate = 0, uint8_t _swevent = 0);

  void send_serialstate (SerialState_t& _serialstate);
  LineState_t& get_line_state (void);
  LineEncoding_t& get_line_encoding (void);

  /*** User callback functions ***/

  void cb_bus_event_sof (void);
  void cb_bus_event_suspend (void);
  void cb_bus_event_resume (void);
  void cb_bus_event_reset (void);
  void cb_bus_event_stalled (void);
  void cb_bus_event_underflow (void);
  void cb_bus_event_overflow (void);

  void cb_cdc_set_lineencoding (LineEncoding_t* _lineencoding);
  void cb_cdc_set_linestate (LineState_t* _linestate);
  void cb_cdc_set_sendbreak (uint16_t _sendbreak);
  void cb_cdc_clear_sendbreak (void);

  /*** Handling functions ***/

  void handling_bus_events (void);
  void handling_transaction_events (void);

} /* USB_CDC */

#else
#error This feature is only available on the AVR-DU series.
#include BUILD_STOP
#endif
// end of header
