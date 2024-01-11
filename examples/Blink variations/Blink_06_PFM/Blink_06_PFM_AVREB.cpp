/**
 * @file Blink_09_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#if defined(AVR_AVREB)

#include <math.h>

#define BLINK_HZ (0.5)

void setup (void) {
  uint8_t periodic = sqrt((F_CPU / 1024.0) * (1.0 / BLINK_HZ)) - 0.5;
  uint8_t harfperi = periodic >> 1;

  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;             // PIN_PA7 <-- EVOUTA
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT0_gc;               // <-- LUT0OUT
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;           // --> EVOUTA

  CCL_TRUTH0 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
  CCL_LUT0CTRLB = CCL_INSEL0_TCE0_gc | CCL_INSEL1_TCB1_gc;  // <-- WOA0 XOR WOB1
  CCL_LUT0CTRLA = CCL_ENABLE_bm;
  CCL_CTRLA = CCL_ENABLE_bm;                                // --> LUT0OUT

  TCE0_PER = periodic - 1;
  TCE0_CMP0 = harfperi;
  TCE0_CTRLB = TCE_WGMODE_SINGLESLOPE_gc;
  TCE0_CTRLA = TCE_ENABLE_bm
             | TCE_CLKSEL_DIV1024_gc;

  TCB1_CCMP = (harfperi << 8) | periodic;
  TCB1_CTRLB = TCB_CNTMODE_PWM8_gc;                         // --> WOB1
  TCB1_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_TCE0_gc;          // <-- CLK_TCE

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

void loop (void) {
  sleep_cpu();
}

#endif
// end of code
