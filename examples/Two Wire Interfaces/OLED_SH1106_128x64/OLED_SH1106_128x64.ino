/**
 * @file OLED_SH1106_128x64.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2023-02-04
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <OLED_SH1106_128x64.h>

/* constructor and Load FontSet : 0x32-0x7F */
OLED_SH1106_Class OLED = {Wire, moderndos_8x16_vh};

/* Volatility External character user area : 0x10-0x1F */
static uint8_t extra_table[256] = {};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  /* speed selection : TWI_SM TWI_FM TWI_FMP */
  Wire.initiate(TWI_SM, false);

  /* custom pointer */
  OLED
    .clear()
    // .setFlip(true)
    // .setRevesible(true)
    .setExtTableInROM(moderndos_8x16_vh) /* 0x80-0xFF External character bank */
    .setExtTableInRAM(extra_table)    /* 0x10-0x1F Volatility External character area */
  ;

  /* Volatility External character Setup */
  for (int i = 0; i < 16; i++) {
    OLED
      .setExtFont(&extra_table[i << 4], '@' + i, OLED_SET) /* Load ROM Font */
      .setExtFont(&extra_table[i << 4], '\x0', OLED_XOR)   /* Exor Mask */
    ;
  }

  /* fill the screen with a test pattern */
  // OLED.drawTestPattern();

  /* generate character stroke weight */
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC1024_gc; /* 32768 DIV Number */

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

EMPTY_INTERRUPT(RTC_PIT_vect);

void loop (void) {

  for (int _c = 16; _c < 224; _c++) {
    OLED.write(_c);
    RTC_PITINTFLAGS = RTC_PI_bm;
    sleep_cpu(); /* comment out is max display speed */
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
  OLED.println(F("Hello World!"));
}

// end of code
