/**
 * @file UrowNVM_sample.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
#include <UrowNVM.h>
#include <TaskChanger.h>

struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
};

#if (PROGMEM_SIZE > 2048)
volatile char task1_stack[64];
void yield (void) { TaskChanger::yield(); }
#endif

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\n<startup>"));
  Serial.print(F("_AVR_IOXXX_H_ = ")).println(_AVR_IOXXX_H_);

#if (PROGMEM_SIZE > 2048)
  TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);

  Serial.print(F("USER_SIGNATURES_START=0x")).println(USER_SIGNATURES_START, HEX);
  Serial.print(F("USER_SIGNATURES_SIZE=")).println(USER_SIGNATURES_SIZE, DEC);
  Serial.printHex((const void *)USER_SIGNATURES_START, USER_SIGNATURES_SIZE, ' ', 16).ln();
#endif
  Serial.ln();

  /* SRAM <- NVM structure copy */
  struct nvm_store_t nvm_buffer;
  UrowNVM::userrow_load(&nvm_buffer, sizeof(nvm_buffer));

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
    if ( UrowNVM::userrow_clear()
      && UrowNVM::userrow_save(&nvm_buffer, sizeof(nvm_buffer))
    )    Serial.println(F("[success]"));
    else Serial.println(F("*failed*"));
  }

  if (UrowNVM::userrow_verify(&nvm_buffer, sizeof(nvm_buffer)))
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

#if (PROGMEM_SIZE > 2048)
void task1 (void) {
  while (true) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay(1000);
  }
}
#endif

// end of code
