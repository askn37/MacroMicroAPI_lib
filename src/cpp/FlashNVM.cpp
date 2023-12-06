/**
 * @file FlashNVM.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023 askn at github.com
 *
 */
#include "../FlashNVM.h"
#include <avr/pgmspace.h>
#include <string.h>

namespace FlashNVM {

#if (NVMCTRL_VER != 0)
  /* AVR_Dx/Ex */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (pgm_read_dword(PROGMEM_START + 2) == 0x950895F8);
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_cmd (uint8_t _nvm_cmd) {
    __asm__ __volatile__ (
      R"#ASM#(
        JMP   0x6
      )#ASM#"
    );
  }

  #if defined(HAVE_RAMPZ)
  /* AVR_Dx 24bit */

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_spm (uint32_t _addr) {
    __asm__ __volatile__ (
      R"#ASM#(
        STS   %0, R24 ; RAMPZ <- (_addr:HH)
        MOVW  Z, R22  ; Z <- (_addr)
        JMP   0x2
      )#ASM#"
      :
      : "p" (&RAMPZ)
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint32_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      R"#ASM#(
        STS   %0, R24       ; RAMPZ <- (_addr:HH)
        MOVW  Z, R22        ; Z <- (_addr:HI:LO)
        MOVW  X, R20        ; X <- (_data)
        LDI   R24, lo8(%1)  ;
        LDI   R25, hi8(%1)  ;
    1:  LD    R0, X+        ; R0 <- (_data)++
        LD    R1, X+        ; R1 <- (_data)++
        CALL  0x2           ; SPM Z+
        SBIW  R24, 1        ; (PROGMEM_PAGE_SIZE / 2)--
        BRNE  1b            ;
        CLR   __zero_reg__  ;
        RET                 ;
      )#ASM#"
      :
      : "p" (&RAMPZ)
      , "p" (PROGMEM_PAGE_SIZE / 2)
    );
  }

  #else /* NOTUSE_RAMPZ */
  /* AVR_Dx/Ex 16bit */

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_spm (uint16_t _addr) {
    __asm__ __volatile__ (
      R"#ASM#(
        JMP   0x2
      )#ASM#"
      :
      : "z" (_addr)
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint16_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      R"#ASM#(
        MOVW  Z, R24        ; Z <- (_addr:HI:LO)
        MOVW  X, R22        ; X <- (_data)
        LDI   R24, lo8(%0)  ;
        LDI   R25, hi8(%0)  ;
    1:  LD    R0, X+        ; R0 <- (_data)++
        LD    R1, X+        ; R1 <- (_data)++
        CALL  0x2           ; SPM Z+
        SBIW  R24, 1        ; (PROGMEM_PAGE_SIZE / 2)--
        BRNE  1b            ;
        CLR   __zero_reg__  ;
        RET                 ;
      )#ASM#"
      :
      : "p" (PROGMEM_PAGE_SIZE / 2)
    );
  }
  #endif /* HAVE_RAMPZ */

  #if (NVMCTRL_VER == 3)
  /* AVR_Ex */

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
      nvm_spm(_page_top);
      nvm_cmd(NVMCTRL_CMD_FLPER_gc);
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size > 0);
    return nvmstat();
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size) {
    uint8_t* _data_top = (uint8_t*)_data_addr;
    uint8_t _buff_off = (nvmptr_t)_page_addr & (PROGMEM_PAGE_SIZE - 1);
    nvmptr_t _page_top = (nvmptr_t)_page_addr - _buff_off;
    uint8_t buffer[PROGMEM_PAGE_SIZE];
    while (_save_size) {
      memset(&buffer, 0xFF, PROGMEM_PAGE_SIZE);
      do {
        buffer[_buff_off] = *((uint8_t*)_data_top++);
      } while (--_save_size > 0 && ++_buff_off < PROGMEM_PAGE_SIZE);
      _buff_off = 0;
      nvm_cmd(NVMCTRL_CMD_FLPBCLR_gc);
      nvm_write (_page_top, buffer);
      nvm_cmd(NVMCTRL_CMD_FLPW_gc);
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvmstat();
  }

  #else /* NVMCTRL_VER == 2 */
  /* AVR_Dx */

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_cmd(NVMCTRL_CMD_FLPER_gc);
      nvm_spm(_page_top);
      nvm_cmd(NVMCTRL_CMD_NONE_gc);
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size > 0);
    return nvmstat();
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size) {
    uint8_t* _data_top = (uint8_t*)_data_addr;
    uint8_t _buff_off = (nvmptr_t)_page_addr & (PROGMEM_PAGE_SIZE - 1);
    nvmptr_t _page_top = (nvmptr_t)_page_addr - _buff_off;
    uint8_t buffer[PROGMEM_PAGE_SIZE];
    while (_save_size) {
      memset(&buffer, 0xFF, PROGMEM_PAGE_SIZE);
      do {
        buffer[_buff_off] = *((uint8_t*)_data_top++);
      } while (--_save_size > 0 && ++_buff_off < PROGMEM_PAGE_SIZE);
      _buff_off = 0;
      nvm_cmd(NVMCTRL_CMD_FLWR_gc);
      nvm_write (_page_top, buffer);
      nvm_cmd(NVMCTRL_CMD_NONE_gc);
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvmstat();
  }
  #endif /* NVMCTRL_VER == 2 */
#endif /* NVMCTRL_VER != 0 */

#if (NVMCTRL_VER == 0)
  /* tinyAVR, megaAVR 16bit : not SPM+ */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (*((uint32_t*)(MAPPED_PROGMEM_START + 2)) == 0x95089201);
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_cmd (uint8_t _nvm_cmd) {
    __asm__ __volatile__ (
      R"#ASM#(
        JMP   0x6
      )#ASM#"
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_spm (uint16_t _addr) {
    __asm__ __volatile__ (
      R"#ASM#(
        MOVW  Z, R24
        JMP   0x2
      )#ASM#"
      :
      : "z" (_addr)
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint16_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      R"#ASM#(
        MOVW  Z, R24        ; Z <- (_addr:HI:LO)
        MOVW  X, R22        ; X <- (_data)
        LDI   R24, lo8(%0)  ;
        LDI   R25, hi8(%0)  ;
    1:  LD    R0, X+        ; R0 <- (_data)++
        CALL  0x2           ; ST Z+, R0
        SBIW  R24, 1        ; (PROGMEM_PAGE_SIZE)--
        BRNE  1b            ;
        RET                 ;
      )#ASM#"
      :
      : "p" (PROGMEM_PAGE_SIZE)
    );
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr + MAPPED_PROGMEM_START;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_spm(_page_top);
      nvm_cmd(NVMCTRL_CMD_PAGEERASE_gc);
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size != 0);
    return nvmstat();
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size) {
    uint8_t* _data_top = (uint8_t*)_data_addr;
    uint8_t _buff_off = (nvmptr_t)_page_addr & (PROGMEM_PAGE_SIZE - 1);
    nvmptr_t _page_top = (nvmptr_t)_page_addr + MAPPED_PROGMEM_START - _buff_off;
    uint8_t buffer[PROGMEM_PAGE_SIZE];
    while (_save_size) {
      memset(&buffer, 0xFF, PROGMEM_PAGE_SIZE);
      do {
        buffer[_buff_off] = *((uint8_t*)_data_top++);
      } while (--_save_size > 0 && ++_buff_off < PROGMEM_PAGE_SIZE);
      _buff_off = 0;
      nvm_write (_page_top, buffer);
      nvm_cmd(NVMCTRL_CMD_PAGEWRITE_gc);
    }
    return nvmstat();
  }

#endif /* NVMCTRL_VER == 0 */
}

// end of code
