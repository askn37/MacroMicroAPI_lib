/**
 * @file XRTC_PCF85063A.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-17
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../XRTC_PCF85063A.h"

bool XRTC_PCF85063A::_write_register (uint8_t _start, uint8_t _index, uint8_t _length) {
    TWIC.stop();
    return 1 + _length == (uint8_t)TWIC
        .start(XRTC_PCF85063A_ADDR, _length + 1)
        .send(_start)
        .send((uint8_t*)((uint16_t)&_xrtc_register + _index), _length)
        .write_size();
}

bool XRTC_PCF85063A::_read_register (uint8_t _start, uint8_t _index, uint8_t _length) {
    TWIC.stop();
    return _length == TWIC
        .start(XRTC_PCF85063A_ADDR)
        .send(_start)
        .request(XRTC_PCF85063A_ADDR, _length)
        .read((uint8_t*)((uint16_t)&_xrtc_register + _index), _length);
}


bool XRTC_PCF85063A::update (void) {
    return _read_register(REG_OFF_ALARM, REG_IDX_ALARM, sizeof(_xrtc_register));
}

bool XRTC_PCF85063A::saveSettings (void) {
    _xrtc_register.SR = false;
    return _write_register(REG_OFF_CONTROL1, REG_IDX_CONTROL1, REG_SIZ_CONTROLS);
}

bool XRTC_PCF85063A::reset (void) {
    return 2 == TWIC
        .start(XRTC_PCF85063A_ADDR, 2)
        .send(REG_OFF_CONTROL1)
        .send(0x58) /* Software Reset Value */
        .write_size();
}

/*
* Realtime Clock Selection
*/
bcddatetime_t XRTC_PCF85063A::getBcdDateTimeNow (void) {
    bcddatetime_t t_bcd;
    _BCDDT(t_bcd)->bytes[0] = _xrtc_register.SECONDS ;
    _BCDDT(t_bcd)->bytes[1] = _xrtc_register.MINUTES ;
    _BCDDT(t_bcd)->bytes[2] = _xrtc_register.HOURS   ;
    _BCDDT(t_bcd)->bytes[3] = 0;
    _BCDDT(t_bcd)->bytes[4] = _xrtc_register.DAYS    ;
    _BCDDT(t_bcd)->bytes[5] = _xrtc_register.MONTHS  ;
    _BCDDT(t_bcd)->bytes[6] = _xrtc_register.YEARS   ;
    _BCDDT(t_bcd)->bytes[7] = 0x20;
    return t_bcd;
}

time_t XRTC_PCF85063A::getEpochNow (void) {
    bcddatetime_t t_bcd = getBcdDateTimeNow();
    if (is12hourMode()) t_bcd = bcdDateTime12to24(t_bcd);
    return bcdDateTimeToEpoch(t_bcd);
}

time_t XRTC_PCF85063A::getCenturyEpochNow (void) {
    bcddatetime_t t_bcd = getCenturyDateTimeNow();
    if (is12hourMode()) t_bcd = bcdDateTime12to24(t_bcd);
    return bcdDateTimeToEpoch(t_bcd);
}

bool XRTC_PCF85063A::adjustBcdDateTime (const bcddatetime_t t_bcd) {
    _xrtc_register.OS = 0;
    _xrtc_register.SECONDS  = _BCDDT(t_bcd)->bytes[0]      ;
    _xrtc_register.MINUTES  = _BCDDT(t_bcd)->bytes[1]      ;
    _xrtc_register.HOURS    = _BCDDT(t_bcd)->bytes[2]      ;
    _xrtc_register.DAYS     = _BCDDT(t_bcd)->bytes[4]      ;
    _xrtc_register.WEEKDAYS = bcdDateToWeekday(t_bcd.date) ;
    _xrtc_register.MONTHS   = _BCDDT(t_bcd)->bytes[5]      ;
    _xrtc_register.YEARS    = _BCDDT(t_bcd)->bytes[6]      ;
    return _write_register(REG_OFF_SECONDS, REG_IDX_SECONDS, REG_SIZ_SECONDS + REG_SIZ_DAYS);
}

bool XRTC_PCF85063A::adjustEpoch (const time_t t_time) {
    bcddatetime_t t_bcd = epochToBcdDateTime(t_time);
    if (is12hourMode()) t_bcd = bcdDateTime24to12(t_bcd);
    return adjustBcdDateTime(t_bcd);
}

