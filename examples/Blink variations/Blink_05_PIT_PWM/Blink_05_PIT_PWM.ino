/**
 * @file Blink_05_PIT_PWM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#if !defined(PORTMUX_EVSYSROUTEA)
  #error This MCU model not supported
  #include BUILD_STOP
#endif

extern "C" void __vector_5 (void);

void setup (void) {

  /* ポート多重化器で LED1=PIN_PA7 を駆動 */
  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;

  /* 事象システムで RTC_PITクロックを LED1 に向ける */
  EVSYS_CHANNEL0 = EVSYS_CHANNEL0_RTC_PIT_DIV2048_gc;
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;

  /* RTC_PIT有効化 : 1024Hz */
  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;
  RTC_PITCTRLA = RTC_PITEN_bm;

  /* 1/1024/2048 -> 0.5Hz */

  /* Lチカ */
  pinMode(PIN_PA6, OUTPUT); // LED2
}

void loop (void) {

  /* LED2 は delay遅延で駆動 */
  delay(1000);
  digitalWriteMacro(PIN_PA6, TOGGLE);
}

// end of code
