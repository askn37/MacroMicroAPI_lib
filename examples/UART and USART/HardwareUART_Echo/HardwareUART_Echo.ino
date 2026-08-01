/**
 * @file HardwareUART_Echo.ino
 * @author askn (K.Sato) multix.jp
 * @brief Example of using HardwareUART class
 * @version 0.2
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  /* Flashes at 0.5 Hz */
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC16384_gc;

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.print(F("_AVR_IOXXX_H_=")).println(_AVR_IOXXX_H_);
  Serial.print(F("CONSOLE_BAUD=")).println(CONSOLE_BAUD, DEC);
  Serial.print(F("BAUD=")).println(Serial.is_baud(), DEC);
  Serial.print(F("BUFSIZE=")).println(INTERNAL_SRAM_SIZE / 2, DEC);
}

void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 2];
  length = Serial.readBytes(&buff, sizeof(buff), '\n');
  if (length) {
    Serial.write(&buff, length);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}

/*
The_quick_brown_fox_jumps_over_the_lazy_dog. 0123456789ABCDEF0123456789abcdef
*/

// end of code
