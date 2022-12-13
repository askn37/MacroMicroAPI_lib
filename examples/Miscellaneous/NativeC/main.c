/**
 * @file main.c
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <api/macro_digital.h>
#include <api/delay_busywait.h>

__attribute__((used,naked,section(".init9")))
int main (void) {
  __asm__ __volatile__ ("CLR __zero_reg__");
    _CLKCTRL_SETUP();
  pinModeMacro(LED_BUILTIN, OUTPUT);
  while (1) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay_millis(1000);
  }
}

// end of code
