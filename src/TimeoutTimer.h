/**
 * @file TimeoutTimer.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <stdint.h>
#include <setjmp.h>

/*
 * Arduino API alias macro
 */
#if !defined(DISABLE_ALIAS_DELAY)
#define DISABLE_ALIAS_DELAY 1
#undef delay
#endif
#define delay(MS) \
  __extension__({TimeoutTimer::delay_ticks(timeout_millis_to_ticks(MS));})

#if !defined(DISABLE_ALIAS_MILLIS)
#define DISABLE_ALIAS_MILLIS 1
#undef millis
#endif
#define millis() \
  __extension__({TimeoutTimer::millis_left();})

extern "C" void yield (void);

struct TimeoutTimer_work_t {
  uint16_t counter;
  jmp_buf* context;
};

#if defined(HAVE_RTC) && !defined(USED_RTC)
#define USED_RTC HAVE_RTC
#endif

#ifndef __TIMEOUT_CLKFREQ
#define __TIMEOUT_CLKFREQ 1024
#endif

#if (__TIMEOUT_CLKFREQ == 1024)
/* DIV32 1024Hz */
#define __TIMEOUT_PRESCALER         RTC_PRESCALER_DIV32_gc
#define __TIMEOUT_MILLIS_MAX        63998
#define timeout_millis_to_ticks(MS) ((uint32_t)MS+(((uint32_t)MS*1573)>>16))
#define timeout_ticks_to_millis(TS) ((uint32_t)TS-(((uint32_t)TS*1536)>>16))
#define timeout_ticks_to_micros(TS) (((uint32_t)TS*15625)>>4)

#elif (__TIMEOUT_CLKFREQ == 2048)
/* DIV16 2048Hz */
#define __TIMEOUT_PRESCALER         RTC_PRESCALER_DIV16_gc
#define __TIMEOUT_MILLIS_MAX        31266
#define timeout_millis_to_ticks(MS) (((uint32_t)MS<<1)+(((uint32_t)MS*3146)>>16))
#define timeout_ticks_to_millis(TS) (((uint32_t)TS>>1)-(((uint32_t)TS*768)>>16))
#define timeout_ticks_to_micros(TS) (((uint32_t)TS*15625)>>5)

#elif (__TIMEOUT_CLKFREQ == 512)
/* DIV64 512Hz */
#define __TIMEOUT_PRESCALER         RTC_PRESCALER_DIV64_gc
#define __TIMEOUT_MILLIS_MAX        65535
#define timeout_millis_to_ticks(MS) (((uint32_t)MS>>1)+(((uint32_t)MS*787)>>16))
#define timeout_ticks_to_millis(TS) (((uint32_t)TS<<1)-(((uint32_t)TS*3072)>>16))
#define timeout_ticks_to_micros(TS) (((uint32_t)TS*15625)>>3)

#else
  #error Illegal __TIMEOUT_CLKFREQ spec
  #include BUILD_STOP
#endif

namespace TimeoutTimer {
  extern TimeoutTimer_work_t __worker;
  __attribute__((noreturn)) void abort (void);
  void __setup (uint16_t __ticks, jmp_buf *__return, volatile TimeoutTimer_work_t &__before);
  void __cleanup (volatile TimeoutTimer_work_t *__before);
  void begin (uint32_t _init_count = 0);
  uint32_t end (void);
  uint32_t ticks_left (void);
  uint16_t time_left (void);
  uint32_t millis_left (void);
  uint32_t micros_left (void);
  void delay_ticks (uint16_t _ticks);
  void sleep_cpu_ticks (uint32_t _ticks);
  bool interval_check_ticks (uint32_t &_ticks, uint16_t _interval = 0);
}

#define TIMEOUT_BLOCK(MS) \
  for(bool __ext=(__extension__({volatile TimeoutTimer_work_t __before;\
  jmp_buf __context;setjmp(__context)==0\
  ?(TimeoutTimer::__setup(timeout_millis_to_ticks(MS),&__context,__before),true)\
  :(TimeoutTimer::__cleanup(&__before),false);}));__ext;\
    (__extension__({if(__ext)TimeoutTimer::abort();})))

#define delay_timer(MS) \
  __extension__({TimeoutTimer::delay_ticks(timeout_millis_to_ticks(MS));})

#define sleep_cpu_timer(MS) \
  __extension__({TimeoutTimer::sleep_cpu_ticks(timeout_millis_to_ticks(MS));})

#define interval_check_timer(TC, MS) \
  __extension__({TimeoutTimer::interval_check_ticks(TC, timeout_millis_to_ticks(MS));})

// end of code
