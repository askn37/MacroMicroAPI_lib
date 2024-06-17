/**
 * @file Blink_05_PIT_PWM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-03
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#if !defined(PORTMUX_EVSYSROUTEA)
  #error This MCU model not supported
  #include BUILD_STOP
#endif

extern "C" void __vector_5 (void);

void setup (void) {

  /* ポート多重化器で LED1=PIN_PA7 を駆動 */
  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;

  /* 事象システムで RTC_PITクロックを LED1 に向ける */
  /* ここは AVR_Ex以降 かそれ以外かで変わる */
  #if AVR_EVSYS >= 201
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_RTC_EVGEN0_gc;
  #else
  EVSYS_CHANNEL0 = EVSYS_CHANNEL0_RTC_PIT_DIV2048_gc;
  #endif
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;

  /* RTC_PIT有効化 : 1024Hz */
  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;

  /* AVR_EX では Event Generator で PITクロックを決める */
  #if defined(AVR_AVREX)
  RTC_PITEVGENCTRLA = RTC_EVGEN0SEL_DIV2048_gc;
  #endif

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
