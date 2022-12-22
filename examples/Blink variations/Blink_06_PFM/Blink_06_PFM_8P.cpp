/**
 * @file PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#if defined(AVR_TINYAVR_8)

#include <math.h>

#define BLINK_HZ (0.5)

void setup (void) {
  uint8_t periodic = sqrt((F_CPU / 1024.0) * (1.0 / BLINK_HZ)) - 0.5;
  uint8_t harfperi = periodic >> 1;

  pinMode(LED_BUILTIN, OUTPUT);                             // --> PA3

  pinControlRegister(PIN_EVOUT0) = PORT_ISC_BOTHEDGES_gc;   // <-- LUT0_OUT

  PORTMUX_CTRLA = PORTMUX_EVOUT0_bm;
  EVSYS_ASYNCCH0 = EVSYS_ASYNCCH0_CCL_LUT0_gc;
  EVSYS_ASYNCUSER8 = EVSYS_ASYNCUSER0_ASYNCCH0_gc;

  CCL_TRUTH0 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
  CCL_LUT0CTRLB = CCL_INSEL1_TCA0_gc | CCL_INSEL0_TCB0_gc;  // <-- WOA1 XOR WOB0
  CCL_LUT0CTRLA = CCL_ENABLE_bm;                            // --> LUT0_OUT (PA6)
  CCL_CTRLA = CCL_ENABLE_bm;

  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0_SPLIT_LPER = periodic - 1;
  TCA0_SPLIT_LCMP1 = harfperi;                              // --> WOA1
  TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm
                   | TCA_SPLIT_CLKSEL_DIV1024_gc;

  TCB0_CCMP = (harfperi << 8) | periodic;
  TCB0_CTRLB = TCB_CNTMODE_PWM8_gc;                         // --> WOB0
  TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_TCA0_gc;          // <-- CLK_TCA

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

ISR(PORTA_PORT_vect) {
  portRegister(PIN_EVOUT0).INTFLAGS = _BV(pinPosition(PIN_EVOUT0));
  digitalWrite(LED_BUILTIN, TOGGLE);
}

void loop (void) {
  sleep_cpu();
}

#endif
// end of code
