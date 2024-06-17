/**
 * @file XRTC_GPS_Adjust.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

/**
 * [M%Stack: Mini GPS/BDS Unit (AT6558) SKU:U032]
 *
 * [GPS]    [Duino]
 *  TXD --> PIN_PD2
 *  RXD <-- PIN_PD1
 *  VCC <-- VCC (3.7V〜5.5V)
 *  GND --> GND
*/

#include <XRTC_PCF85063A.h>
#include <SoftwareUART.h>
#include <GPS_RMC.h>

#define TZ_OFFSET (9 * 3600)

XRTC_PCF85063A XRTC = {Wire};
SoftwareUART_Class GPS = {PIN_PD1, PIN_PD2};
GPS_RMC_Class GPS_RMC;

const char _PCAS01_1152[] PROGMEM = "$PCAS01,5*19"; /* 115200bps */
const char _PCAS02_MIN[]  PROGMEM = "$PCAS02,1000*2E"; /* 1000ms=1Hz */
const char _PCAS03_MIN[]  PROGMEM = "$PCAS03,0,0,0,0,1,0,0,0*03"; /* RMC only */
const char _PCAS04_MIN[]  PROGMEM = "$PCAS04,1*18"; /* GPS */
const char _PCAS04_MID[]  PROGMEM = "$PCAS04,5*1C"; /* GPS+GLONASS */

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);

  GPS.begin(9600);

  // GPS.println(P(_PCAS01_1152));
  // GPS.end();
  // delay(100);
  // GPS.begin(115200);

  GPS.println(P(_PCAS02_MIN));
  GPS.println(P(_PCAS03_MIN));
  GPS.println(P(_PCAS04_MID));

  Wire.initiate(TWI_SM);
  XRTC.reset();
}

void loop (void) {
  char buff[128];
  size_t length = GPS.readBytes(&buff, sizeof(buff));
  if (0 < length && '$' == buff[0]) {
    digitalWrite(LED_BUILTIN, TOGGLE);
    if (GPS_RMC.update(&buff, length)) {
      XRTC.update();
      gpsdata_t gpsdata = GPS_RMC.getData();
      if (gpsdata.update.stamp) {
        time_t gps_time = bcdDateTimeToEpoch(gpsdata.stamp) + TZ_OFFSET;
        time_t xrtc_time = XRTC.getEpochNow();
        if (gps_time != xrtc_time) {
          XRTC.stop();
          if (XRTC.adjust(gps_time)) Serial.println(F("RTC: Adjust"));
          XRTC.start();
          XRTC.update();
        }
        Serial.print(F("GPS: "));
        Serial.print(gpsdata.stamp.date, ZHEX, 8);
        Serial.print(' ');
        Serial.print(gpsdata.stamp.time, ZHEX, 6);
        if (gpsdata.update.coordinate) {
          Serial.print(' ');
          Serial.print(gpsdata.coordinate.latitude / 6000000.0, 7);
          Serial.print(',');
          Serial.print(gpsdata.coordinate.longitude / 6000000.0, 7);
          digitalWrite(LED_BUILTIN, TOGGLE);
        }
        Serial.ln();
      }
      bcddatetime_t t_bcd = XRTC.getBcdDateTimeNow();
      Serial.print(F("RTC: "));
      Serial.print(t_bcd.date, ZHEX, 8);
      Serial.print(' ');
      Serial.print(t_bcd.time, ZHEX, 6);
      Serial.ln();
    }
    else {
      Serial.println(F("nan nan nan"));
    }
  }
}

// end of code
