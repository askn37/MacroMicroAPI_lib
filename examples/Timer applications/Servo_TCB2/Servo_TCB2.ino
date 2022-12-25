/**
 * @file Servo_TCB2.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.2
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * FS0403 Servo motor wiring : FUTABA / JS Type Connector
 *
 * ORANGE <-- PIN_PC0 (TCB2 WO DEFAULT)
 *    RED <-- IOREF (4.8V-6.0V)
 *  BROWN --> GND
 *
 * Please insert two freewheel diode
 *
 *        CATHODE ANODE
 * ORANGE ------|<|---- GND
 *    RED ------|<|---- GND
 */

#include <stdlib.h>

/* PWM Servo neutral position = 1500 us */
#define POSITION_MIN     ( 900L * (F_CPU / 2000000L)) /*  30 deg min right */
#define POSITION_NEUTRAL (1500L * (F_CPU / 2000000L)) /*  90 deg neutral   */
#define POSITION_MAX     (2100L * (F_CPU / 2000000L)) /* 150 deg max left  */

void setup (void) {
  /* サーボ制御出力 */
  pinMode(PIN_PC0, OUTPUT);

  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("input: -60 ~ 60[Hit enter] : 0 is neutral position."));

  /* TCB計時器 : TCB2 WO -> PIN_PC0 */
  TCB2_CCMP = POSITION_NEUTRAL;
  TCB2_CTRLB = TCB_CCMPEN_bm | TCB_CNTMODE_SINGLE_gc;
  TCB2_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;
  loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);
}

void loop (void) {
  long _angle;
  size_t _length;
  char _buff[5];
  _length = Serial.readBytes(&_buff, sizeof(_buff), '\n');
  if (_length) {
    _angle = strtol((const char*)&_buff, NULL, 10);
    if (-60 <= _angle && _angle <= 60) {
      TCB2_CCMP = map_long(_angle, -60, 60, POSITION_MIN, POSITION_MAX);
      TCB2_CNT = 0;
      loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);
      Serial.print(F("new angle: ")).println(_angle, DEC);
    }
  }
}

// end of code
