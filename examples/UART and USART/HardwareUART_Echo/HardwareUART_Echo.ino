/**
 * @file HardwareUART_Echo.ino
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  // Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  // Serial.print(F("BAUD=")).println(Serial.is_baud(), DEC);
}

void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 4];
  length = Serial.readBytes(&buff, sizeof(buff), '\n');
  if (length) {
    Serial.write(&buff, length);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

// end of code
