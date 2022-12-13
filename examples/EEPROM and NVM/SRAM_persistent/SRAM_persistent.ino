/**
 * @file SRAM_persistent.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-31
 *
 * @copyright Copyright (c) 2022
 *
 */
#if (INTERNAL_SRAM_SIZE < 256)
  #error This MCU not supported
  #include BUILD_STOP
#endif

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  static struct {
    uint32_t reset_count;
    char save_string[26];
    uint16_t crc16;
  } persistent_save NIMEM;

  /* check CRC16 */
  if (crc16(&persistent_save, sizeof(persistent_save)) != 0) {
    Serial.println(F("*reinitialization*"));
    persistent_save.reset_count = 0;
    strcpy((char*)&persistent_save.save_string, __DATE__ " " __TIME__);
  }

  /* printing and reconfigure */
  Serial.print(F(" before_reset=0x")).println(GPR_GPR0, ZFILL|HEX, 2);
  Serial.print(F(" persistent_save_addr=0x")).println((int)&persistent_save, ZFILL|HEX, 4);
  Serial.print(F(" crc16=0x")).println(persistent_save.crc16, ZFILL|HEX, 4);
  Serial.print(F(" save_string=")).println((char*)&persistent_save.save_string);
  Serial.print(F(" reset_count=")).println(persistent_save.reset_count, DEC);
  Serial.flush();

  /* crc recalculation */
  persistent_save.reset_count++;
  persistent_save.crc16 = crc16(&persistent_save, sizeof(persistent_save) - sizeof(persistent_save.crc16));

  /* software reset */
  // _PROTECTED_WRITE(RSTCTRL_SWRR, 1);

  /* Watch Dog Timer delay after reset */
  loop_until_bit_is_clear(WDT_STATUS, WDT_SYNCBUSY_bp);
  _PROTECTED_WRITE(WDT_CTRLA, WDT_PERIOD_2KCLK_gc);
}

void loop (void) {}

// end of code
