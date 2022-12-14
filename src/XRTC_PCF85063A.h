/**
 * @file XRTC_PCF85063A.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-17
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

// #include <inttypes.h>
// #include <util/atomic.h>
#include <api/TWIM.h>
#include "bcddatetime.h"

// I2C values
#define XRTC_PCF85063A_ADDR 0x51 // R:A3 W:A2

// RTC_REG_Offset Correction offset
// MSB MODE=0 (1/32s   x +-Offset / 2 hour  :ppm) (0.375     sec:day) (11.4375     sec:month)
// MSB MODE=1 (1/1024s x +-Offset / 4 minute:ppm) (0.3515625 sec:day) (10.72265625 sec:month)
// #define XRTC_PCF85063A_COLLECTON_VALUE 0

struct XRTC_ALARM_SETTINGS {
  uint8_t WEEKDAYS : 3;
      bool SECOND  : 1;
      bool MINUTE  : 1;
      bool HOUR    : 1;
      bool DAY     : 1;
      bool WEEKDAY : 1;
};

struct XRTC_TIMER_SETTINGS {
  uint8_t Value;
     bool InterruptMode   : 1;
     bool InterruptEnable : 1;
     bool Enable          : 1;
  uint8_t ClockFrequency  : 2;
};

class XRTC_PCF85063A {
private:
  TWIM_Class TWIC;

  typedef enum XRTC_PCF85063A_enum : uint8_t {
    REG_OFF_CONTROL1 = 0x00
  , REG_OFF_CONTROL2 = 0x01
  ,	REG_OFF_RAMBYTE  = 0x03
  ,	REG_OFF_SECONDS  = 0x04
  ,	REG_OFF_DAYS     = 0x07
  , REG_OFF_ALARM    = 0x0B
  , REG_OFF_TIMER    = 0x10
  , REG_IDX_CONTROL1 = 0x00 + 0x07
  , REG_IDX_CONTROL2 = 0x01 + 0x07
  , REG_IDX_OFFSET   = 0x02 + 0x07
  ,	REG_IDX_RAMBYTE  = 0x03 + 0x07
  ,	REG_IDX_SECONDS  = 0x04 + 0x07
  ,	REG_IDX_DAYS     = 0x07 + 0x07
  , REG_IDX_ALARM    = 0x0B - 0x0B
  , REG_IDX_TIMER    = 0x10 - 0x0B
  , REG_SIZ_CONTROL1 = 0x01
  , REG_SIZ_CONTROL2 = 0x01
  , REG_SIZ_CONTROL  = 0x02
  , REG_SIZ_CONTROLS = 0x04
  , REG_SIZ_OFFSET   = 0x01
  ,	REG_SIZ_RAMBYTE  = 0x01
  ,	REG_SIZ_SECONDS  = 0x03
  ,	REG_SIZ_DAYS     = 0x04
  , REG_SIZ_ALARM    = 0x05
  , REG_SIZ_TIMER    = 0x02
  } XRTC_PCF85063A_t;

  struct {
    /* $0B-0F Alarm registers */

    /* 00: $0B Second_alarm */
    uint8_t SECOND_ALARM  : 7;
       bool AEN_S         : 1;

    /* 01: $0C Minute_alarm */
    uint8_t MINUTE_ALARM  : 7;
       bool AEN_M         : 1;

    /* 02: $0D Hour_alarm */
    uint8_t HOUR_ALARM    : 6;
       bool reserve0      : 1;
       bool AEN_H         : 1;

    /* 03: $0E Day_alarm */
    uint8_t DAY_ALARM     : 6;
       bool reserve1      : 1;
       bool AEN_D         : 1;

    /* 04: $0F Weekday_alarm */
    uint8_t WEEKDAY_ALARM : 3;
    uint8_t reserve2      : 4;
       bool AEN_W         : 1;

    /* $10-$11 Timer registers */

    /* 05: $10 Timer_value */
    uint8_t T             : 8;

    /* 06: $11 Timer_mode */
       bool TI_TP         : 1;
       bool TIE           : 1;
       bool TE            : 1;
    uint8_t TCF           : 2;
    uint8_t reserve3      : 3;

    /* $00-$03 Control and status registers */

    /* 07: $00 Control_1 */
       bool CAP_SEL       : 1;
       bool _12_24        : 1;
       bool CIE           : 1;
       bool reserve4      : 1;
       bool SR            : 1;
       bool STOP          : 1;
       bool reserve5      : 1;
       bool EXT_TEST      : 1;

    /* 08: $01 Control_2 */
    uint8_t COF           : 3;
       bool TF            : 1;
       bool HMI           : 1;
       bool MI            : 1;
       bool AF            : 1;
       bool AIE           : 1;

    /* 09: $02 Offset */
     int8_t OFFSET        : 7;
       bool MODE          : 1;

    /* 0A: $03 RAM_byte */
    uint8_t B             : 8;

    /* $04-$0A Time and date registers */

    /* 0B: $04 Seconds */
    uint8_t SECONDS       : 7;
       bool OS            : 1;

    /* 0C: $05 Minutes */
    uint8_t MINUTES       : 7;
       bool reserve6      : 1;

    /* 0D: $06 Hours */
    uint8_t HOURS         : 6;
    uint8_t reserve7      : 2;

    /* 0E: $07 Days */
    uint8_t DAYS          : 6;
    uint8_t reserve8      : 2;

    /* 0F: $08 Weekdays */
    uint8_t WEEKDAYS      : 3;
    uint8_t reserve9      : 5;

    /* 10: $09 Months */
    uint8_t MONTHS        : 5;
    uint8_t reserve10     : 3;

    /* 11: $0A Years */
    uint8_t YEARS         : 8;

  } _xrtc_register;

  bool _write_register (uint8_t _start, uint8_t _index, uint8_t _length);
  bool _read_register (uint8_t _start, uint8_t _index, uint8_t _length);

