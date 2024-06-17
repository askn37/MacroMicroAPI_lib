/**
 * @file Blink_08_delay_SLOW.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include "timer_delay_SLOW.h"

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  Timer::init();
}

void loop (void) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  Timer::delay(1000);
}

// end of code
