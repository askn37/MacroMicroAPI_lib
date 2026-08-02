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
  /* 32.768kHz ULP (低消費電力発振器) で 512Hz の主クロックを生成する */
  /* ここは AVR_Ex以降 かそれ以外かで変わる */
  #if AVR_EVSYS >= 201
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_OSC32K_gc);
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_DIV64_gc | CLKCTRL_PEN_bm);
  #else
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKSEL_OSC32K_gc);
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm);
  #endif

  /* ここからは TCE0 搭載かそれ以外かで変わる */
  #ifdef HAVE_TCE0

  /* CCL を経由する場合 */
  CCL_TRUTH0 = CCL_TRUTH_1_bm;
  CCL_LUT0CTRLB = CCL_INSEL0_TCE0_gc;   /* TCE0_WO0 を拾う */
  CCL_LUT0CTRLA =
  // CCL_OUTEN_bm |   /* これは PA3 出力ピンを有効にする */
    CCL_ENABLE_bm;
  CCL_CTRLA = CCL_ENABLE_bm;

  /* TCE0 で WO0 信号を生成する */
  TCE0_PER = UINT8_MAX;
  TCE0_CMP0 = UINT8_MAX / 2;
  TCE0_CTRLB = TCE_WGMODE_SINGLESLOPE_gc;
  TCE0_CTRLA = TCE_ENABLE_bm | TCE_CLKSEL_DIV4_gc;
  #else

  /* CCL を経由する場合 */
  CCL_TRUTH0 = CCL_TRUTH_1_bm;
  CCL_LUT0CTRLB = CCL_INSEL0_TCA0_gc;   /* TCA0_WO0 を拾う */
  CCL_LUT0CTRLA =
  // CCL_OUTEN_bm |   /* これは PA3 出力ピンを有効にする */
    CCL_ENABLE_bm;
  CCL_CTRLA = CCL_ENABLE_bm;

  /* TCA0 で WO0 信号を生成する */
  TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0_SPLIT_LPER = UINT8_MAX;
  // TCA0_SPLIT_CTRLB = TCA_SPLIT_LCMP0EN_bm; /* これは PA0 出力ピンを有効にする */
  TCA0_SPLIT_LCMP0 = UINT8_MAX / 2;
  TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV4_gc;
  #endif

  /* EVSYS 経由で PA7 出力ピンを有効にする場合 */
  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;
  // EVSYS_CHANNEL0 = EVSYS_CHANNEL0_TCA0_CMP0_LCMP0_gc;  /* TCA0_WO0 を拾う場合 */
  // EVSYS_CHANNEL0 = EVSYS_CHANNEL_TCE0_CMP0_gc;         /* TCE0_WO0 を拾う場合 */
  EVSYS_CHANNEL0 = EVSYS_CHANNEL_CCL_LUT0_gc;             /* CCL_LUT0 を経由した場合 */
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;

  power_idle();
}

// end of code
