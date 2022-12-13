/**
 * @file Servo_TCB2.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2022
 *
 */

/* PWM Servo neutral position = 1500 us */
#define POSITION_MIN     ( 900L * (F_CPU / 2000000L))
#define POSITION_NEUTRAL (1500L * (F_CPU / 2000000L))
#define POSITION_MAX     (2100L * (F_CPU / 2000000L))

void setup (void) {

  /* サーボ制御出力:初期状態HIGH */
  // digitalWrite(PIN_PC0, HIGH);
  pinMode(PIN_PC0, OUTPUT);

  /* シリアルプロッタ表示 */
  // Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* TCB計時器 : TCB2 WO -> PIN_PC0 */
  TCB2_CCMP = POSITION_NEUTRAL;
  TCB2_CTRLB = TCB_CCMPEN_bm | TCB_CNTMODE_SINGLE_gc;
  TCB2_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;
  TCB2_EVCTRL = TCB_EDGE_bm;
  loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);

  /* ADC0 VREF=VDD */
  VREF_ADC0REF = VREF_REFSEL_VDD_gc;

  /* ADC0 sense AIN1=PIN_PD1 */
  ADC0_MUXPOS = ADC_MUXPOS_AIN1_gc;
  ADC0_MUXNEG = ADC_MUXNEG_GND_gc;
  ADC0_CTRLD = ADC_INITDLY_DLY16_gc | ADC_SAMPDLY_DLY2_gc;
  ADC0_CTRLC = ADC_PRESC_DIV16_gc;
  ADC0_CTRLA = ADC_FREERUN_bm | ADC_RESSEL_10BIT_gc | ADC_ENABLE_bm;
  ADC0_COMMAND = ADC_STCONV_bm;
}

void loop (void) {

  ADC0_INTFLAGS = ADC_RESRDY_bm;
  loop_until_bit_is_set(ADC0_INTFLAGS, ADC_RESRDY_bp);
  int _position = map(ADC0_RES, 0, 1023, POSITION_MIN, POSITION_MAX);

  /* TCB経時器再始動 */
  TCB2_CNT = 0;
  TCB2_CCMP = _position;
  loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);

  /* 事後ディレイ */
  delay_micros(600);

  /* シリアルプロッタ表示 */
  // Serial.println(_position, DEC);
}

// end of code
