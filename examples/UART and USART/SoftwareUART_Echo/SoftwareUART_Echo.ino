/**
 * @file SoftwareUART_Echo.ino
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <SoftwareUART.h>

SoftwareUART_Class SoftSerial = {PIN_USART0_TXD, PIN_USART0_RXD};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  SoftSerial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  // SoftSerial.print(F("F_CPU=")).println(F_CPU, DEC);
  // SoftSerial.print(F("BAUD=")).println(SoftSerial.is_baud(), DEC);
}

void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 4];
  length = SoftSerial.readBytes(&buff, sizeof(buff), '\n');
  if (length) {
    SoftSerial.write(&buff, length);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

// end of code
