/**
 * @file timer_delay.h
 * @author askn (K.Sato) multix.jp
 * @brief Blink using delay timer sketch code
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

/* select use timer peripheral */
#if !defined(USE_TIMER_DELAY)
  #define USE_TIMER_DELAY HAVE_TCB0
#endif

#if defined(USE_TIMER_DELAY)
  #if defined(HAVE_TCB4) && (USE_TIMER_DELAY == HAVE_TCB4)
    #include "timer_delay_TCB4.h"
  #elif defined(HAVE_TCB3) && (USE_TIMER_DELAY == HAVE_TCB3)
    #include "timer_delay_TCB3.h"
  #elif defined(HAVE_TCB2) && (USE_TIMER_DELAY == HAVE_TCB2)
    #include "timer_delay_TCB2.h"
  #elif defined(HAVE_TCB1) && (USE_TIMER_DELAY == HAVE_TCB1)
    #include "timer_delay_TCB1.h"
  #elif defined(HAVE_TCB0) && (USE_TIMER_DELAY == HAVE_TCB0)
    #include "timer_delay_TCB0.h"
  #endif
#endif

#if defined(delay)
  #error Not selected USE_TIMER_DELAY
  #include BUILD_STOP
#endif

// end of code
