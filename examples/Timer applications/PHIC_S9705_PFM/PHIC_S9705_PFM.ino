/**
 * @file PHIC_S9705_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * HAMAMATSU PHOTO IC S9705
 *      |
 * RESET:1 --> GND
 *   GND:2 --> GND
 *   VDD:3 <-- IOREF
 *   OUT:4 --> PIN_PD2
 *             |
 *             modernAVR
 */
#define FREQ_SCALE 516

void setup (void) {
  pinModeMacro(PIN_PD2, INPUT);
  pinModeMacro(LED_BUILTIN, OUTPUT);

  /* TCB0の計数を捕獲する */
  /* DIV2048 == 16 Hz 周期 */
  EVSYS_CHANNEL2 = EVSYS_CHANNEL2_RTC_PIT_DIV2048_gc;
  EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;

  /* S9705からの入力を TCB0で計数する */
  EVSYS_CHANNEL3 = EVSYS_CHANNEL3_PORTD_PIN2_gc;
  EVSYS_USERTCB0COUNT = EVSYS_USER_CHANNEL3_gc;

  /* TCB0を計数捕獲モードに設定 */
  TCB0_EVCTRL = TCB_CAPTEI_bm;     /* 事象捕獲有効 */
  TCB0_CTRLB = TCB_CNTMODE_FRQ_gc; /* 計数捕獲モード */
  TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc; /* 事象から計数入力 */

  /* TCB1を周期割込モードに設定 */
  TCB1_CCMP = ~0;
  TCB1_INTCTRL = TCB_CAPT_bm;
  TCB1_CTRLB = TCB_CNTMODE_INT_gc;
  TCB1_CTRLA = TCB_ENABLE_bm;

  /* PIT有効化 */
  /* CYC64 = 512Hz PIT周期割込 */
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC64_gc;
}

volatile uint16_t new_CCMP = ~0;

/* TCB1 比較一致割込 */
ISR(TCB1_INT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  TCB1_CCMP = new_CCMP;
  TCB1_INTFLAGS = TCB_CAPT_bm;
}

/* PIT周期割込 */
ISR(RTC_PIT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  RTC_PITINTFLAGS = RTC_PI_bm;
}

void loop (void) {
  loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    new_CCMP = (F_CPU - ((uint32_t)TCB0_CCMP * (F_CPU / 1000000L))) / FREQ_SCALE;
  }
  TCB0_INTFLAGS = TCB_CAPT_bm;
}

// end of code
