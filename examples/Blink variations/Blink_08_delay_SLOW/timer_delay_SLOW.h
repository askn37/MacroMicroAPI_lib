/**
 * @file timer_delay_SLOW.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#if (F_CPU < 4096L)
  #error "Minimum supported F_CPU is 4096L"
  #include "BUILD_STOP"
#endif

#ifdef delay
  #undef delay
#endif

static volatile uint32_t _timer_seconds;

ISR(RTC_CNT_vect) {
  RTC_INTFLAGS = RTC_OVF_bm;
  _timer_seconds++;
}

namespace Timer {
  inline void init (void) {
    loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  #if (F_CPU < 131072L)
    RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;
  #endif
    RTC_PER = UINT16_MAX;
    RTC_INTCTRL = RTC_OVF_bm;
    RTC_CTRLA = RTC_RTCEN_bm;
  }

  uint32_t millis (void) {
    uint32_t _sc, _tc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _sc = _timer_seconds;
      _tc = RTC_CNT;
      if (bit_is_set(RTC_INTFLAGS, RTC_OVF_bp)) {
        _tc = RTC_CNT;
        _sc++;
      }
    }

  #if (F_CPU < 131072L)
    _tc = (_tc * 64000) >> 16;
    return _sc * 64000 + _tc;
  #else
    _tc = (_tc * 2000) >> 16;
    return _sc * 2000 + _tc;
  #endif

  }

  uint32_t micros (void) {
    uint32_t _sc, _tc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _sc = _timer_seconds;
      _tc = RTC_CNT;
      if (bit_is_set(RTC_INTFLAGS, RTC_OVF_bp)) {
        _tc = RTC_CNT;
        _sc++;
      }
    }

  #if (F_CPU < 131072L)
    _tc = _tc * 976 + (_tc >> 1);
    return _sc * 64000000 + _tc;
  #else
    _tc = _tc * 30 + (_tc >> 1);
    return _sc * 2000000 + _tc;
  #endif

  }

  void delay (uint32_t _ms) {
    uint32_t start_us = micros(), step_us;
    while (_ms > 0) {
      yield();
      while (_ms > 0 && (step_us = micros() - start_us) >= 1000) {
        do {
          _ms--;
          start_us += 1000;
          step_us -= 1000;
        } while (_ms > 0 && step_us >= 1000);
      }
    }
  }
}

// end of code
