/**
 * @file XRTC_Blink_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-22
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <XRTC_PCF85063A.h>
XRTC_PCF85063A XRTC = {Wire};
void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinControlRegister(PIN_PF1) = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;

  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC512_gc; // PIT Periodic 64Hz

  Wire.initiate(TWI_SM);
  XRTC.reset();
  XRTC_TIMER_SETTINGS _settings = {
    64    // .Value           : 4096Hz / DIV64 = 64Hz
  , true  // .InterruptMode   : Periodic Interrupt
  , true  // .InterruptEnable : Interrupt Enable
  , true  // .Enable          : Timer Enable
  , 0     // .ClockFrequency  : Select Freq 4096Hz
  };
  XRTC.activateTimer(_settings);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

ISR(RTC_PIT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  RTC_PITINTFLAGS = RTC_PI_bm;
}

ISR(PORTF_PORT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  PORTF_INTFLAGS = PIN1_bm;
}

void loop (void) {
  sleep_cpu();
}
// end of code
