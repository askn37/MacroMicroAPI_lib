/**
 * @file NVM_sample.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <FlashNVM.h>

#if (INTERNAL_SRAM_SIZE < 256)
  #error This MCU not supported
  #include BUILD_STOP
#endif

struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
};

extern const uint8_t* __data_load_end;

// const struct nvm_store_t nvm_store PGM_ALIGN ROMEM = {0, __DATE__ " " __TIME__, 0xABCD};
const struct nvm_store_t nvm_store PGM_ALIGN NVMEM = {};

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* Printed system parameters */
  Serial.print(F(" PROGMEM_PAGE_SIZE=")).println(PROGMEM_PAGE_SIZE, DEC);
  Serial.print(F(" PROGMEM_END=0x")).println(PROGMEM_END, HEX);
  Serial.print(F(" upload_end=0x")).println(pgm_get_far_address(__data_load_end), HEX);
  Serial.print(F(" nvm_store=0x")).println(pgm_get_far_address(nvm_store), HEX);

  /* Support check */
  if (! FlashNVM::spm_support_check()) {
    Serial.println(F(" SPM_CHECK=<nosupported>"));
    return;
  }
  Serial.println(F(" SPM_CHECK=<supported>"));

  /* SRAM <- NVM structure copy */
  struct nvm_store_t nvm_buffer;
  memcpy_PF(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer));

  /* Before NVM check */
  /* Before NVM check */
  if (nvm_buffer.magic != 0xABCD) {
    Serial.println(F("*reinit*"));
    Serial.println(F("*reinit*"));
    // Serial.print(F(" before=0x")).println(nvm_buffer.magic, HEX);
    strcpy(nvm_buffer.datetime, __DATE__ " " __TIME__);
    nvm_buffer.count = 0;
    nvm_buffer.magic = 0xABCD;
  }
  Serial.print(F(" datetime=")).println(nvm_buffer.datetime);
  Serial.print(F(" count=")).println(nvm_buffer.count, DEC);

  /* Fix new parameter */
  nvm_buffer.count++;

  /* Fix new parameter */
  /* Fix new parameter */

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if ( FlashNVM::page_erase_P(&nvm_store, sizeof(nvm_buffer))
      && FlashNVM::page_update_P(&nvm_store, &nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[success]"));
    else Serial.println(F("*failed*"));
  }
}

void loop (void) {
  Serial.println(F("<Hit ENTER to continue>"));
  while( Serial.read() != '\n' );

  Serial.println(F("<Going reset>"));
  Serial.flush();

  /* Watch Dog Timer delay after reset */
  loop_until_bit_is_clear(WDT_STATUS, WDT_SYNCBUSY_bp);
  _PROTECTED_WRITE(WDT_CTRLA, WDT_PERIOD_8CLK_gc);
  for (;;);
}

// end of code
