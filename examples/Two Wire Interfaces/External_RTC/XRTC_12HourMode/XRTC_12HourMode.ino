/**
 * @file XRTC_12HourMode.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <XRTC_PCF85063A.h>

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

	/* Set 12 hours mode and Periodic Intrrupt 1 sec */
  XRTC.set12hourMode(true).startPeriodTimer(1);
	Serial.println(F("[Demonstration Wraparound 12-Hours]"));
}

ISR(PORTF_PORT_vect) {
  PORTF_INTFLAGS = PIN1_bm;
}

void loop (void) {
	bcdtime_t times[] = { 0x315957, 0x125957, 0x115957, 0x325957 };
	for (auto bcdtime: times) {
		bcddatetime_t t_bcd = XRTC.getBcdDateTimeNow();
		t_bcd.time = bcdtime;
		XRTC.adjustBcdDateTime(t_bcd);
		for (int i = 0; i < 4; i++) {
			Serial.flush();
			sleep_cpu();
			digitalWrite(LED_BUILTIN, TOGGLE);
			XRTC.update();
			t_bcd = XRTC.getBcdDateTimeNow();
			bool ampm = t_bcd.time & 0x200000;
			bcdtime_t time12 = t_bcd.time & ~0x200000;
			Serial.printf(F("Date: %08lx  Week: %d  Time: %s %06lx (BCD:%06lx)\r\n"),
				t_bcd.date,
				XRTC.getWeekdays(),
				(ampm ? "pm" : "am"),
				time12,
				t_bcd.time
			);
		}
		Serial.println(F("---"));
	}
}

// end of code
