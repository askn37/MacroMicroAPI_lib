/**
 * @file SelfErase.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.2
 * @date 2023-12-09
 *
 * @copyright Copyright (c) 2023 askn37 at github.com
 *
 */

#ifndef ENABLE_MACRO_API
#define ENABLE_MACRO_API
#include <api/macro_api.h>
#endif
#include <FlashNVM.h>

alignas(PROGMEM_PAGE_SIZE) const int _ro[] PROGMEM = {};

__attribute__ ((used))
__attribute__ ((OS_main))
__attribute__ ((section (".init0")))
int main (void) {
  asm("CLR R1");
  pinMode(LED_BUILTIN, OUTPUT);
  nvmptr_t top = (nvmptr_t)_ro;
  for (;;) {
    top -= PROGMEM_PAGE_SIZE;
    FlashNVM::page_erase_PF(top);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

// end of code
