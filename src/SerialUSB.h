/**
 * @file SerialUSB_Class.h
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
#include "USB/CDC.h"

#include "api/Print.h"  /* from Micro_API */

class SerialUSB_Class : public Print {
public:
  /* 
   * Application Methods 
   */

  explicit operator bool (void) { return USB_NAMESPACE::is_ready(); }
  using Print::write; // pull in write(str) and write(buf, size) from Print

  inline SerialUSB_Class& begin (void) { USB_NAMESPACE::start(); return *this; }
  inline SerialUSB_Class& begin (const uint32_t /* unused */) { USB_NAMESPACE::start(); return *this; }
  inline SerialUSB_Class& initiate (void) { USB_NAMESPACE::start(); return *this; }
  inline SerialUSB_Class& initiate (const uint16_t /* unused */) { USB_NAMESPACE::start(); return *this; }
  inline void end (void) { return USB_NAMESPACE::stop(); }

  size_t write (const uint8_t _c) { return USB_NAMESPACE::write_byte(_c); }
  int read (void) { return USB_NAMESPACE::read_byte(); }
  void flush (void) { return USB_NAMESPACE::write_flush(); }
  void clear (void) { return USB_NAMESPACE::read_clear(); }
  size_t available (void) { return USB_NAMESPACE::read_available(); }
  size_t availableForWrite (void) { return USB_NAMESPACE::write_available(); }
  int peek (void) { return USB_NAMESPACE::peek_byte(); }
  bool find (const uint8_t _c = '\n') { return USB_NAMESPACE::find_byte(_c); }

  void setTimeout (uint16_t _timeout) { USB_NAMESPACE::set_timeout(_timeout); }
  size_t readBytes (void* _buffer, size_t _limit, char _terminate = 0, uint8_t _swevent = 0) {
    return USB_NAMESPACE::read_bytes(_buffer, _limit, _terminate, _swevent);
  }

  inline void setSerialState (USB_NAMESPACE::SerialState_t& _serialstate) { USB_NAMESPACE::send_serialstate(_serialstate); }
  inline USB_NAMESPACE::LineState_t& getLineState (void) { return USB_NAMESPACE::get_line_state(); }
  inline USB_NAMESPACE::LineEncoding_t& getLineEncoding (void) { return USB_NAMESPACE::get_line_encoding(); }

};  /* SerialUSB_Class */

extern "C" {
  extern SerialUSB_Class SerialUSB;
} /* extern "C" */

#else
#error This feature is only available on the AVR-DU series.
#include BUILD_STOP
#endif
// end of header
