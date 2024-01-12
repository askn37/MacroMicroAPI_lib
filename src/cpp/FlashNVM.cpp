/**
 * @file FlashNV.cpp
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
  bool nvm_result (void) { return NVMCTRL_STATUS == 0; }
  void nvm_wait (void) { while (NVMCTRL_STATUS & 3); }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_stc (uint16_t _addr) {
    __asm__ __volatile__ (
      /* Z <- R24:R25 <- &_addr */
      R"#ASM#(
        MOVW  Z, R24
        JMP   0x0002        ; ST Z+, Dummy
      )#ASM#"
      ::: "r30", "r31"
    );
  }

#if (AVR_NVMCTRL != 0)
  /* AVR_Dx/Ex */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (pgm_read_dword(PROGMEM_START + 2) == 0x95089361L);
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_stz (uint16_t _addr, uint8_t _data) {
    __asm__ __volatile__ (
      /* Z   <- R24:R25 <- &_addr */
      /* R22 <- R22     <- _data */
      R"#ASM#(
        MOVW  Z, R24
        JMP   0x0002        ; ST Z+, R22
      )#ASM#"
      ::: "r30", "r31"
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  uint8_t nvm_ldz (uint16_t _addr) {
    __asm__ __volatile__ (
      /* Z <- R24:R25 <- &_addr */
      R"#ASM#(
        MOVW  Z, R24
        JMP   0x0006        ; LD R24, Z+
      )#ASM#"
      ::: "r30", "r31"
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_cmd (uint8_t _nvm_cmd) {
    __asm__ __volatile__ (
      /* R24 <- _nvm_cmd */
      R"#ASM#(
        JMP   0x000E        ; call nvm_cmd
      )#ASM#"
      ::: "r24", "r25"
    );
  }

  #if (__AVR_ARCH__ == 104)
  /* AVR_Dx 24bit */

  /* 24bit version */
  __attribute__((naked))
  __attribute__((noinline))
  void nvm_spm (uint32_t _addr) {
    /* R22:R23:R24:R25 <- &_addr */
    __asm__ __volatile__ (
      R"#ASM#(
        STS   %0, R24       ; RAMPZ <- (_addr:hh8)
        MOVW  Z, R22        ; Z <- (_addr:lo8)
        JMP   0x000A        ; SPM Z+
      )#ASM#"
      :
      : "p" (&RAMPZ)
    );
  }

  /* 24bit version */
  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint32_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      /* R22:R23:R24:R25 <- &_addr */
      /* R20             <- &_data */
      R"#ASM#(
        STS   %0, R24       ; RAMPZ <- (_addr:hh8)
        MOVW  Z, R22        ; Z <- (_addr:HI:lo8)
        MOVW  X, R20        ; X <- (_data)
    1:  LD    R0, X+        ; R0 <- (_data)++
        LD    R1, X+        ; R1 <- (_data)++
        CALL  0x000A        ; SPM Z+ ; nvm_spm
        SBIW  %1, 1         ; (PROGMEM_PAGE_SIZE / 2) -= 1
        BRNE  1b            ;
        CLR   __zero_reg__  ;
        RET                 ;
      )#ASM#"
      :
      : "p" (&RAMPZ)
      , "w" (PROGMEM_PAGE_SIZE / 2)
      : "r26", "r27", "r30", "r31"  /* X,Z */
    );
  }

  #else /* (__AVR_ARCH__ != 104) */
  /* AVR_Dx/Ex 16bit */

  /* 16bit version */
  __attribute__((naked))
  __attribute__((noinline))
  void nvm_spm (uint16_t _addr) {
    __asm__ __volatile__ (
      /* Z <- R24:R25 <- &_addr */
      R"#ASM#(
        MOVW  Z, R24
        JMP   0x000A        ; SPM Z+
      )#ASM#"
      ::: "r30", "r31"
    );
  }

  /* 16bit version */
  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint16_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      /* R24:R25 <- &_addr */
      /* R22:R23 <- &_data */
      R"#ASM#(
        MOVW  Z, R24        ; Z <- (_addr:HI:LO)
        MOVW  X, R22        ; X <- (_data)
        LDI   R24, lo8(%0)  ;
        LDI   R25, hi8(%0)  ;
    1:  LD    R0, X+        ; R0 <- (_data)++
        LD    R1, X+        ; R1 <- (_data)++
        CALL  0x000A        ; SPM Z+
        SBIW  R24, 1        ; (PROGMEM_PAGE_SIZE / 2)--
        BRNE  1b            ;
        CLR   __zero_reg__  ;
        RET                 ;
      )#ASM#"
      :
      : "p" (PROGMEM_PAGE_SIZE / 2)
      : "r26", "r27", "r30", "r31"  /* X,Z */
    );
  }
  #endif /* leave (__AVR_ARCH__ == 104) */

  #if (AVR_NVMCTRL == 3) || (AVR_NVMCTRL == 5)
  /* AVR_Ex */

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
      nvm_spm(_page_top); /* write dummy byte */
      nvm_cmd(NVMCTRL_CMD_FLPER_gc);
      nvm_wait();
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size > 0);
    return nvm_result();
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size) {
    uint8_t* _data_top = (uint8_t*)_data_addr;
    uint16_t _buff_off = (nvmptr_t)_page_addr & (PROGMEM_PAGE_SIZE - 1);
    nvmptr_t _page_top = (nvmptr_t)_page_addr - _buff_off;
    uint8_t buffer[PROGMEM_PAGE_SIZE];
    while (_save_size) {
      memset(&buffer, 0xFF, PROGMEM_PAGE_SIZE);
      do {
        buffer[_buff_off] = *((uint8_t*)_data_top++);
      } while (--_save_size > 0 && ++_buff_off < PROGMEM_PAGE_SIZE);
      _buff_off = 0;
      nvm_cmd(NVMCTRL_CMD_FLPBCLR_gc);
      nvm_write(_page_top, buffer);
      nvm_cmd(NVMCTRL_CMD_FLPW_gc);
      nvm_wait();
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvm_result();
  }

    #ifdef FLASHNVM_BOOTROW
  /* AVR_EB */
  bool bootrow_clear (void) {
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
    nvm_stc(BOOTROW_START);
    nvm_cmd(NVMCTRL_CMD_FLPERW_gc);
    nvm_wait();
    return 0 == (NVMCTRL_STATUS & 3);
  }

  bool bootrow_save (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= BOOTROW_SIZE);
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
    uint16_t _p = BOOTROW_START;
    uint8_t *_q = (uint8_t*)_data_addr;
    do { nvm_stz(_p++, *(_q++)); } while (--_save_size);
    nvm_cmd(NVMCTRL_CMD_FLPERW_gc);
    nvm_wait();
    return 0 == (NVMCTRL_STATUS & 3);
  }
    #endif  /* leave BOOTROW_SIZE */

  #else /* (AVR_NVMCTRL == 2) || (AVR_NVMCTRL == 4) */
  /* AVR_Dx */

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_cmd(NVMCTRL_CMD_FLPER_gc);
      nvm_spm(_page_top); /* write dummy byte */
      nvm_wait();
      nvm_cmd(NVMCTRL_CMD_NONE_gc);
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size > 0);
    return nvm_result();
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
      nvm_write(_page_top, buffer);
      nvm_wait();
      nvm_cmd(NVMCTRL_CMD_NONE_gc);
      _page_top += PROGMEM_PAGE_SIZE;
    }
    return nvm_result();
  }

    #ifdef FLASHNVM_BOOTROW
  /* AVR_DU */
  __attribute__((noinline))
  bool bootrow_clear (void) {
    nvm_cmd(NVMCTRL_CMD_FLPER_gc);
    nvm_stc(BOOTROW_START); /* write dummy byte */
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_NONE_gc);
    return 0 == (NVMCTRL_STATUS & 3);
  }

  __attribute__((noinline))
  bool bootrow_save (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= BOOTROW_SIZE);
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_FLWR_gc);
    uint16_t _p = BOOTROW_START;
    uint8_t *_q = (uint8_t*)_data_addr;
    do { nvm_stz(_p++, *(_q++)); } while (_save_size);
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_NONE_gc);
    return 0 == (NVMCTRL_STATUS & 3);
  }
    #endif  /* leave BOOTROW_SIZE */

  #endif /* leave (AVR_NVMCTRL == 3) || (AVR_NVMCTRL == 5) */
