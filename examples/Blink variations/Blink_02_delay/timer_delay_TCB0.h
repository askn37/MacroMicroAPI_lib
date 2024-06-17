/**
 * @file timer_delay_TCB0.h
 * @author askn (K.Sato) multix.jp
 * @brief Blink using delay timer sketch code
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once

#if (F_CPU < 1000000L)
  #error Minimum supported F_CPU is 1000000L
  #include BUILD_STOP
#endif

#ifdef delay
  #undef delay
#endif

#define USE_TCB0 HAVE_TCB0

static volatile uint32_t _timer_millis;

ISR(TCB0_INT_vect) {
  TCB0_INTFLAGS = TCB_CAPT_bm;
  _timer_millis++;
}

namespace Timer {
  inline void init (void) {
    TCB0_CCMP = F_CPU / 1000 - 1;
    TCB0_INTCTRL = TCB_CAPT_bm;
    TCB0_CTRLB = TCB_CNTMODE_INT_gc;
    TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV1_gc;
  }

  inline uint32_t millis (void) {
    uint32_t _ms;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _ms = _timer_millis;
    }
    return _ms;
  }

  uint32_t micros (void) {
    uint32_t _ms;
    uint16_t _tc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _ms = _timer_millis;
      _tc = TCB0_CNT;
      if (bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp)) {
        _tc = TCB0_CNT;
        _ms++;
      }
    }

  /*
   * best practice frequency : 2 ^ n * 1000000
   */

  #if (F_CPU == 32000000L)
    /* top 0b_0111_1101_1100_0000 == 0x7dc0 == 32000    */
    /* div 0b.0000_1000_0000_0000 == 0x0800 == 65536/32 */
    return (_ms * 1000) + (_tc >> 5);

  #elif (F_CPU == 16000000L)
    /* top 0b_0011_1110_1000_0000 == 0x3e80 == 16000    */
    /* div 0b.0001_0000_0000_0000 == 0x1000 == 65536/16 */
    return (_ms * 1000) + (_tc >> 4);

  #elif (F_CPU == 8000000L)
    /* top 0b_0001_1111_0100_0000 == 0x1f40 == 8000     */
    /* div 0b.0010_0000_0000_0000 == 0x2000 == 65536/8  */
    return (_ms * 1000) + (_tc >> 3);

  #elif (F_CPU == 4000000L)
    /* top 0b_0000_1111_1010_0000 == 0x0fa0 == 4000     */
    /* div 0b.0100_0000_0000_0000 == 0x4000 == 65536/4  */
    return (_ms * 1000) + (_tc >> 2);

  #elif (F_CPU == 2000000L)
    /* top 0b_0000_0111_1101_0000 == 0x07d0 == 2000     */
    /* div 0b.1000_0000_0000_0000 == 0x8000 == 65536/2  */
    return (_ms * 1000) + (_tc >> 1);

  #elif (F_CPU == 1000000L)
    /* top 0b_0000_0011_1110_1000 == 0x03e8 == 1000     */
    /* div 0b.0000_0000_0000_0000 == 0x0000 == 65536/1  */
    return (_ms * 1000) + (_tc);

  /*
   * other frequencies : F_CPU > 1000000L
   */

  #else
    return (_ms * 1000) + (((uint32_t)_tc * (uint16_t)(65536000000UL / F_CPU)) >> 16);

  #endif
  }

  void delay (uint32_t _ms) {
    uint32_t start_us = micros();
    while (_ms > 0) {
      yield();
      while (_ms > 0 && (micros() - start_us) >= 1000) {
        _ms--;
        start_us += 1000;
      }
    }
  }
}

// end of code
