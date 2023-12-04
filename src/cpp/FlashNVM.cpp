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
/*****************
 * AVR EA Series *
 *****************/
#if (NVMCTRL_VER == 3)
  /* The maximum flash size for this series is 64KiB */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (pgm_read_dword(PROGMEM_START + 2) == 0x950895F8);
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    do {
      nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
      __asm__ __volatile__ (
        R"#ASM#(
          CALL  0x2     ; SPM Z+ (dummy write)
        )#ASM#"
        :
        : "z" (_page_top)
      );
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
      __asm__ __volatile__ (
        R"#ASM#(
      1:  LD    R0, X+  ; R0 <- (buffer)++
          LD    R1, X+  ; R1 <- (buffer)++
          CALL  0x2     ; SPM Z+
          SBIW  %0, 1   ; (PROGMEM_PAGE_SIZE / 2)--
          BRNE  1b      ;
          CLR   __zero_reg__
        )#ASM#"
        :
        : "w" (PROGMEM_PAGE_SIZE / 2)
        , "x" (buffer)
        , "z" (_page_top)
      );
      nvm_cmd(NVMCTRL_CMD_FLPW_gc);
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvmstat();
  }

/***********************
 * AVR DA/DB/DD Series *
 ***********************/
#elif (NVMCTRL_VER == 2)
  /* The maximum flash size for this series is 128KiB */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (pgm_read_dword(PROGMEM_START + 2) == 0x950895F8);
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    do {
      nvm_cmd(NVMCTRL_CMD_FLPER_gc);
    #if defined(HAVE_RAMPZ)
      __asm__ __volatile__ (
        R"#ASM#(
          STS   %1, %C0 ; RAMPZ <- (_page_top:HH
          MOVW  Z, %A0  ; Z <- (_page_top)
          CALL  0x2     ; SPM Z+ (dummy write)
        )#ASM#"
        :
        : "r" (_page_top)
        , "p" (&RAMPZ)
      );
    #else
      __asm__ __volatile__ (
        R"#ASM#(
          CALL  0x2     ; SPM Z+ (dummy write)
        )#ASM#"
        :
        : "z" (_page_top)
      );
    #endif
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
    #if defined(HAVE_RAMPZ)
      __asm__ __volatile__ (
        R"#ASM#(
          STS   %3, %C2 ; RAMPZ <- (_page_top:HH)
          MOVW  Z, %A2  ; Z <- (_page_top)
      1:  LD    R0, X+  ; R0 <- (buffer)++
          LD    R1, X+  ; R1 <- (buffer)++
          CALL  0x2     ; SPM Z+
          SBIW  %0, 1   ; (PROGMEM_PAGE_SIZE / 2)--
          BRNE  1b      ;
          CLR   __zero_reg__
        )#ASM#"
        :
        : "r" (PROGMEM_PAGE_SIZE / 2)
        , "x" (buffer)
        , "r" (_page_top)
        , "p" (&RAMPZ)
      );
    #else
      __asm__ __volatile__ (
        R"#ASM#(
      1:  LD    R0, X+  ; R0 <- (buffer)++
          LD    R1, X+  ; R1 <- (buffer)++
          CALL  0x2     ; SPM Z+
          SBIW  %0, 1   ; (PROGMEM_PAGE_SIZE / 2)--
          BRNE  1b      ;
          CLR   __zero_reg__
        )#ASM#"
        :
        : "r" (PROGMEM_PAGE_SIZE / 2)
        , "x" (buffer)
        , "z" (_page_top)
      );
    #endif
      nvm_cmd(NVMCTRL_CMD_NONE_gc);
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvmstat();
  }

/**************************
 * megaAVR/tinyAVR Series *
 **************************/
#elif (NVMCTRL_VER == 0)
  /* The maximum flash size for this series is 48KiB */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (*((uint32_t*)(MAPPED_PROGMEM_START + 2)) == 0xE99DC009);
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr + MAPPED_PROGMEM_START;
    do {
      nvm_write(_page_top, 0xFF);
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
      do {
        nvm_write(_page_top++, buffer[_buff_off]);
      } while (++_buff_off < PROGMEM_PAGE_SIZE);
      nvm_cmd(NVMCTRL_CMD_PAGEWRITE_gc);
    }
    return nvmstat();
  }

#endif
}

// end of code