#else /* (AVR_NVMCTRL == 0) */
  /* tinyAVR, megaAVR 16bit : not SPM+ */

  bool spm_support_check (void) {
    return ((uint16_t)&__vectors > 0) &&
      (*((uint32_t*)(MAPPED_PROGMEM_START + 2)) == 0x95089361L);
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_cmd (uint8_t _nvm_cmd) {
    __asm__ __volatile__ (
      /* R24 <- _nvm_cmd */
      R"#ASM#(
        JMP   0x0006        ; nvm_cmd function
      )#ASM#"
      ::: "r25"
    );
  }

  __attribute__((naked))
  __attribute__((noinline))
  void nvm_write (uint16_t _addr, uint8_t *_data) {
    __asm__ __volatile__ (
      /* R24:R25 <- &_addr */
      /* R22:R23 <- &_data */
      R"#ASM#(
        MOVW  Z, R24        ; Z <- (_addr:HI:LO)
        MOVW  X, R22        ; X <- (_data)
        LDI   R24, lo8(%0)  ;
        LDI   R25, hi8(%0)  ;
    1:  LD    R22, X+       ; R22 <- (_data)++
        CALL  0x0002        ; ST Z+, R22
        SBIW  R24, 1        ; (PROGMEM_PAGE_SIZE / 2)--
        BRNE  1b            ;
        RET                 ;
      )#ASM#"
      :
      : "p" (PROGMEM_PAGE_SIZE / 2)
      : "r22", "r26", "r27", "r30", "r31"  /* X,Z */
    );
  }

  /* NVMCTRL v0 */
  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr + MAPPED_PROGMEM_START;
    _page_top -= _page_top & (PROGMEM_PAGE_SIZE - 1);
    do {
      nvm_stc(_page_top); /* write dummy byte */
      nvm_cmd(NVMCTRL_CMD_PAGEERASE_gc);
      nvm_wait();
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size != 0);
    return nvm_result();
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
      nvm_write(_page_top, buffer);
      nvm_cmd(NVMCTRL_CMD_PAGEWRITE_gc);
      nvm_wait();
    }
    return nvm_result();
  }

