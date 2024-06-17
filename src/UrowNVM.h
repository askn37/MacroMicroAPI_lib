/**
 * @file UrowNVM.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once
#include <avr/io.h>
#include <variant.h>

#define __ASSERT_USE_STDERR
#include <assert.h>

#if !defined(AVR_NVMCTRL) || \
    !defined(__AVR_XMEGA__) || \
           ((__AVR_ARCH__ != 102) && \
            (__AVR_ARCH__ != 103) && \
            (__AVR_ARCH__ != 104))
  #error CPU not supported by this target
  #include BUILD_STOP
#endif

namespace UrowNVM {
  bool userrow_clear (void);
  void userrow_load (void* _data_addr, size_t _save_size = USER_SIGNATURES_SIZE);
  bool userrow_save (const void* _data_addr, size_t _save_size = USER_SIGNATURES_SIZE);
  bool userrow_verify (const void* _data_addr, size_t _save_size = USER_SIGNATURES_SIZE);
}

// end of code