bool XRTC_PCF85063A::adjustBcdDate (const bcddate_t t_bcd_date) {
    _xrtc_register.DAYS     = _BCDD(t_bcd_date)->bytes[0]  ;
    _xrtc_register.WEEKDAYS = bcdDateToWeekday(t_bcd_date) ;
    _xrtc_register.MONTHS   = _BCDD(t_bcd_date)->bytes[1]  ;
    _xrtc_register.YEARS    = _BCDD(t_bcd_date)->bytes[2]  ;
    return _write_register(REG_OFF_DAYS, REG_IDX_DAYS, REG_SIZ_DAYS);
}

bool XRTC_PCF85063A::adjustBcdTime (const bcdtime_t t_bcd_time) {
    _xrtc_register.OS = 0;
    _xrtc_register.SECONDS = _BCDT(t_bcd_time)->bytes[0] ;
    _xrtc_register.MINUTES = _BCDT(t_bcd_time)->bytes[1] ;
    _xrtc_register.HOURS   = _BCDT(t_bcd_time)->bytes[2] ;
    return _write_register(REG_OFF_SECONDS, REG_IDX_SECONDS, REG_SIZ_SECONDS);
}

/* Extend 400 years */
/* 1900-01-01 mjd:15020 to 2299-12-31 mjd:161116 */
bcddatetime_t XRTC_PCF85063A::getCenturyDateTimeNow (void) {
    bcddatetime_t t_bcd = getBcdDateTimeNow();
    if (is12hourMode()) t_bcd = bcdDateTime12to24(t_bcd);
    date_t t_mjd = bcdDateToMjd(t_bcd.date);
    int8_t week_shift = (mjdToWeekday(t_mjd) + 7 - _xrtc_register.WEEKDAYS) % 7;
    if (week_shift != 0) { /* other year 2000-2099 mjd:51544-88068 */
        bool fix = false;
        if (t_mjd <= 51603) { /* under 'year xx00 day 02/29' */
            if (t_mjd == 51603 && (uint16_t)t_bcd.date == 0x0229) fix = true;
            if      (week_shift == 5) t_mjd -= 36524;  /* 1900 */
            else if (week_shift == 1) t_mjd += 36525;  /* 2100 */
            else if (week_shift == 3) t_mjd += 73049;  /* 2200 */
        }
        else {
            if      (week_shift == 6) t_mjd -= 36525;  /* 1900 */
            else if (week_shift == 2) t_mjd += 36524;  /* 2100 */
            else if (week_shift == 4) t_mjd += 73048;  /* 2200 */
        }
        t_bcd.date = mjdToBcdDate(t_mjd);
        if (fix) adjustBcdDate(t_bcd.date); /* fix 'year xx00 day 02/29' leap day */
    }
    return t_bcd;
}

/*
    * Alarm Timer Selection
    */
bool XRTC_PCF85063A::activeAlarm (const bool t_enable) {
    _xrtc_register.AF  = false;
    _xrtc_register.AIE = t_enable;
    return _write_register(REG_OFF_ALARM, REG_IDX_ALARM, REG_SIZ_ALARM)
            && _write_register(REG_OFF_CONTROL2, REG_IDX_CONTROL2, REG_SIZ_CONTROL2);
}

bool XRTC_PCF85063A::clearAlarmFlag (void) {
    _xrtc_register.AF = false;
    return _write_register(REG_OFF_CONTROL2, REG_IDX_CONTROL2, REG_SIZ_CONTROL2);
}

XRTC_PCF85063A& XRTC_PCF85063A::setAlarmSettings (const bcdtime_t t_bcd, XRTC_ALARM_SETTINGS t_enable_flags) {
    _xrtc_register.SECOND_ALARM  = _BCDT(t_bcd)->col.second ;
    _xrtc_register.MINUTE_ALARM  = _BCDT(t_bcd)->col.minute ;
    _xrtc_register.HOUR_ALARM    = _BCDT(t_bcd)->col.hour   ;
    _xrtc_register.DAY_ALARM     = _BCDT(t_bcd)->col.wday   ;
    _xrtc_register.WEEKDAY_ALARM =  t_enable_flags.WEEKDAYS  ;
    _xrtc_register.AEN_S         = !t_enable_flags.SECOND    ;
    _xrtc_register.AEN_M         = !t_enable_flags.MINUTE    ;
    _xrtc_register.AEN_H         = !t_enable_flags.HOUR      ;
    _xrtc_register.AEN_D         = !t_enable_flags.DAY       ;
    _xrtc_register.AEN_W         = !t_enable_flags.WEEKDAY   ;
    return *this;
}

