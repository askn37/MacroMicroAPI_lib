/**
 * @file XRTC_CenturyDays.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <XRTC_PCF85063A.h>

#define TZ_OFFSET (0)

XRTC_PCF85063A XRTC = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinControlRegister(PIN_PF1) = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);

  Wire.initiate(TWI_SM);

  XRTC.reset();
  if (!XRTC.update()) {
    Serial.println(F("Failed XRTC"));
    for (;;);
  }

  XRTC.startPeriodTimer(1);
}

EMPTY_INTERRUPT(PORTF_PORT_vect);

void loop (void) {
  /* normal */
  /* 2000-01-01 mjd:51544 to 2099-12-31 mjd:88068 */

  /* extended */
  /* 1900-01-01 mjd:15020 to 2299-12-31 mjd:161116 */

  bcddate_t years[] = {
    0x19000228,             0x19000301, 0x19991231,
    0x20000228, 0x20000229, 0x20000301, 0x20991231,
    0x21000228,             0x21000301, 0x21991231,
    0x22000228,             0x22000301, 0x22991231
  };

  for (auto bcddate: years) {
    bcddatetime_t t_bcd;
    t_bcd.date = bcddate;
    t_bcd.time = 0x235957;
    Serial.printf(F("\r\nMJD: %ld  Wd: %d  Date: %08lx  adj:%d\r\n\r\n"),
      bcdDateToMjd(t_bcd.date)
    , bcdDateToWeekday(t_bcd.date)
    , t_bcd.date
    , XRTC.adjustBcdDateTime(t_bcd)
    );
    for (int i = 0; i < 5; i++) {
      Serial.flush();
      PORTF_INTFLAGS = PIN1_bm;
      sleep_cpu();
      digitalWrite(LED_BUILTIN, TOGGLE);
      XRTC.update();
      XRTC.clearTimerFlag();
      t_bcd = XRTC.getCenturyDateTimeNow();
      Serial.printf(F("MJD: %ld  Wd: %d  Date: %08lx  Time: %06lx  Wd: %d  Epoch: %lu\r\n"),
        bcdDateToMjd(t_bcd.date)
      , XRTC.getWeekdays()
      , t_bcd.date
      , t_bcd.time
      , bcdDateToWeekday(t_bcd.date)
      , XRTC.getCenturyEpochNow() - TZ_OFFSET
      );
    }
  }
}

// end of code
