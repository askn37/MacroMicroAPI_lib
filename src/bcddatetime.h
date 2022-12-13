/**
 * @file bcddatetime.h
 * @author askn (K.Sato) multix.jp
 * @brief BCD, MJD, Epoch date time convert
 * @version 0.1
 * @date 2022-09-21
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// type of time data
//
#ifndef time_t
typedef uint32_t time_t;    // UTC unix epoch 0 == 1970/1/1.0
#endif
typedef uint32_t date_t;    // Modified Julian Date (MJD) 0 == 1858/11/17.0
typedef uint32_t bcdtime_t;
typedef uint32_t bcddate_t;

struct bcddatetime_t {
  bcdtime_t time;
  bcddate_t date;
};

typedef union {
  uint8_t  bytes[2];
  uint16_t word;
} wbyte_t;
#define _WB(p) ((wbyte_t*)(&p))

typedef union {
  uint8_t  bytes[8];
  uint16_t words[4];
  uint32_t dwords[2];
  struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t wday;
    uint8_t day;
    uint8_t month;
    uint16_t year;
  } col;
  bcddatetime_t bcd;
} bcddt_t;
#define _BCDDT(p) ((bcddt_t*)&(p))

typedef union {
  uint8_t  bytes[4];
  uint16_t words[2];
  uint32_t dword;
  struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t wday;
  } col;
  bcdtime_t bcd;
} bcdt_t;
#define _BCDT(p) ((bcdt_t*)&(p))

typedef union {
  uint8_t  bytes[4];
  uint16_t words[2];
  uint32_t dword;
  struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
  } col;
  bcddate_t bcd;
} bcdd_t;
#define _BCDD(p) ((bcdd_t*)&(p))

//
// bcd hex convert
//
extern uint8_t dtob (const uint8_t);
extern uint8_t btod (const uint8_t);
extern uint16_t wdtob (const uint16_t);
extern uint16_t wbtod (const uint16_t);

//
// bcd date/time to
//
extern time_t bcdToEpoch (const bcddate_t, const bcdtime_t);
extern time_t bcdDateTimeToEpoch (const bcddatetime_t);

extern bcddatetime_t bcdDateTime24to12 (const bcddatetime_t);
extern bcddatetime_t bcdDateTime12to24 (const bcddatetime_t);
extern bcdtime_t bcdTime24to12 (const bcdtime_t);
extern bcdtime_t bcdTime12to24 (const bcdtime_t);

//
// bcd date to
//
extern date_t bcdDateToMjd (const bcddate_t);
extern uint8_t bcdDateToWeekday (const bcddate_t);

//
// mjd to
//
extern bcddate_t mjdToBcdDate (const date_t);
extern uint8_t mjdToWeekday (const date_t);
extern time_t mjdToEpoch (const date_t);

//
// epoch to
//
extern date_t epochToMjd (const time_t);
extern bcddatetime_t epochToBcdDateTime (const time_t);
extern bcddate_t epochToBcdDate (const time_t);
extern bcdtime_t epochToBcdTime (const time_t);
extern bcdtime_t epochToBcdDayTime (const time_t);
extern uint8_t epochToWeekday (const time_t);

//
// bcd time to
//
extern time_t bcdTimeToSeconds (const bcdtime_t);

#ifdef __cplusplus
}
#endif

// end of header