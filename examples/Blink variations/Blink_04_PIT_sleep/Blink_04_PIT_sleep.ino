/**
 * @file Blink_04_PIT_sleep.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-03
 *
 * @copyright Copyright (c) 2022
 *
 */

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
}

void sleep_ms (uint32_t _ms) {
  _ms = (_ms + (_ms >> 6) + (_ms >> 7) + 1024) >> 10;
  RTC_PITINTCTRL = RTC_PI_bm;
  do power_down(); while (--_ms > 0);
  RTC_PITINTCTRL = 0;
}

void loop (void) {
  digitalWrite(LED_BUILTIN, TOGGLE);
  sleep_ms(1000);
}

// end of code
