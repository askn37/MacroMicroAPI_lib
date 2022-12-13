/**
 * @file PHOTOIC_S9705.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#define LX_PDIV 4000

void setup (void) {
	pinMode(PIN_PD2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

	/* TCB0の計数を捕獲する */
	/* 1 lx = 500 Hz */
	EVSYS_CHANNEL2 = EVSYS_CHANNEL2_RTC_PIT_DIV2048_gc;	/* DIV2048=16Hz */
	EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;

	/* S9705の出力をTCB0で計数する */
	EVSYS_CHANNEL3 = EVSYS_CHANNEL3_PORTD_PIN2_gc;
	EVSYS_USERTCB0COUNT = EVSYS_USER_CHANNEL3_gc;

	TCB0_EVCTRL = TCB_CAPTEI_bm;
	TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
	TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc;

	RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC4096_gc;

	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();

	Serial.println("LX,kHz,CNT");
}

void loop (void) {
	loop_until_bit_is_set(RTC_PITINTFLAGS, RTC_PI_bp);
	RTC_PITINTFLAGS = RTC_PI_bm;

	digitalWrite(LED_BUILTIN, TOGGLE);

	/* 計数捕獲 */
	// loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
	uint16_t _CMP = TCB0_CCMP;

	/* 捕獲値をlxに換算 */
	float _lx = (float)_CMP / (500.0 / 16.0);
	Serial.print(F("LX=")).print(_lx).print(':').print(_lx);
	Serial.print(",kHz=").print((float)_CMP / (1000.0 / 16.0));
	Serial.print(",CNT=").println(_CMP);
}

// end of code
