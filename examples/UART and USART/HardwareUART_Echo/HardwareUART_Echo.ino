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

#ifdef AVR_AVRSX
  #define WDT_PERIOD WDT_PERIOD_8KCLK_gc
#else
  #define WDT_PERIOD WDT_PERIOD_256CLK_gc
#endif

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
  Serial.print(F(" RATE=")).print(Serial.is_baud(), DEC);
  Serial.print(F(" BUFSIZE=")).println(INTERNAL_SRAM_SIZE / 2, DEC);
  Serial.println(F("# It echoes back the input string. for example;"));
  Serial.println(F("The quick brown fox jumps over the lazy dog."));
  #ifdef SW_BUILTIN
  Serial.println(F("# Pressing SW0 triggers a reboot.")).ln();
  pinMode(SW_BUILTIN, INPUT_PULLUP);
  #endif

  _PROTECTED_WRITE(WDT_CTRLA, WDT_PERIOD);
}

void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 2];
  length = Serial.readBytes(&buff, sizeof(buff), '\n');
  if (length) {
    Serial.write(&buff, length);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
  #ifdef SW_BUILTIN
  if (!digitalRead(SW_BUILTIN)) {
    Serial.println(F("<Pressing SW0>"));
    for (;;);
  }
  #endif
  wdt_reset();
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}

// end of code
