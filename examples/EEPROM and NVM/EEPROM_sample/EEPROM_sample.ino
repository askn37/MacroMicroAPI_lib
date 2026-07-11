/**
 * @file EEPROM_test.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-31
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <avr/eeprom.h>

char _date_time[] EEMEM = __DATE__ " " __TIME__;

uint32_t _reset_count EEMEM = 0;

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F(" EEPROM_START=")).println(EEPROM_START, HEX);
  Serial.print(F(" DATETIME="));
  if (*((char*)&_date_time + EEPROM_START) == -1) {
    Serial.println(F("<noinit>"));
  }
  else {
    Serial.println((char*)&_date_time + EEPROM_START);
  }

  uint32_t _count = eeprom_read_dword(&_reset_count);
  Serial.print(F(" COUNT=")).println(_count++, DEC);

  eeprom_update_dword(&_reset_count, _count);
}

void loop (void) {
  Serial.println(F("<Hit ENTER to continue>"));
  while( Serial.read() != '\n' );

  Serial.println(F("<Going reset>"));
  Serial.flush();
  reboot();
}

// end of code
