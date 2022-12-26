/**
 * @file bcdtime.h
 * @author askn (K.Sato) multix.jp
 * @brief BCD, MJD, Epoch date time convert (functions)
 * @version 0.1
 * @date 2022-09-21
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../bcddatetime.h"
#include <api/btools.h>
#include <api/capsule.h>

//
// 8bit decimal integer to bcd 4+4bit hexdecimal
//
// 0-99 to 0x00-0x99 (159 to 0xF9)
//
uint8_t dtob (const uint8_t dint) {
  return ((dint / 10) << 4) + (dint % 10);
}

//
// bcd 4+4bit hexdecimal to 8bit decimal integer
//
// 0x00-0x99 to 0-99 (0xF9 to 159)
//
uint8_t btod (const uint8_t bcd) {
  return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

//
// 16bit decimal integer to bcd 4x4bit hexdecimal
//
// 0-9999 to 0x0000-0x9999 (15999 to 0xF999)
//
uint16_t wdtob (const uint16_t dint) {
  uint16_t _r;
  _WB(_r)->bytes[0] = dtob(dint % 100);
  _WB(_r)->bytes[1] = dtob(dint / 100);
  return _r;
}

//
// bcd 4x4bit hexdecimal to 16bit decimal integer
//
// 0x0000-0x9999 to 0-9999 (0xF999 to 15999)
//
uint16_t wbtod (const uint16_t bcd) {
  return btod(_WB(bcd)->bytes[0])
       + btod(_WB(bcd)->bytes[1]) * 100;
}

//
// bcd date to integer mjd
//
// 0x18581117 to 0
// 0x19700101 to 2973483
//
date_t bcdDateToMjd (const bcddate_t bcd_date) {
  uint8_t t_day = btod(_BCDD(bcd_date)->col.day);
  uint8_t t_mon = btod(_BCDD(bcd_date)->col.month);
  uint16_t t_year = wbtod(_BCDD(bcd_date)->col.year);
  if (t_mon < 3) { t_year--; t_mon += 12; }
  return ((uint32_t)(36525UL * t_year / 100)
        + (uint32_t)(t_year / 400) - (t_year / 100)
        + (uint32_t)(3059UL * (t_mon - 2) / 100)
        + (uint32_t)t_day - 678912UL);
}

//
// bcd time to integer seconds
//
// 0x000000-0x235959 to 0-86399
//
time_t bcdTimeToSeconds (const bcdtime_t bcd_time) {
  uint8_t t_sec  = btod(_BCDT(bcd_time)->bytes[0]) % 60;
  uint8_t t_min  = btod(_BCDT(bcd_time)->bytes[1]) % 60;
  uint8_t t_hour = btod(_BCDT(bcd_time)->bytes[2]) % 24;
  uint8_t t_day  = btod(_BCDT(bcd_time)->bytes[3]);
  return (86400UL * (uint32_t)t_day
         + 3600UL * (uint32_t)t_hour
         +   60UL * (uint32_t)t_min
         +          (uint32_t)t_sec);
}
time_t bcdTimeToEpoch (const bcdtime_t bcd_time) {
  uint8_t t_sec  = btod(_BCDT(bcd_time)->bytes[0]) % 60;
  uint8_t t_min  = btod(_BCDT(bcd_time)->bytes[1]) % 60;
  uint8_t t_hour = btod(_BCDT(bcd_time)->bytes[2]) % 24;
  return ( 3600UL * (uint32_t)t_hour
         +   60UL * (uint32_t)t_min
         +          (uint32_t)t_sec);
}

//
// mjd to bcd date
//
// 0       to 0x18581117
// 2973483 to 0x19700101
//
bcddate_t mjdToBcdDate (const date_t mjd) {
  int32_t jdn = mjd + 678881L;
  uint32_t t_base = (jdn << 2)
                  + ((((((jdn + 1) << 2)
                  / 146097UL + 1) * 3) >> 2) << 2) + 3;
  uint16_t t_leap = ((t_base % 1461) >> 2) * 5 + 2;
  uint16_t t_year = t_base / 1461;
  uint8_t t_mon = t_leap / 153 + 3;
  uint8_t t_day = (t_leap % 153) / 5 + 1;
  if (t_mon > 12) { t_year++; t_mon -= 12; }
  bcddate_t t_date;
  _BCDD(t_date)->col.day   = dtob(t_day);
  _BCDD(t_date)->col.month = dtob(t_mon);
  _BCDD(t_date)->words[1]  = wdtob(t_year);
  return t_date;
}

//
// mjd to epoch
//
// 1970/01/01 to 0
// 2038/01/19 to 0x80000000
// 2106/02/07 to 0xFFFFFFFF
//
time_t mjdToEpoch (const date_t mjd) {
  return bcdToEpoch(mjdToBcdDate(mjd), 0);
}

//
// bcd date/time to epoch
//
// 0x19700101,0x000000 to 0
//
time_t bcdToEpoch (const bcddate_t bcd_date, const bcdtime_t bcd_time) {
  return (86400UL * (bcdDateToMjd(bcd_date) - 40587UL)
                  + bcdTimeToEpoch(bcd_time));
}
time_t bcdDateTimeToEpoch (const bcddatetime_t bcddatetime) {
  return (86400UL * (bcdDateToMjd(bcddatetime.date) - 40587UL)
                  + bcdTimeToEpoch(bcddatetime.time));
}

//
// bcd date/time 24hour to 12hour
//
// 0x00230000 to 0x00310000
// 0x00000000 to 0x00120000
// 0x00120000 to 0x00320000
// 0x00130000 to 0x00210000
//
bcddatetime_t bcdDateTime24to12 (const bcddatetime_t bcd_datetime) {
  bcddatetime_t t_bcd;
  _BCDDT(t_bcd)->bcd.time = bcdTime24to12(bcd_datetime.time);
  _BCDDT(t_bcd)->bcd.date = bcd_datetime.date;
  return t_bcd;
}
bcdtime_t bcdTime24to12 (const bcdtime_t bcd_time) {
  uint8_t t_hour = _BCDT(bcd_time)->col.hour;
  uint8_t t_12h = dtob((btod(t_hour) + 11) % 12 + 1);
  if (t_hour & 0x80) t_12h |= 0x20;
  bcdtime_t t_time;
  _BCDT(t_time)->words[0] = _BCDT(bcd_time)->words[0];
  _BCDT(t_time)->words[1] = t_12h;
  return t_time;
}

//
// bcd date/time 12hour to 24hour
//
// 0x00310000 to 0x00230000
// 0x00120000 to 0x00000000
// 0x00320000 to 0x00120000
// 0x00210000 to 0x00130000
//
bcddatetime_t bcdDateTime12to24 (const bcddatetime_t bcd_datetime) {
  bcddatetime_t t_bcd;
  _BCDDT(t_bcd)->bcd.time = bcdTime12to24(bcd_datetime.time);
  _BCDDT(t_bcd)->bcd.date = bcd_datetime.date;
  return t_bcd;
}
bcdtime_t bcdTime12to24 (const bcdtime_t bcd_time) {
  uint8_t t_hour = _BCDT(bcd_time)->col.hour;
  uint8_t t_24h = btod(t_hour & 0x1F) % 12;
  if (t_hour & 0x20) t_24h += 12;
  bcdtime_t t_time;
  _BCDT(t_time)->words[0] = _BCDT(bcd_time)->words[0];
  _BCDT(t_time)->words[1] = dtob(t_24h);
  return t_time;
}

//
// epoch to bcd date
//
// 0 to 0x19700101
//
bcddate_t epochToBcdDate (const time_t t_epoch) {
  return mjdToBcdDate((uint32_t)t_epoch / 86400UL + 40587UL);
}

bcdtime_t epochToBcdTime (const time_t t_epoch) {
  bcdtime_t t_time;
  _BCDT(t_time)->bytes[0] = dtob(t_epoch % 60);
  _BCDT(t_time)->bytes[1] = dtob(t_epoch / 60 % 60);
  _BCDT(t_time)->words[1] = dtob(t_epoch / 3600 % 24);
  return t_time;
}

bcdtime_t epochToBcdDayTime (const time_t t_epoch) {
  bcdtime_t t_time;
  _BCDT(t_time)->bytes[0] = dtob(t_epoch % 60);
  _BCDT(t_time)->bytes[1] = dtob(t_epoch / 60 % 60);
  _BCDT(t_time)->bytes[2] = dtob(t_epoch / 3600 % 24);
  _BCDT(t_time)->bytes[3] = dtob(t_epoch / 86400UL);
  return t_time;
}

uint8_t epochToWeekday (const time_t t_epoch) {
  return (((uint32_t)t_epoch / 86400UL + 4) % 7);
}

date_t epochToMjd (const time_t t_epoch) {
  return ((uint32_t)t_epoch / 86400UL + 40587UL);
}

bcddatetime_t epochToBcdDateTime (const time_t t_epoch) {
  bcddatetime_t t_bcd;
  _BCDDT(t_bcd)->bcd.time = epochToBcdTime(t_epoch);
  _BCDDT(t_bcd)->bcd.date = epochToBcdDate(t_epoch);
  return t_bcd;
}

uint8_t mjdToWeekday (const date_t mjd) {
  return ((mjd + 3) % 7);
}

uint8_t bcdDateToWeekday (const bcddate_t bcd_date) {
  return ((bcdDateToMjd(bcd_date) + 3) % 7);
}

// end of code
