/**
 * @file PHIC_S9705.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-13
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

void setup (void) {
  pinModeMacro(PIN_PD2, INPUT);
  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("LX,kHz,CNT"));

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

  /* PIT有効化 */
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITCTRLA = RTC_PITEN_bm;
}

void loop (void) {
  /* 計数捕獲 */
  loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
  uint16_t _CMP = TCB0_CCMP;
  TCB0_INTFLAGS = TCB_CAPT_bm;

  /* 捕獲値を kHzに換算 */
  float _khz = (float)_CMP / (1000.0 / 16.0);

  /* kHz を lxに換算 */
  /* 1 lx = 500 Hz */
  float _lx = _khz / 0.5;
  Serial.print(F("LX=")).print(_lx).print(':').print(_lx);
  Serial.print(F(",kHz=")).print(_khz);
  Serial.print(F(",CNT=")).println(_CMP);
}

// end of code
