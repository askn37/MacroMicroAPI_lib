/**
 * @file FlashNVM.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <avr/io.h>
#include <stddef.h>

#if defined(RAMZ)
  /* 128KiB model */
  typedef int32_t nvmptr_t;
  #define HAVE_RAMPZ
#else
  /* under 64KiB model */
  typedef int16_t nvmptr_t;
  #define NOTUSE_RAMPZ
#endif

#if (LOCKBITS_DEFAULT == 0x5CC5C55C)
  /* AVR DA/DB/DD/EA Series */
  #define NVMCTRL_VER 2
#elif (LOCKBITS_DEFAULT == 0xC5)
  /* megaAVR/tinyAVR Series */
  #define NVMCTRL_VER 1
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
/**************************
 * AVR DA/DB/DD/EA Series *
 **************************/
#if (NVMCTRL_VER == 2)
  bool spm_support_check (void);
    inline bool nvmstat (void) {
    return NVMCTRL_STATUS == 0;
    }
  void nvm_ctrl (uint8_t _nvm_cmd = NVMCTRL_CMD_NONE_gc);

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size = PROGMEM_PAGE_SIZE);
  inline bool page_erase (const void* _page_addr, size_t _page_size = PROGMEM_PAGE_SIZE) {
      return page_erase_PF ((const nvmptr_t)_page_addr, _page_size);
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size);
  inline bool page_update (const void* _page_addr, const void* _data_addr, size_t _save_size) {
      return page_update_PF ((const nvmptr_t)_page_addr, _data_addr, _save_size);
  }

/**************************
 * megaAVR/tinyAVR Series *
 **************************/
#elif (NVMCTRL_VER == 1)

    typedef void (*nvmctrl_t) (uint8_t _nvm_cmd);
    typedef void (*nvmwrite_t) (uint16_t _address, uint8_t _data);

    const nvmctrl_t nvmctrl = (nvmctrl_t)((PROGMEM_START + 4) >> 1);
    const nvmwrite_t nvmwrite = (nvmwrite_t)((PROGMEM_START + 2) >> 1);

  bool spm_support_check (void);
    inline bool nvmstat (void) {
    return NVMCTRL_STATUS == 0;
    }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size = PROGMEM_PAGE_SIZE);
  inline bool page_erase (const void* _page_addr, size_t _page_size = PROGMEM_PAGE_SIZE) {
      return page_erase_PF ((const nvmptr_t)_page_addr, _page_size);
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size);
  inline bool page_update (const void* _page_addr, const void* _data_addr, size_t _save_size) {
      return page_update_PF ((const nvmptr_t)_page_addr, _data_addr, _save_size);
  }

#endif
}

// end of code
