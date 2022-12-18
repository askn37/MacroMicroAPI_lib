/**
 * @file FlashNVM_fillspace.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <FlashNVM.h>

#if (INTERNAL_SRAM_SIZE < 256)
  #error This MCU not supported
  #include BUILD_STOP
#endif

/*
 * Select NVM store type
 */
#define NVM ROMEM /* Static initialize section */
// #define NVM NVMEM /* No initialize section */

extern const uint8_t* __data_load_end;
extern const uint8_t* __nvmem_start;
extern const uint8_t* __nvmem_end;

struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
};

/* AVR DA/DB/DD/EA */
#if (PROGMEM_PAGE_SIZE == 512)

  #if (PROGMEM_SIZE > 0x10000)
    /* 128KiB */
    #define NVM_STORE0 (16 * 1)
    #define NVM_STORE1 (16 * 62)
  #elif (PROGMEM_SIZE > 0x8000)
    /* 64KiB */
    #define NVM_STORE0 (16 * 1)
    #define NVM_STORE1 (16 * 30)
  #elif (PROGMEM_SIZE > 0x4000)
    /* 32KiB */
    #define NVM_STORE0 (16 * 1)
    #define NVM_STORE1 (16 * 14)
  #else
    /* 16KiB */
    #define NVM_STORE0 (16 * 1)
    #define NVM_STORE1 (16 * 6)
  #endif

/* ATmega48nn and ATtiny32nn */
#elif (PROGMEM_PAGE_SIZE == 128)

  #if (PROGMEM_SIZE > 0x8000)
    /* 48KiB */
    #define NVM_STORE0 (4 * 3)
    #define NVM_STORE1 (4 * 89)
  #elif (PROGMEM_SIZE > 0x4000)
    /* 32KiB */
    #define NVM_STORE0 (4 * 3)
    #define NVM_STORE1 (4 * 57)
  #endif

/* under ATmega16nn */
#elif (PROGMEM_PAGE_SIZE == 64)

  #if (PROGMEM_SIZE > 0x2000)
    /* 16KiB */
    #define NVM_STORE0 (2 * 3)
    #define NVM_STORE1 (2 * 51)
  #elif (PROGMEM_SIZE > 0x1000)
    /* 8KiB */
    #define NVM_STORE0 (2 * 2)
    #define NVM_STORE1 (2 * 19)
  #elif (PROGMEM_SIZE > 0x800)
    /* 4KiB */
    #define NVM_STORE0 (2 * 1)
    #define NVM_STORE1 (2 * 4)
  #endif

#endif

const struct nvm_store_t nvm_store0[NVM_STORE0] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store1[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store2[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store3[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store4[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  // Serial.print(F("BAUD=")).println(Serial.is_baud(), DEC);
  Serial.print(F(" MAPPED_PROGMEM_START=0x")).println(MAPPED_PROGMEM_START, HEX);
  Serial.print(F(" MAPPED_PROGMEM_END=0x")).println(MAPPED_PROGMEM_END, HEX);
  Serial.print(F(" PROGMEM_PAGE_SIZE=")).println(PROGMEM_PAGE_SIZE, DEC);
  Serial.print(F(" PROGMEM_END=0x")).println(PROGMEM_END, HEX);
  Serial.print(F(" __data_load_end=0x")).println(pgm_get_far_address(__data_load_end), HEX);
  Serial.print(F(" __nvmem_start=0x")).println(pgm_get_far_address(__nvmem_start), HEX);
  Serial.print(F(" __nvmem_end=0x")).println(pgm_get_far_address(__nvmem_end), HEX);
  Serial.print(F(" nvm_store0=0x")).println(pgm_get_far_address(nvm_store0), HEX);
  Serial.print(F(" nvm_store1=0x")).println(pgm_get_far_address(nvm_store1), HEX);
  Serial.print(F(" nvm_store2=0x")).println(pgm_get_far_address(nvm_store2), HEX);
  Serial.print(F(" nvm_store3=0x")).println(pgm_get_far_address(nvm_store3), HEX);
  Serial.print(F(" nvm_store4=0x")).println(pgm_get_far_address(nvm_store4), HEX);
  Serial.print(F(" save_store=0x")).println(pgm_get_far_address(nvm_store0[NVM_STORE0-1]), HEX);
  Serial.print(F(" flash_free=")).println(PROGMEM_END + 1 - pgm_get_far_address(__nvmem_end), DEC);

  if (! FlashNVM::spm_support_check()) {
    Serial.println(F(" SPM_CHECK=<nosupported>"));
    return;
  }
  Serial.println(F(" SPM_CHECK=<supported>"));

  struct nvm_store_t nvm_buffer;
  memcpy_PF(&nvm_buffer, pgm_get_far_address(nvm_store0[NVM_STORE0-1]), sizeof(nvm_buffer));

  if (nvm_buffer.magic != 0xABCD) {
    Serial.println(F("*reinitialization*"));
    Serial.print(F(" before=0x")).println(nvm_buffer.magic, HEX);
    strcpy(nvm_buffer.datetime, __DATE__ " " __TIME__);
    nvm_buffer.count = 0;
    nvm_buffer.magic = 0xABCD;
  }
  Serial.print(F(" datetime=")).println(nvm_buffer.datetime);
  Serial.print(F(" count=")).println(nvm_buffer.count, DEC);

  nvm_buffer.count++;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  #if defined(HAVE_RAMPZ)
    /* over 17bit address (over 64KiB) */
    if ( FlashNVM::page_erase_PF(pgm_get_far_address(nvm_store0[NVM_STORE0-1]), sizeof(nvm_buffer))
      && FlashNVM::page_update_PF(pgm_get_far_address(nvm_store0[NVM_STORE0-1]), &nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[update success]"));
    else Serial.println(F("*update failed*"));
  #else
    /* under 16bit address (under 64KiB) */
    if ( FlashNVM::page_erase_P(&nvm_store0[NVM_STORE0-1], sizeof(nvm_buffer))
      && FlashNVM::page_update_P(&nvm_store0[NVM_STORE0-1], &nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[update success]"));
    else Serial.println(F("*update failed*"));
  #endif
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
