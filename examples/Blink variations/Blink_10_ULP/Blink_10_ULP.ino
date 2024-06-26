/**
 * @file Blink_10_ULP.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-28
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#if !defined(PORTMUX_EVSYSROUTEA)
  #error This MCU not supported
  #include BUILD_STOP
#endif

int main (void) {
  /* ここは AVR_EX以降 かそれ以外かで変わる */
  #if AVR_EVSYS >= 201
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_OSC32K_gc);
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_DIV64_gc | CLKCTRL_PEN_bm);
  #else
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKSEL_OSC32K_gc);
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm);
  #endif

  CCL_TRUTH0 = CCL_TRUTH_1_bm;
  CCL_LUT0CTRLB = CCL_INSEL0_TCA0_gc;
  CCL_LUT0CTRLA = CCL_ENABLE_bm;
  CCL_CTRLA = CCL_ENABLE_bm;

  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT0_gc;
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;

  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0_SPLIT_LPER = UINT8_MAX;
  TCA0_SPLIT_LCMP0 = UINT8_MAX / 2;
  TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV4_gc;

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_cpu();
}

// end of code
