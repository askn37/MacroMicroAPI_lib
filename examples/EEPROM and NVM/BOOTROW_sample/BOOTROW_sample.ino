/**
 * @file BOOTROW_sample.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <FlashNVM.h>
#include <TaskChanger.h>

#ifndef BOOTROW_SIZE
  #error This sketch can only be built on AVR_DU/EB
  #include BUILD_STOP
#endif

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

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\n<startup>"));
  Serial.print(F("_AVR_IOXXX_H_ = ")).println(_AVR_IOXXX_H_);
  TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);

  /* Support check */
  if (! FlashNVM::spm_support_check()) {
    Serial.println(F("SPM_CHECK=<nosupported>"));
    return;
  }
  Serial.println(F("SPM_CHECK=<supported>")).ln();

  {
    uint8_t buf[BOOTROW_SIZE];
    FlashNVM::bootrow_load(&buf);
    Serial.print(F("BOOTROW_START=0x")).println(BOOTROW_START, HEX);
    Serial.print(F("BOOTROW_SIZE=")).println(BOOTROW_SIZE, DEC);
    Serial.printHex((const void *)&buf, BOOTROW_SIZE, ' ', 16).ln().ln();
  }

  /* SRAM <- NVM structure copy */
  struct nvm_store_t nvm_buffer;
  FlashNVM::bootrow_load(&nvm_buffer);

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
    if ( FlashNVM::bootrow_clear()
      && FlashNVM::bootrow_save(&nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[success]"));
    else Serial.print(F("*failed* 0x")).println(NVMCTRL_STATUS, ZHEX, 2);
  }

  if (FlashNVM::bootrow_verify(&nvm_buffer, sizeof(nvm_buffer)))
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