#endif  /* leave (AVR_NVMCTRL == 0) */

#ifdef FLASHNVM_BOOTROW
  __attribute__((noinline))
  void bootrow_load (void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= BOOTROW_SIZE);
    nvm_wait();
    __asm__ __volatile__ (
      R"#ASM#(
    1:  CALL  0x0006        ; LD R24, Z++ ; nvm_ldz
        ST    X+, R24       ; X+ <- R24
        DEC   %0            ;
        BRNE  1b            ;
      )#ASM#"
      :
      : "r" (_save_size)
      , "x" (_data_addr)
      , "z" (BOOTROW_START)
      : "r24"
    );
  }

  __attribute__((noinline))
  bool bootrow_verify (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= BOOTROW_SIZE);
    nvm_wait();
    bool _result;
    __asm__ __volatile__ (
      R"#ASM#(
        CLR   %0
    1:  CALL  0x0006        ; LD R24, Z++ ; nvm_ldz
        LD    R25, X+       ;
        EOR   R24, R25      ;
        BRNE  2f            ;
        DEC   %1            ;
        BRNE  1b            ;
        DEC   %0
    2:
      )#ASM#"
      : "=r" (_result)
      : "d" (_save_size)
      , "x" (_data_addr)
      , "z" (BOOTROW_START)
      : "r24", "r25"
    );
    return _result;
  }
#endif  /* leave BOOTROW_SIZE */
}

// end of code
