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

#include "SerialUSB.h"

#undef Serial
#define Serial SerialUSB

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);

  /* Open the USB-CDC character device port. */
  Serial.begin(/* The BAUD parameter is not used so it can be omitted. */);
  while (!Serial);
  Serial.println(F("<SerialUSB startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.print(F("_AVR_IOXXX_H_=")).println(_AVR_IOXXX_H_);
}

void loop (void) {
  while (Serial.available() > 0) {
    int _c = Serial.read();
    Serial.write(_c);
    if (_c == '\n') digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

// end of code
