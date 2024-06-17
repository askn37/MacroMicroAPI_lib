/**
 * @file Blink_09_PFM_14P.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#if defined(AVR_TINYAVR_14) || defined(AVR_TINYAVR_20) || defined(AVR_TINYAVR_24)

#include <math.h>

#define BLINK_HZ (0.5)

void setup (void) {
  uint8_t periodic = sqrt((F_CPU / 1024.0) * (1.0 / BLINK_HZ)) - 0.5;
  uint8_t harfperi = periodic >> 1;

  CCL_TRUTH1 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
  CCL_LUT1CTRLB = CCL_INSEL1_TCA0_gc | CCL_INSEL0_TCB0_gc;  // <-- WOA1 XOR WOB0
  CCL_LUT1CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;             // --> LUT1OUT = LED_BUILTIN
  CCL_CTRLA = CCL_ENABLE_bm;

  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0_SPLIT_LPER = periodic - 1;
  TCA0_SPLIT_LCMP1 = harfperi;
  TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm
                   | TCA_SPLIT_CLKSEL_DIV1024_gc;

  TCB0_CCMP = (harfperi << 8) | periodic;
  TCB0_CTRLB = TCB_CNTMODE_PWM8_gc;                         // --> WOB0
  TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_TCA0_gc;          // <-- CLK_TCA

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

void loop (void) {
  sleep_cpu();
}

#endif
// end of code
