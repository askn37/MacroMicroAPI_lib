/**
 * @file ReadUART_Echo.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2023-12-20
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <ReadUART.h>

Settings_SerialR0A(ReadUART_BUFFSIZE);

#ifdef Serial
#undef Serial
#define Serial SerialR0A
#endif

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
}

void loop (void) {
  while (Serial.available()) {
    Serial.write( Serial.read() );
  }
}

// end of code