bcdtime_t XRTC_PCF85063A::getAlarmTime (void) {
    bcdtime_t t_bcd;
    _BCDT(t_bcd)->col.second = _xrtc_register.SECOND_ALARM ;
    _BCDT(t_bcd)->col.minute = _xrtc_register.MINUTE_ALARM ;
    _BCDT(t_bcd)->col.hour   = _xrtc_register.HOUR_ALARM   ;
    _BCDT(t_bcd)->col.wday   = _xrtc_register.DAY_ALARM    ;
    return t_bcd;
}

XRTC_ALARM_SETTINGS XRTC_PCF85063A::getAlarmSettings (void) {
    XRTC_ALARM_SETTINGS t_enable_flags;
    t_enable_flags.WEEKDAYS =  _xrtc_register.WEEKDAY_ALARM ;
    t_enable_flags.SECOND   = !_xrtc_register.AEN_S         ;
    t_enable_flags.MINUTE   = !_xrtc_register.AEN_M         ;
    t_enable_flags.HOUR     = !_xrtc_register.AEN_H         ;
    t_enable_flags.DAY      = !_xrtc_register.AEN_D         ;
    t_enable_flags.WEEKDAY  = !_xrtc_register.AEN_W         ;
    return t_enable_flags;
}

/*
    * Period/Countdown Timer Selection
    */
bool XRTC_PCF85063A::activeTimer (const bool t_enable, const bool t_interrupt) {
    if (t_enable)    _xrtc_register.TE  = true;
    else           { _xrtc_register.TE  = false; _xrtc_register.T = 0; }
    if (t_interrupt) _xrtc_register.TIE = true;
    else             _xrtc_register.TIE = false;
    _xrtc_register.TF = false;
    return _write_register (REG_OFF_TIMER, REG_IDX_TIMER, REG_SIZ_TIMER + REG_SIZ_CONTROL);
}

bool XRTC_PCF85063A::clearTimerFlag (void) {
    _xrtc_register.TF = false;
    return _write_register (REG_OFF_CONTROL2, REG_IDX_CONTROL2, REG_SIZ_CONTROL2);
}

bool XRTC_PCF85063A::startCountdownTimer (const uint16_t t_seconds, const bool t_enable_pluse) {
    uint16_t t_period_count = t_seconds;
    uint8_t t_timer_mode = 0b10;
    if (t_period_count > 15300) t_period_count = 15300;
    if (t_period_count > 255) {
        t_period_count += 59;
        t_period_count /= 60;
        t_timer_mode = 0b11;
    }
    _xrtc_register.TI_TP = t_enable_pluse;
    _xrtc_register.TCF   = t_timer_mode;
    _xrtc_register.T     = (uint8_t)t_period_count;
    bool _t = t_period_count != 0;
    return activeTimer(_t, _t);
}

XRTC_TIMER_SETTINGS XRTC_PCF85063A::getTimerValues (void) {
    XRTC_TIMER_SETTINGS t_settings;
    t_settings.Value           = _xrtc_register.T     ;
    t_settings.InterruptMode   = _xrtc_register.TI_TP ;
    t_settings.InterruptEnable = _xrtc_register.TIE   ;
    t_settings.Enable          = _xrtc_register.TE    ;
    t_settings.ClockFrequency  = _xrtc_register.TCF   ;
    return t_settings;
}

bool XRTC_PCF85063A::activateTimer (const XRTC_TIMER_SETTINGS t_settings) {
    _xrtc_register.T     = t_settings.Value           ;
    _xrtc_register.TI_TP = t_settings.InterruptMode   ;
    _xrtc_register.TIE   = t_settings.InterruptEnable ;
    _xrtc_register.TE    = t_settings.Enable          ;
    _xrtc_register.TCF   = t_settings.ClockFrequency  ;
    _xrtc_register.TF    = false;
    return _write_register (REG_OFF_TIMER, REG_IDX_TIMER, REG_SIZ_TIMER + REG_SIZ_CONTROL);
}

/*
    * Clock Out Frequency
    */

bool XRTC_PCF85063A::changeClockOutFrequency (const uint8_t t_clockfreq) {
    _xrtc_register.COF = t_clockfreq;
    return _write_register (REG_OFF_CONTROL2, REG_IDX_CONTROL2, REG_SIZ_CONTROL2);
}

// end of code
