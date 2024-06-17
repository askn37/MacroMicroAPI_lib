/**
 * @file SerialUSB_Echo.ino
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

/* USB-CDC communication class library. */
#include "SerialUSB_Class.h"

/* Volatile variable for communication with interrupt functions. */
volatile uint32_t _baud = 0;
volatile uint16_t _ss;
volatile bool _le, _ls, _cs;

/* Here are some examples of various event callbacks. */
namespace USB::CDC {
  void cb_cdc_set_lineencoding (LineEncoding_t* _lineencoding) {
    if (_baud != _lineencoding->dwDTERate) {
      _baud = _lineencoding->dwDTERate;
      _le = true;
    }
  }
  void cb_cdc_set_linestate (LineState_t*) { _ls = true; }
  void cb_cdc_set_sendbreak (uint16_t _sendbreak) { _ss = _sendbreak; }
  void cb_cdc_clear_sendbreak (void) { _baud = -1; _cs = true; }
}

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);

  /* Open UART port for debugging. */
  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("<startup>"));
  Serial.println(F("This is the UART port for debugging."));
  Serial.println(F("The USB-serial communication port should be found separately."));

  /* Open the USB-CDC communication port. */
  SerialUSB.begin(/* The BAUD parameter is not used so it can be omitted */);
  while (!SerialUSB);

  SerialUSB.println(F("<SerialUSB startup>"));
  SerialUSB.print(F("F_CPU=")).println(F_CPU, DEC);
  SerialUSB.print(F("_AVR_IOXXX_H_=")).println(_AVR_IOXXX_H_);
  SerialUSB.print(F("LED_BUILTIN=")).println(LED_BUILTIN, DEC);

  Serial.println(F("<SerialUSB startup>"));
}

void loop (void) {
  /* Echo Back Loop */
  while (SerialUSB.available()) {
    int _c = SerialUSB.read();
    if (_c < 0) break;
    SerialUSB.write(_c);
    if (_c == '\n') digitalWrite(LED_BUILTIN, TOGGLE);
    if (_le || _ls || _ss || _cs) break;
  }

  /* Retrieving and displaying information from callback functions. */
  if (_ls) {
    SerialUSB.print(F(" dtr=")).print(SerialUSB.getLineState().bStateDTR, DEC);
    SerialUSB.print(F(" rts=")).println(SerialUSB.getLineState().bStateRTS, DEC);
    _ls = false;
  }
  if (_ss) {
    SerialUSB.print(F(" break=")).println(_ss, DEC);
    _ss = 0;
  }
  if (_cs) {
    SerialUSB.println(F(" break=clear"));
    _cs = false;
  }
  if (_le) {
    SerialUSB.print(F(" baud=")).println(_baud, DEC);
    _le = false;
  }
}

/* The_quick_brown_fox_jumps_over_the_lazy_dog. */
/* 0123456789ABCDEF0123456789abcdef */

// end of code
