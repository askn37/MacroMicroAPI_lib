/**
 * @file FlashNVM.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023 askn at github.com
 *
 */
#pragma once
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <api/memspace.h>
#include <stddef.h>

#if defined(RAMPZ)
  /* 128KiB model */
  typedef int32_t nvmptr_t;
  #define HAVE_RAMPZ
#else
  /* under 64KiB model */
  typedef int16_t nvmptr_t;
  #define NOTUSE_RAMPZ
#endif

#if (LOCKBITS_DEFAULT == 0x5CC5C55C)
  #if defined(NVMCTRL_FLBUSY_bm)
    /* AVR EA Series */
    #define NVMCTRL_VER 3
  #else
    /* AVR DA/DB/DD Series */
    #define NVMCTRL_VER 2
  #endif
#elif (LOCKBITS_DEFAULT == 0xC5)
  /* megaAVR/tinyAVR Series */
  #define NVMCTRL_VER 0
#endif

#if !defined(NVMCTRL_VER) || \
    !defined(__AVR_XMEGA__) || \
           ((__AVR_ARCH__ != 102) && \
            (__AVR_ARCH__ != 103) && \
            (__AVR_ARCH__ != 104))
  #error CPU not supported by this target
  #include BUILD_STOP
#endif

extern const uint8_t* __vectors;

namespace FlashNVM {

#if defined(HAVE_RAMPZ)
  /*  AVR_Dx 24bit */

  void nvm_spm (uint32_t _addr);
  void nvm_write (uint32_t _addr, uint8_t *_data);

#elif (NVMCTRL_VER != 0)
  /*  AVR_Dx/Ex 16bit */

  void nvm_spm (uint16_t _addr);
  void nvm_write (uint16_t _addr, uint8_t *_data);

#else
  /*  megaAVR, tinyAVR 16bit */

  void nvm_spm (uint16_t _addr);
  void nvm_write (uint16_t _addr, uint8_t _data);

#endif

  void nvm_cmd (uint8_t _nvm_cmd);

  bool spm_support_check (void);
  inline bool nvmstat (void) {
    return NVMCTRL_STATUS == 0;
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size = 1);
  inline bool page_erase_P (const void* _page_addr, size_t _page_size = 1) {
    return page_erase_PF ((const nvmptr_t)_page_addr, _page_size);
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size);
  inline bool page_update_P (const void* _page_addr, const void* _data_addr, size_t _save_size) {
    return page_update_PF ((const nvmptr_t)_page_addr, _data_addr, _save_size);
  }
}

// end of code
