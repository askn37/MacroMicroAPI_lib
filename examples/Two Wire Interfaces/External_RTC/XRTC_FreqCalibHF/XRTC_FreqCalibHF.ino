/**
 * @file XRTC_FreqCalib.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-21
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <api/capsule.h>
#include <XRTC_PCF85063A.h>
XRTC_PCF85063A XRTC = {Wire};

#define INTERVAL_PERIOD 1
#define ENABLE_FREQCARIB_EXTERNAL
#define ENABLE_FREQCARIB_AUTOTUNE
// #define ENABLE_FREQCARIB_USER +1

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinControlRegister(PIN_PF1) = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;

#ifdef ENABLE_FREQCARIB_EXTERNAL
  {
    _PROTECTED_WRITE(CLKCTRL_XOSC32KCTRLA, CLKCTRL_SEL_bm | CLKCTRL_ENABLE_bm);
  }
#else
  {
    _PROTECTED_WRITE(CLKCTRL_XOSC32KCTRLA, CLKCTRL_LPMODE_bm);
  }
#endif

#ifdef ENABLE_FREQCARIB_AUTOTUNE
  {
    uint8_t fix = CLKCTRL_OSCHFCTRLA | CLKCTRL_AUTOTUNE_bm;
    _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, fix);
  }
#elif ENABLE_FREQCARIB_USER
  {
    _PROTECTED_WRITE(CLKCTRL_OSCHFTUNE, ENABLE_FREQCARIB_USER);
  }
#endif

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);

  /* TCB0とTCB1の捕獲事象を用意 */
  EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL4_gc;
  EVSYS_USERTCB1CAPT = EVSYS_USER_CHANNEL4_gc;

  /* TCB0溢れを TCB1の計数事象に接続 */
  EVSYS_CHANNEL5 = EVSYS_CHANNEL5_TCB0_OVF_gc;
  EVSYS_USERTCB1COUNT = EVSYS_USER_CHANNEL5_gc;

  /* TCB1は計数捕獲周波数測定動作かつ連結上位 */
  TCB1_EVCTRL = TCB_CAPTEI_bm;
  TCB1_CTRLB = TCB_CNTMODE_FRQ_gc;
  TCB1_CTRLA = TCB_RUNSTDBY_bm | TCB_CASCADE_bm | TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm;

  /* TCB0は計数捕獲周波数測定動作かつ連結下位 */
  /* CLK元は主クロック（F_CPU）*/
  TCB0_EVCTRL = TCB_CAPTEI_bm;
  TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
  TCB0_CTRLA = TCB_RUNSTDBY_bm | TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;

  Wire.initiate(TWI_SM);
  XRTC.startPeriodTimer(INTERVAL_PERIOD);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

ISR(PORTF_PORT_vect) {
  PORTF_INTFLAGS = PIN1_bm;
}

void loop (void) {
  static float _old = 0.0;
  uint32_t _count;
  Serial.flush();
  digitalWrite(LED_BUILTIN, TOGGLE);
  sleep_cpu();
  EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm;	/* 計数捕獲 */
  _CAPS32(_count)->words[0] = TCB0_CCMP;
  _CAPS32(_count)->words[1] = TCB1_CCMP;
  float _ppm = 1000000.0 * _count / INTERVAL_PERIOD / F_CPU - 1000000.0;
  if ((_ppm - _old) < 500 && -500 < (_ppm - _old)) {
    Serial.print(F("F_CPU=")).print(_count / INTERVAL_PERIOD, DEC).print(',');
    Serial.print(_ppm, 3).print("ppm:");
    Serial.println(_ppm, 3);
  }
  _old = _ppm;
}
// end of code
