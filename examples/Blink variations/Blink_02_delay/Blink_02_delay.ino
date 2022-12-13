/**
 * @file Blink_03_delay.ino
 * @author askn (K.Sato) multix.jp
 * @brief Blink using delay timer sketch code
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2022
 *
 */

/* select use timer peripheral */
#define USE_TIMER_DELAY HAVE_TCB0

#include "timer_delay.h"

void setup (void) {
	pinModeMacro(LED_BUILTIN, OUTPUT);
	Timer::init();
}

void loop (void) {
	digitalWriteMacro(LED_BUILTIN, TOGGLE);
	Timer::delay(1000);
}

// end of code
