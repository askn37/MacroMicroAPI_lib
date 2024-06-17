/**
 * @file Blink_07_PIT_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, TOGGLE);

  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC128_gc;

  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_INTCTRL = RTC_OVF_bm | RTC_CMP_bm;
  RTC_PER = 254;
  RTC_CMP = 128;
  RTC_CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV1_gc | RTC_RUNSTDBY_bm;

  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
}

ISR(RTC_CNT_vect) {
  RTC_INTFLAGS = RTC_OVF_bm | RTC_CMP_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}

void loop (void) {
  sleep_cpu();
}

// end of code
