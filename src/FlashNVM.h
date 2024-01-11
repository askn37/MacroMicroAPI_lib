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

#if (__AVR_ARCH__ == 104)
  /* 128KiB model */
  typedef int32_t nvmptr_t;
#else
  /* under 64KiB model */
  typedef int16_t nvmptr_t;
#endif

extern const uint8_t* __vectors;

namespace FlashNVM {

#if (__AVR_ARCH__ == 104)
  /*  AVR_Dx 24bit */

  void nvm_stc (uint16_t _addr);                  /* 0x0002: ST Z+, dummy */
  void nvm_stz (uint16_t _addr, uint8_t _data);   /* 0x0002: ST Z+, R0 */
  uint8_t nvm_ldz (uint16_t _addr);               /* 0x0006: LD R24, Z+ */
  void nvm_spm (uint32_t _addr);                  /* 0x000A: SPM Z+ */
  void nvm_write (uint32_t _addr, uint8_t *_data);

#elif (AVR_NVMCTRL != 0)
  /*  AVR_Dx/Ex 16bit */

  void nvm_stc (uint16_t _addr);                  /* 0x0002: ST Z+, dummy */
  void nvm_stz (uint16_t _addr, uint8_t _data);   /* 0x0002: ST Z+, R0 */
  uint8_t nvm_ldz (uint16_t _addr);               /* 0x0006: LD R24, Z+ */
  void nvm_spm (uint16_t _addr);                  /* 0x000A: SPM Z+ */
  void nvm_write (uint16_t _addr, uint8_t *_data);

#else   /* AVR_NVMCTRL == 0 */
  /*  megaAVR, tinyAVR 16bit */

  void nvm_stc (uint16_t _addr);    /* 0x0002: ST Z+, dummy */
  void nvm_write (uint16_t _addr, uint8_t *_data);

#endif  /* leave AVR_NVMCTRL == 0 */

  void nvm_cmd (uint8_t _nvm_cmd);  /* 0x000E:(v2~) or 0x0006:(v0) */

  bool spm_support_check (void);
  bool nvm_result (void);
  void nvm_wait (void);

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size = 1);
  inline bool page_erase_P (const void* _page_addr, size_t _page_size = 1) {
    return page_erase_PF ((const nvmptr_t)_page_addr, _page_size);
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size);
  inline bool page_update_P (const void* _page_addr, const void* _data_addr, size_t _save_size) {
    return page_update_PF ((const nvmptr_t)_page_addr, _data_addr, _save_size);
  }

#ifdef BOOTROW_SIZE
  #define FLASHNVM_BOOTROW
  bool bootrow_clear (void);
  void bootrow_load (void* _data_addr, size_t _save_size = BOOTROW_SIZE);
  bool bootrow_save (const void* _data_addr, size_t _save_size = BOOTROW_SIZE);
  bool bootrow_verify (const void* _data_addr, size_t _save_size = BOOTROW_SIZE);
#endif

}

// end of code
