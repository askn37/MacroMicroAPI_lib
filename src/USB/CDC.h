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
#if (F_CPU < 12000000L)
#error Please specify an MCU operating speed (F_CPU) of 12MHz (12000000L) or higher.
#include BUILD_STOP
#endif
#include <stddef.h>

/* If you fork and create a custom build, you will need to change this name. */
#define USB_NAMESPACE USB::CDC

#include "CDC/define.h"

namespace USB_NAMESPACE {

  /*
   * Application interface
   */

  void bus_reset (void);
  void bus_detach (void);
  void bus_attach (void);
  void bus_resume (void);

  /*** Application functions ***/

  bool is_ready (void);
  bool is_busy (void);
  bool is_recv_ready (void);
  bool is_send_ready (void);

  Interface_State* get_state (void);
  uint16_t get_frame (void);

  void start (void);
  void stop (void);

  void write_flush (void);
  void read_clear (void);

  size_t write_byte (const uint8_t _c);
  size_t write_available (void);
  int read_byte (void);
  size_t read_available (void);
  int peek_byte (void);
  void set_timeout (uint16_t _timeout);
  size_t read_bytes (void* _buffer, size_t _limit, char _terminate = 0, uint8_t _swevent = 0);
  bool find_byte (const uint8_t _c = '\n');

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
  void cb_bus_event_start (void);
  void cb_bus_event_stop (void);
  bool cb_bus_check (void);

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
