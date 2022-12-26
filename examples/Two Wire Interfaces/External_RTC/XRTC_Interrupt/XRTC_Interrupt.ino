/**
 * @file XRTC_Interrupt.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <XRTC_PCF85063A.h>

#define XRTC_PERIOD_TIMER_INTERVAL 7
#define XRTC_PERIOD_ALARM_INTERVAL 17

XRTC_PCF85063A XRTC = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinControlRegister(PIN_PF1) = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);

  Wire.initiate(TWI_SM);

  if (!XRTC.update()) {
    Serial.println(F("Failed XRTC"));
    for (;;);
  }

  if (XRTC.isPowerDown()) {
    Serial.println(F("isPowerDown"));

    // bcddatetime_t _bcddatetime = buildtime("08:34:56 Nov 18 2022");
    bcddatetime_t _bcddatetime = buildtime(__TIME__ " " __DATE__);
    if (XRTC.adjust(_bcddatetime)) {
      Serial.print(F("Adjust buildtime: "));
      printDateTime(Serial, _bcddatetime); Serial.ln();
    }
    else {
      Serial.println(F("Adjust failed"));
    }
  }

  makeAlarm();

  XRTC
    .set12hourMode(false)
    .setMinuteInterruptEnable(false)
    .setHarfMinuteInterruptEnable(true)
    .activeTimer()
  ;

  XRTC.startPeriodTimer(XRTC_PERIOD_TIMER_INTERVAL);

  if (XRTC.is12hourMode()) Serial.println(F("is12hourMode"));
  if (XRTC.isCapacitorSelection()) Serial.println(F("isCS"));
  if (XRTC.isMinuteInterruptEnable()) Serial.println(F("isMIE"));
  if (XRTC.isHarfMinuteInterruptEnable()) Serial.println(F("isHMIE"));

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

EMPTY_INTERRUPT(PORTF_PORT_vect);

void loop (void) {
  digitalWrite(LED_BUILTIN, TOGGLE);

  XRTC.update();

  if (XRTC.isPowerDown()) Serial.println(F("isPowerDown"));
  if (XRTC.isStopped()) Serial.println(F("isStopped"));
  if (XRTC.isCorrectionInterruptEnable()) Serial.println(F("isCIE"));

  if (XRTC.isTimer()) {
    Serial.println(F("isTimer"));
    XRTC.clearTimerFlag();
  }

  if (XRTC.isAlarm()) {
    Serial.println(F("isAlarm"));
    XRTC.clearAlarmFlag();
    makeAlarm();
  }

  bcddatetime_t _bcddatetime = XRTC.getBcdDateTimeNow();

  printDateTime(Serial, _bcddatetime); Serial.print(' ');
  Serial.println(XRTC.getEpochNow(), DEC);
  Serial.flush();

  PORTF_INTFLAGS = PIN1_bm;
  sleep_cpu();
}

//
// Alarm
//
void makeAlarm (void) {
  time_t epoch = XRTC.getEpochNow();
  bcddatetime_t _alarm = epochToBcdDateTime(epoch + XRTC_PERIOD_ALARM_INTERVAL);
  Serial.print(F("Next Alarm Time: "));
  printDateTime(Serial, _alarm); Serial.ln();
  XRTC.setAlarmSettings(_alarm.time, (XRTC_ALARM_SETTINGS){
    0, true, true, true, false, false
  }).activeAlarm(true);
}

//
// printDateTime
//
template<class T>
void printDateTime (T UART, bcddatetime_t BCD) {
  UART
    .print(_BCDDT(BCD)->col.year, ZHEX, 4)
    .print('-')
    .print(_BCDDT(BCD)->col.month, ZHEX, 2)
    .print('-')
    .print(_BCDDT(BCD)->col.day, ZHEX, 2)
    .print(' ')
    .print(_BCDDT(BCD)->col.hour, ZHEX, 2)
    .print(':')
    .print(_BCDDT(BCD)->col.minute, ZHEX, 2)
    .print(':')
    .print(_BCDDT(BCD)->col.second, ZHEX, 2)
  ;
}

//
// buildtime
//
bcddatetime_t buildtime (const char* _timedate) {
  // Ex) _datetime = "12:34:56 May 31 2018"
  // Serial.println(_timedate);
  char * _str = (char*)_timedate;
  uint8_t x, y, m, d, hh, mm, ss;
  x = stob(&_str[16]);
  y = stob(&_str[18]);
  d = stob(&_str[13]);
  switch (_str[9]) {
    case 'J': m = (_str[10] == 'a') ? 0x01 : (m = _str[11] == 'n') ? 0x06 : 0x07; break;
    case 'F': m = 0x02; break;
    case 'A': m = _str[11] == 'r' ? 0x04 : 0x08; break;
    case 'M': m = _str[11] == 'r' ? 0x03 : 0x05; break;
    case 'S': m = 0x09; break;
    case 'O': m = 0x10; break;
    case 'N': m = 0x11; break;
    case 'D': m = 0x12; break;
    default:
      d = stob(&_str[9]);
      m = stob(&_str[12]);
      x = stob(&_str[15]);
      y = stob(&_str[17]);
  }
  hh = stob(&_str[0]);
  mm = stob(&_str[3]);
  ss = stob(&_str[6]);
  bcddatetime_t _bcddatetime;
  _BCDDT(_bcddatetime)->col.second = ss;
  _BCDDT(_bcddatetime)->col.minute = mm;
  _BCDDT(_bcddatetime)->col.hour = hh;
  _BCDDT(_bcddatetime)->col.day = d;
  _BCDDT(_bcddatetime)->col.month = m;
  _BCDDT(_bcddatetime)->bytes[6] = y;
  _BCDDT(_bcddatetime)->bytes[7] = x;
  return _bcddatetime;
}
// end of code