public:
  inline XRTC_PCF85063A (TWIM_Class _twim) : TWIC(_twim) {}

  bool update (void);
  inline bool load (void) { return update(); }
  bool saveSettings (void);
  bool reset (void);
  inline bool start (void) { setRunning(true); return saveSettings(); }
  inline bool stop (void) { setRunning(false); return saveSettings(); }

  // void dump (void) { Serial.printDump((uint8_t*)&_xrtc_register, sizeof(_xrtc_register)); }

  /*
   * getter
   */
  inline bool isPowerDown (void) { return _xrtc_register.OS; }
  inline bool isRunning (void) {return !isStopped(); }
  inline bool isStopped (void) {return _xrtc_register.STOP; }
  inline bool is12hourMode (void) { return _xrtc_register._12_24; }

  inline bool isAlarm (void) { return _xrtc_register.AF; }
  inline bool isAlarmInterruptEnable (void) { return _xrtc_register.AIE; }
  inline bool isTimer (void) { return _xrtc_register.TF; }
  inline bool isTimerEnable (void) { return _xrtc_register.TE; }
  inline bool isTimerInterruptEnable (void) { return _xrtc_register.TIE; }
  inline bool isTimerPeriod (void) { return _xrtc_register.TI_TP; }

  inline bool isMinuteInterruptEnable (void) { return _xrtc_register.MI; }
  inline bool isHarfMinuteInterruptEnable (void) { return _xrtc_register.HMI; }

  inline bool isExternalTestMode (void) { return _xrtc_register.EXT_TEST; }
  inline bool isCapacitorSelection (void) { return _xrtc_register.CAP_SEL; }
  inline bool isCorrectionOffsetMode (void) { return _xrtc_register.MODE; }
  inline bool isCorrectionInterruptEnable (void) { return _xrtc_register.CIE; }

  inline uint8_t getClockOutFrequency (void) { return _xrtc_register.COF; }
  inline uint8_t getWeekdays (void) { return _xrtc_register.WEEKDAYS; }
  inline uint8_t getCorrectionOffset (void) { return _xrtc_register.OFFSET; }
  inline uint8_t getRamByte (void) { return _xrtc_register.B; }
  inline uint8_t getControl1 (void) { return ((uint8_t*)&_xrtc_register)[REG_IDX_CONTROL1]; }
  inline uint8_t getControl2 (void) { return ((uint8_t*)&_xrtc_register)[REG_IDX_CONTROL2]; }

  /*
   * setter
   */
  inline XRTC_PCF85063A& setRunning (const bool t_enable) { _xrtc_register.STOP = !t_enable; return *this; }
  inline XRTC_PCF85063A& set12hourMode (const bool t_enable) { _xrtc_register._12_24 = t_enable; return *this; }
  inline XRTC_PCF85063A& setMinuteInterruptEnable (const bool t_enable) { _xrtc_register.MI = t_enable; return *this; }
  inline XRTC_PCF85063A& setHarfMinuteInterruptEnable (const bool t_enable) { _xrtc_register.HMI = t_enable; return *this; }

  inline XRTC_PCF85063A& setTimerEnable (const bool t_enable) { _xrtc_register.TE = t_enable; return *this; }
  inline XRTC_PCF85063A& setTimerInterruptEnable (const bool t_enable) { _xrtc_register.TIE = t_enable; return *this; }

  inline XRTC_PCF85063A& setExternalTestMode (const bool t_enable) { _xrtc_register.EXT_TEST = t_enable; return *this; }
  inline XRTC_PCF85063A& setCapacitorSelection (const bool t_enable) { _xrtc_register.CAP_SEL = t_enable; return *this; }
  inline XRTC_PCF85063A& setCorrectionOffsetMode (const bool t_enable) { _xrtc_register.MODE = t_enable; return *this; }
  inline XRTC_PCF85063A& setCorrectionInterruptEnable (const bool t_enable) { _xrtc_register.CIE = t_enable; return *this; }

  inline XRTC_PCF85063A& setCorrectionOffset (const uint8_t t_offset) { _xrtc_register.OFFSET = t_offset; return *this; }
  inline XRTC_PCF85063A& setRamByte (const uint8_t t_ram_byte) { _xrtc_register.B = t_ram_byte; return *this; }

  /*
  * Realtime Clock Selection
  */
  inline time_t now (void) { return getEpochNow(); }
  inline bool adjustMjd (const date_t t_mjd) { return adjustBcdDate(mjdToBcdDate(t_mjd)); }
  inline bool adjust (const bcddatetime_t t_bcd) { return adjustBcdDateTime(t_bcd); }
  inline bool adjust (const time_t t_time) { return adjustEpoch(t_time); }

  bcddatetime_t getBcdDateTimeNow (void);
  time_t getEpochNow (void);
  time_t getCenturyEpochNow (void);
  bool adjustBcdDateTime (const bcddatetime_t t_bcd);
  bool adjustEpoch (const time_t t_time);
  bool adjustBcdDate (const bcddate_t t_bcd_date);
  bool adjustBcdTime (const bcdtime_t t_bcd_time);

  /* Extend 400 years */
  /* 1900-01-01 mjd:15020 to 2299-12-31 mjd:161116 */
  bcddatetime_t getCenturyDateTimeNow (void);

  /*
   * Alarm Timer Selection
   */
  bool activeAlarm (const bool t_enable);
  bool clearAlarmFlag (void);
  XRTC_PCF85063A& setAlarmSettings (const bcdtime_t t_bcd, XRTC_ALARM_SETTINGS t_enable_flags);
  bcdtime_t getAlarmTime (void);
  XRTC_ALARM_SETTINGS getAlarmSettings (void);

  /*
   * Period/Countdown Timer Selection
   */
  inline bool deactiveTimer (void) { return activeTimer(false, false); }
  inline bool activeTimer (const bool t_enable = true) { return activeTimer(t_enable, t_enable); }
  bool activeTimer (const bool t_enable, const bool t_interrupt);
  bool clearTimerFlag (void);
  inline bool startPeriodTimer (const uint16_t t_seconds) { return startCountdownTimer(t_seconds, true); }
  bool startCountdownTimer (const uint16_t t_seconds, const bool t_enable_pluse = true);
  XRTC_TIMER_SETTINGS getTimerValues (void);
  bool activateTimer (const XRTC_TIMER_SETTINGS t_settings);

  /*
   * Clock Out Frequency
   */
  bool changeClockOutFrequency (const uint8_t t_clockfreq);
};

// end of code
