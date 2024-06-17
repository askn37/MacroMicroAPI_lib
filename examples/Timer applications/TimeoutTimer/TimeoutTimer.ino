/**
 * @file TimeoutTimer.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <TimeoutTimer.h>

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
}

void loop (void) {
  /* タイムアウトで無限ループからでも強制脱出 */
  TIMEOUT_BLOCK(1000) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    Serial.println(TimeoutTimer::millis_left(), DEC);
    while (true) {
      /* STUB */
      /* 途中で抜けることもできる */
      /* TimeoutTimer::abort(); */
    }
  }

  // /* 普通の delay : yield を中で呼ぶ（マルチタスク切替対応） */
  // digitalWriteMacro(LED_BUILTIN, TOGGLE);
  // Serial.println(TimeoutTimer::millis_left(), DEC);
  // delay_timer(1000);

  // /* 指定時間を過ぎたら真になる（繰り返し） */
  // static uint32_t _check1 = 0, _check2 = 0;
  // if ( interval_check_timer( _check1, 30 ) ) digitalWriteMacro(LED_BUILTIN, TOGGLE);
  // if ( interval_check_timer( _check2, 31 ) ) digitalWriteMacro(LED_BUILTIN, TOGGLE);

  // /* 指定した時間 CPU を休止状態に */
  // digitalWriteMacro(LED_BUILTIN, TOGGLE);
  // Serial.println(TimeoutTimer::millis_left(), DEC);
  // Serial.flush();
  // sleep_cpu_timer(1000);
}

// end of code
