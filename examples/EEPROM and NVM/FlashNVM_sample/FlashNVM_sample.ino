/**
 * @file FlashNVM_sample.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <FlashNVM.h>
#include <TaskChanger.h>

#if (INTERNAL_SRAM_SIZE < 256)
  #error This MCU not supported
  #include BUILD_STOP
#endif

volatile char task1_stack[64];
void yield (void) { TaskChanger::yield(); }

struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
};

extern const uint8_t* __data_load_end;

// const struct nvm_store_t nvm_store PGM_ALIGN ROMEM = {0, __DATE__ " " __TIME__, 0xABCD};
const struct nvm_store_t nvm_store PGM_ALIGN NVMEM = {};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\n<startup>"));
  Serial.print(F("_AVR_IOXXX_H_ = ")).println(_AVR_IOXXX_H_);
  TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);

  /* Printed system parameters */
  Serial.print(F("PROGMEM_PAGE_SIZE=")).println(PROGMEM_PAGE_SIZE, DEC);
  Serial.print(F("PROGMEM_END=0x")).println(PROGMEM_END, HEX);
  Serial.print(F("upload_end=0x")).println(pgm_get_far_address(__data_load_end), HEX);
  Serial.print(F("nvm_store=0x")).println(pgm_get_far_address(nvm_store), HEX);

  /* Support check */
  if (! FlashNVM::spm_support_check()) {
    Serial.println(F("SPM_CHECK=<nosupported>"));
    return;
  }
  Serial.println(F("SPM_CHECK=<supported>")).ln();

  /* SRAM <- NVM structure copy */
  struct nvm_store_t nvm_buffer;
  memcpy_PF(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer));
  Serial.printHex((const void *)&nvm_buffer, sizeof(nvm_buffer), ' ', 16).ln().ln();

  /* Before NVM check */
  if (nvm_buffer.magic != 0xABCD) {
    Serial.println(F("*reinit*"));
    nvm_buffer.count = ~0;
    nvm_buffer.magic = 0xABCD;
  }

  /* Fix new parameter */
  nvm_buffer.count++;
  strcpy(nvm_buffer.datetime, __DATE__ " " __TIME__);

  Serial.print(F("datetime=")).println(nvm_buffer.datetime);
  Serial.print(F("count=")).println(nvm_buffer.count, DEC).ln();

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if ( FlashNVM::page_erase_P(&nvm_store, sizeof(nvm_buffer))
      && FlashNVM::page_update_P(&nvm_store, &nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[success]"));
    else Serial.println(F("*failed*"));
  }

  if (0 == memcmp_P(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer)))
       Serial.println(F("[matched]"));
  else Serial.println(F("*mismatch*"));
}

void loop (void) {
  Serial.println(F("<Hit ENTER to continue>"));
  while( Serial.read() != '\n' ) yield();

  Serial.println(F("<Going reset>"));
  Serial.flush();

  /* Watch Dog Timer delay after reset */
  loop_until_bit_is_clear(WDT_STATUS, WDT_SYNCBUSY_bp);
  _PROTECTED_WRITE(WDT_CTRLA, WDT_PERIOD_8CLK_gc);
  for (;;);
}

void task1 (void) {
  while (true) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay(1000);
  }
}

// end of code
