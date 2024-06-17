/**
 * @file TimeoutTimer.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include "../TimeoutTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <limits.h>
#include <setjmp.h>
#include <api/capsule.h>

#if !defined(NOTUSED_INITIALIZE_TIMEOUTTIMER)
void __init_timer (void)
  __attribute__ ((naked,weak,used,section (".init8")));
void __init_timer (void) { TimeoutTimer::begin(); }
#endif

namespace TimeoutTimer {
  TimeoutTimer_work_t __worker = {0, nullptr};

  void __setup (uint16_t __ticks, jmp_buf *__return, volatile TimeoutTimer_work_t &__before) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      if (RTC_INTCTRL & RTC_CMP_bm) {
        __before.counter = RTC_CMP - RTC_CNT + 1;
        __before.context = __worker.context;
      }
      __worker.context = __return;
      while (RTC_STATUS != 0);
      RTC_CMP = __ticks + RTC_CNT;
      RTC_INTCTRL = RTC_OVF_bm | RTC_CMP_bm;
      RTC_CTRLA = RTC_RTCEN_bm | __TIMEOUT_PRESCALER;
    }
  }

  void __cleanup (volatile TimeoutTimer_work_t *__before) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      while (RTC_STATUS != 0);
      if (__before->counter) {
        __worker.context = __before->context;
        RTC_CMP = __before->counter + RTC_CNT;
      }
      else {
        RTC_INTCTRL = RTC_OVF_bm;
      }
    }
    __asm__ __volatile__ ("CALL L_RETI");
  }

  void begin (uint32_t _init_count) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      while (RTC_STATUS != 0);
      RTC_PER = UINT16_MAX;
      __worker.counter = _CAPS32(_init_count)->words[1];
      RTC_CNT = _CAPS32(_init_count)->words[0];
      RTC_INTCTRL = RTC_OVF_bm;
      RTC_CTRLA = RTC_RTCEN_bm | __TIMEOUT_PRESCALER;
    }
  }

  void abort (void) {
    jmp_buf* _restore = TimeoutTimer::__worker.context;
    longjmp(*_restore, 1);
  }

  uint32_t end (void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      while (RTC_STATUS != 0);
      RTC_CTRLA = 0;
      RTC_INTCTRL = 0;
      __worker.context = nullptr;
    }
    return ticks_left();
  }

  uint32_t ticks_left (void) {
    uint32_t _value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _CAPS32(_value)->words[1] = __worker.counter;
      _CAPS32(_value)->words[0] = RTC_CNT;
      if (bit_is_set(RTC_INTFLAGS, RTC_OVF_bp)) {
        _CAPS32(_value)->words[0] = RTC_CNT;
        _CAPS32(_value)->words[1]++;
      }
    }
    return _value;
  }

  uint16_t time_left (void) {
    return RTC_CMP - RTC_CNT;
  }

  uint32_t millis_left (void) {
    uint32_t _ticks = ticks_left();
    return timeout_ticks_to_millis(_ticks);
  }

  uint32_t micros_left (void) {
    uint32_t _ticks = ticks_left();
    return timeout_ticks_to_micros(_ticks);
  }

  void delay_ticks (uint16_t _ticks) {
    uint32_t _start = ticks_left();
    while (_ticks > 0) {
      yield();
      uint32_t _step = ticks_left();
      while (_ticks > 0 && (_step - _start) > 0) {
        _ticks--;
        _start++;
      }
    }
  }

  void sleep_cpu_ticks (uint32_t _ticks) {
    uint32_t _save_ticks = end();
    uint8_t _f = __TIMEOUT_PRESCALER;
    while (_CAPS32(_ticks)->words[1] && _f < RTC_PRESCALER_DIV32768_gc) {
      _ticks >>= 1;
      _f += RTC_PRESCALER_DIV2_gc;
    }
    RTC_CMP = _CAPS32(_ticks)->words[0];
    RTC_INTCTRL = RTC_CMP_bm;
    while (RTC_STATUS != 0);
    RTC_CNT = 0;
    RTC_CTRLA = RTC_RUNSTDBY_bm | RTC_RTCEN_bm | _f;
    sei();
    sleep_cpu();
    begin(_save_ticks);
  }

    bool interval_check_ticks (uint32_t &_ticks, uint16_t _interval) {
    if ((int32_t)(_ticks - ticks_left()) <= 0) {
      _ticks += _interval;
      return true;
    }
    return false;
  }
}

/* interrupt base code */
/*
ISR(RTC_CNT_vect) {
  uint8_t _f = RTC_INTFLAGS;
  RTC_INTFLAGS = _f;
  if (_f & RTC_OVF_bm)
    TimeoutTimer::__worker.counter++;
  if ((_f & RTC_CMP_bm) && TimeoutTimer::__worker.context != 0) {
    __asm__ __volatile__ ("CALL L_RETI");
    TimeoutTimer::abort();
  }
}
__asm__ __volatile__ ("L_RETI: RETI");
*/

ISR(RTC_CNT_vect, ISR_NAKED) {
  __asm__ __volatile__ ( R"#ASM#(
        PUSH    R0              ; enter interrupt
        IN      R0, __SREG__    ; keep SREG
        PUSH    R0              ;
        PUSH    R24             ; keep works
        PUSH    R25             ;
        LDS     R0, %0          ; R0 := RTC_INTFLAGS
        STS     %0, R0          ; RTC_INTFLAGS := R0
        SBRS    R0, %3          ; check bit RTC_OVF_bp
        RJMP    1f              ; bit is clear skip
        LDS     R24, %1         ; R25:R24 := counter
        LDS     R25, %1+1       ;
        ADIW    R24, 1          ; R25:R24 += 1
        STS     %1,   R24       ; modify counter
        STS     %1+1, R25       ;
1:      SBRS    R0, %4          ; check bit RTC_CMP_bp
        RJMP    2f              ; bit is clear skip
        LDS     R24, %2         ; R25:R24 := context
        LDS     R25, %2+1       ; (set 'longjmp' 1st parameter)
        SBIW    R24, 0          ; R25:R24 -= 0
        BRNE    3f              ; branch not zero
2:      POP     R25             ; restore works
        POP     R24             ;
        POP     R0              ;
        OUT     __SREG__, R0    ; restore SREG
        POP     R0              ;
L_RETI: RETI                    ; leave interrupt
3:      LDI     R22, 1          ; set 'longjmp' 2nd parameter
        LDI     R23, 0          ;
        JMP     longjmp         ; 'reti' is jump to 'longjmp'
    )#ASM#"
    ::"p" (_SFR_MEM_ADDR(RTC_INTFLAGS))
    , "p" (_SFR_MEM_ADDR(TimeoutTimer::__worker.counter))
    , "p" (_SFR_MEM_ADDR(TimeoutTimer::__worker.context))
    , "n" (RTC_OVF_bp)
    , "n" (RTC_CMP_bp)
  );
}

// end of code
