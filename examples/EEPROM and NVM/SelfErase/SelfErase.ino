/**
 * @file SelfErase.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <FlashNVM.h>

alignas(PROGMEM_PAGE_SIZE) const int _ro[] PROGMEM = {};

__attribute__ ((section (".init0")))
int main (void) {
  asm("CLR R1");
  pinMode(LED_BUILTIN, OUTPUT);
  uint32_t top = (uint32_t)_ro;
  for (;;) {
    top -= PROGMEM_PAGE_SIZE;
    FlashNVM::page_erase_PF(top);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}

// end of code
