/**
 * @file Blink_09_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#if defined(AVR_MEGAAVR) || defined(AVR_MODERNAVR)

#include <math.h>

#define BLINK_HZ (0.5)

void setup (void) {
  uint8_t periodic = sqrt((F_CPU / 1024.0) * (1.0 / BLINK_HZ)) - 0.5;
  uint8_t harfperi = periodic >> 1;

#if defined(PIN_PA7) && (LED_BUILTIN == PIN_PA7)
  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;             // PIN_PA7 <-- EVOUTA
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT1_gc;               // <-- LUT1OUT
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;           // --> EVOUTA:PA7
  #define LED_CCL (0)
#elif defined(PIN_PD2) && (LED_BUILTIN == PIN_PD2)
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT1_gc;               // <-- LUT1OUT
  EVSYS_USEREVSYSEVOUTD = EVSYS_USER_CHANNEL0_gc;           // --> EVOUTD:PD2
  #define LED_CCL (0)
#elif defined(PIN_PF2) && (LED_BUILTIN == PIN_PF2)
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT1_gc;               // <-- LUT1OUT
  EVSYS_USEREVSYSEVOUTF = EVSYS_USER_CHANNEL0_gc;           // --> EVOUTF:PF2
  #define LED_CCL (0)
#elif defined(PIN_PC6) && (LED_BUILTIN == PIN_PC6)
  PORTMUX_CCLROUTEA = PORTMUX_LUT1_ALT1_gc;
  #define LED_CCL (CCL_OUTEN_bm)
#else
  #define LED_CCL (CCL_OUTEN_bm)
#endif

#if defined(PIN_PB3) && defined(CCL_TRUTH4) && (LED_BUILTIN == PIN_PB3)
  CCL_TRUTH4 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
  CCL_LUT4CTRLB = CCL_INSEL0_TCA0_gc | CCL_INSEL1_TCB1_gc;  // <-- WOA0 XOR WOB1
  CCL_LUT4CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;             // --> LUT4_OUT:PB3
#else
  CCL_TRUTH1 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
  CCL_LUT1CTRLB = CCL_INSEL0_TCA0_gc | CCL_INSEL1_TCB1_gc;  // <-- WOA0 XOR WOB1
  CCL_LUT1CTRLA = CCL_ENABLE_bm | LED_CCL;                  // --> LUT1_OUT(PC3/6)
#endif
  CCL_CTRLA = CCL_ENABLE_bm;                                

  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0_SPLIT_LPER = periodic - 1;
  TCA0_SPLIT_LCMP0 = harfperi;
  TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm
                   | TCA_SPLIT_CLKSEL_DIV1024_gc;

  TCB1_CCMP = (harfperi << 8) | periodic;
  TCB1_CTRLB = TCB_CNTMODE_PWM8_gc;                         // --> WOB1
  TCB1_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_TCA0_gc;          // <-- CLK_TCA

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

void loop (void) {
  sleep_cpu();
}

#endif
// end of code
