/**
 * @file Logic_Analyzer.ino
 * @author askn (K.Sato) multix.jp
 * @brief Monitors a set of ports and visualizes the timeline of 0/1 states.
 * @version 0.1
 * @date 2026-08-27
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

/*
 * Set of ports to visualize.
 */
#define CPATURE_GPIN PIN_PC3
#define CAPTURE_MASK (PIN3_bm|PIN2_bm|PIN1_bm|PIN0_bm)
#define CAPTURE_TRIGER (PIN1_bm|PIN0_bm)

/*
 * Maximum number of captures (depends on the amount of SRAM implemented)
 */
#define CAPTURE_MAX (RAMSIZE / 5 - 150)

/*
 * Other macro constants.
 */
#define USEC_COUNT (uint16_t)(F_CPU / 1000000)
#define TIMEOUT_SEC (uint16_t)(F_CPU / 65536 * 2)

#include <api/capsule.h>

uint32_t _time[CAPTURE_MAX + 2];
uint8_t _capt[CAPTURE_MAX + 2];
size_t  _cmax, _ovf;

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  GPR_GPR3 = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    portRegister(CPATURE_GPIN).PINCONFIG  = /* PORT_PULLUPEN_bm | */ 0;
    portRegister(CPATURE_GPIN).PINCTRLUPD = CAPTURE_MASK;
    portRegister(CPATURE_GPIN).PINCONFIG  = /* PORT_PULLUPEN_bm | */ PORT_ISC_BOTHEDGES_gc;
    portRegister(CPATURE_GPIN).PINCTRLUPD = CAPTURE_TRIGER;

    /* TCB0の捕獲事象に接続 */
    /* TCB0とTCB1の捕獲事象に接続 */
    EVSYS_CHANNEL4     = EVSYS_CHANNEL_OFF_gc;
    EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL4_gc;
    EVSYS_USERTCB1CAPT = EVSYS_USER_CHANNEL4_gc;

    /* ch5=TCB0溢れ*/
    /* TCB1の計数事象に接続 */
    EVSYS_CHANNEL5      = EVSYS_CHANNEL_TCB0_OVF_gc;
    EVSYS_USERTCB1COUNT = EVSYS_USER_CHANNEL5_gc;

    /* TCB1は計数捕獲周波数測定動作かつ連結上位 */
    TCB1_EVCTRL = TCB_CAPTEI_bm;
    TCB1_CTRLB  = TCB_CNTMODE_FRQ_gc;
    TCB1_CTRLA  = TCB_CASCADE_bm | TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm;

    /* TCB0は計数捕獲周波数測定動作かつ連結下位 */
    /* CLK元は主クロック（F_CPU）*/
    TCB0_EVCTRL = TCB_CAPTEI_bm;
    TCB0_CTRLB  = TCB_CNTMODE_FRQ_gc;
    TCB0_CTRLA  = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;

    /* 1/4 Hz の　PIT割込 */
    RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC8192_gc;
  }
  // interrupts();

  delay(700);   /* PKOBN(PicKit On Borad Nano) のVCP仮想シリアルポートが開くのを待つ */
  Serial.begin(CONSOLE_BAUD).println(F("\n<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.print(F("_AVR_IOXXX_H_=")).println(_AVR_IOXXX_H_);
  Serial.print(F("CONSOLE_BAUD=")).println(CONSOLE_BAUD, DEC);
  Serial.print(F("CAPTURE_MAX=")).println(CAPTURE_MAX, DEC);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

void loop (void) {
  Serial.println(F("<capture wait>"));
  Serial.flush();
  digitalWriteMacro(LED_BUILTIN, LOW);
  vportRegister(CPATURE_GPIN).INTFLAGS = ~0;
  RTC_PITINTFLAGS |= RTC_PI_bm;
  _cmax = 0;
  TCB1_CNT = 0;
  _time[0] = 0;
  _capt[0] = vportRegister(CPATURE_GPIN).IN;
  bit_set(GPR_GPR3, 0);

  do { sleep_cpu(); } while (bit_is_set(GPR_GPR3, 0));

  digitalWriteMacro(LED_BUILTIN, HIGH);
  Serial.println(F("<stop>"));
  _time[1] = 0;
  uint32_t _total = 0, _temp;
  for (uint16_t _idx = 0; _idx < _cmax; _idx += 1) {
    Serial.print(_idx, DEC, 5);
    Serial.write(' ');
    _temp = _time[_idx];
    _total += _temp;
    time_print(_total);
    time_print(_temp);
    Serial.write(' ');
    Serial.print(_capt[_idx] & CAPTURE_MASK, ZBIN, 8).ln();
  }
  Serial.println(F("<Press the Enter key to the next capture>\r\n"));
  Serial.flush();
  while (!(Serial.available() && Serial.read() == '\n'));
}

void time_print (uint32_t _time) {
  Serial.print((uint32_t)(_time / USEC_COUNT), DEC, 10);
  Serial.write('.');
  Serial.print((uint32_t)(_time % USEC_COUNT), ZDEC, 3);
  Serial.write(' ');
}

/*
 * Capture Interrupt Vector
 */
ISR(portIntrruptVector(CPATURE_GPIN)) {
  GPR_GPR2 = vportRegister(CPATURE_GPIN).IN;
  EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm;
  vportRegister(CPATURE_GPIN).INTFLAGS = vportRegister(CPATURE_GPIN).INTFLAGS;
  if (bit_is_set(GPR_GPR3, 0) && ++_cmax < sizeof(_capt)) {
    _capt[_cmax] = GPR_GPR2;
    _CAPS32(_time[_cmax])->words[0] = TCB0_CCMP;
    _CAPS32(_time[_cmax])->words[1] = TCB1_CCMP;
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    /* 最初のキャプチャ後にタイムアウトPIT割込許可 */
    if (_cmax == 1) RTC_PITINTCTRL = RTC_PI_bm;
  }
  else {
    bit_clear(GPR_GPR3, 0);
  }
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS |= RTC_PI_bm;
  if (TCB1_CNT > TIMEOUT_SEC) {
    uint8_t _d = vportRegister(CPATURE_GPIN).IN;
    EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm;
    _cmax += 1;
    _capt[_cmax] = _d;
    _CAPS32(_time[_cmax])->words[0] = TCB0_CCMP;
    _CAPS32(_time[_cmax])->words[1] = TCB1_CCMP;
    RTC_PITINTCTRL &= ~RTC_PI_bm;
    bit_clear(GPR_GPR3, 0);
  }
}

// end of script
