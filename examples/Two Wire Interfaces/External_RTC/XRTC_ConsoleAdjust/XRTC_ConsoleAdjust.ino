/**
 * @file XRTC_ConsoleAdjust.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <XRTC_PCF85063A.h>

#define TZ_OFFSET (9 * 3600)

XRTC_PCF85063A XRTC = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.println(F("[prefix: ':' time, '/' date, '=' unix-epoch, '+|-' shift seconds]"));

  Wire.initiate(TWI_SM);
}

void loop (void) {
  digitalWrite(LED_BUILTIN, TOGGLE);
  XRTC.update();
  bcddatetime_t t_bcd = XRTC.getCenturyDateTimeNow();
  Serial.printf(F("Date: %08lx  Week: %d  Time: %06lx  Epoch: %lu\r\n"),
    t_bcd.date
  , XRTC.getWeekdays()
  , t_bcd.time
  , XRTC.getCenturyEpochNow() - TZ_OFFSET
  );
  char buff[16];
  size_t length;
  while (0 == (length = Serial.readBytes(&buff, sizeof(buff) - 1, '\n')));
  buff[length] = 0;
  if (buff[0] == '=') {
    uint32_t decnum = strtol(((const char*)&buff) + 1, NULL, 10);
    Serial.print(F("Adjust Epoch: ")).println(decnum, SIGN);
    XRTC.adjustEpoch((time_t)decnum + TZ_OFFSET);
  }
  else if (buff[0] == '-' || buff[0] == '+') {
    int32_t decnum = strtol(((const char*)&buff), NULL, 10);
    Serial.print(F("Adjust Seconds: ")).println(decnum, SIGN);
    XRTC.update();
    decnum += XRTC.getEpochNow();
    XRTC.adjustEpoch((time_t)decnum);
  }
  else if (buff[0] == ':') {
    uint32_t hexnum = strtol(((const char*)&buff) + 1, NULL, 16);
    Serial.print(F("Adjust Time: ")).println(hexnum, HEX);
    XRTC.adjustBcdTime((bcdtime_t)hexnum);
  }
  else if (buff[0] == '/') {
    uint32_t hexnum = strtol(((const char*)&buff) + 1, NULL, 16);
    Serial.print(F("Adjust Date: ")).println(hexnum, HEX);
    XRTC.adjustBcdDate((bcddate_t)hexnum);
  }
}

// end of code
