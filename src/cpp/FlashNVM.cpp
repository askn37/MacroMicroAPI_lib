/**
 * @file FlashNVM.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../FlashNVM.h"
#include <avr/pgmspace.h>
#include <string.h>

namespace FlashNVM {
/**************************
 * AVR DA/DB/DD/EA Series *
 **************************/
#if (NVMCTRL_VER == 2)

  bool spm_support_check (void) {
    // Serial.print(F(" MAGICNUMBER=0x")).println(pgm_read_dword(PROGMEM_START + 2), HEX);
    return ((uint16_t)&__vectors > 0) &&
      (pgm_read_dword(PROGMEM_START + 2) == 0x950895F8);
  }

  void nvm_ctrl (uint8_t _nvm_cmd) {
    while (NVMCTRL_STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm));
    _PROTECTED_WRITE_SPM(NVMCTRL_CTRLA, _nvm_cmd);
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr;
    nvm_ctrl(NVMCTRL_CMD_NONE_gc);  // stop control
    nvm_ctrl(NVMCTRL_CMD_FLPER_gc); // flash page erase command
    do {
    loop_until_bit_is_clear(NVMCTRL_STATUS, NVMCTRL_FBUSY_bp);
    __asm__ __volatile__ (
      "MOV     ZL, %A1 \n"      // set (LO) R30
      "MOV     ZH, %B1 \n"      // set (HI) R31
  #if defined(HAVE_RAMPZ)
      "STS     %2, %C1 \n"      // RAMPZ <- (HH)
  #endif
      "CALL    %0"              // SPM Z+ (dummy write)
      :
      : "p" (PROGMEM_START + 2) // spm_zp()
      , "r" (_page_top)
  #if defined(HAVE_RAMPZ)
      , "p" (&RAMPZ)
  #endif
      : "r31", "r30"
      );
      if (_page_size <= PROGMEM_PAGE_SIZE) break;
      _page_top += PROGMEM_PAGE_SIZE;
      _page_size -= PROGMEM_PAGE_SIZE;
    } while (_page_size != 0);
    nvm_ctrl(NVMCTRL_CMD_NONE_gc);
    return nvmstat();
  }

  bool page_update_PF (const nvmptr_t _page_addr, const void* _data_addr, size_t _save_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr & ~((nvmptr_t)1);
    uint16_t _data_top = (uint16_t)_data_addr;
    _save_size >>= 1;
    nvm_ctrl(NVMCTRL_CMD_NONE_gc); // stop control
    nvm_ctrl(NVMCTRL_CMD_FLWR_gc); // flash page erase command
    __asm__ __volatile__ (
      "MOV     ZL, %A1 \n"      // set (LO) R30
      "MOV     ZH, %B1 \n"      // set (HI) R31
  #if defined(HAVE_RAMPZ)
      "STS     %4, %C1 \n"      // RAMPZ <- (HH)
  #endif
      "1:"
      "LD      R0, X+  \n"
      "LD      R1, X+  \n"
      "CALL    %0      \n"      // SPM Z+ (dummy write)
      "SBIW    %3, 1   \n"
      "BRNE    1b      \n"
      "CLR     __zero_reg__"
    :
    : "p" (PROGMEM_START + 2)   // spm_zp()
    , "r" (_page_top)
    , "x" (_data_top)
    , "w" (_save_size)
  #if defined(HAVE_RAMPZ)
    , "p" (&RAMPZ)
  #endif
    : "r31", "r30"
    );
    nvm_ctrl(NVMCTRL_CMD_NONE_gc);
    return nvmstat();
  }

/**************************
 * megaAVR/tinyAVR Series *
 **************************/
#elif (NVMCTRL_VER == 1)

  bool spm_support_check (void) {
    // Serial.print(F(" MAGICNUMBER=0x")).println(*((uint32_t*)(MAPPED_PROGMEM_START + 2)), HEX);
    return ((uint16_t)&__vectors > 0) &&
      (*((uint32_t*)(MAPPED_PROGMEM_START + 2)) == 0xE99DC009);
  }

  bool page_erase_PF (const nvmptr_t _page_addr, size_t _page_size) {
    nvmptr_t _page_top = (nvmptr_t)_page_addr + MAPPED_PROGMEM_START;
    do {
      nvmwrite(_page_top, 0xFF);
      nvmctrl(NVMCTRL_CMD_PAGEERASE_gc);
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
        nvmwrite(_page_top++, buffer[_buff_off]);
      } while (++_buff_off < PROGMEM_PAGE_SIZE);
      nvmctrl(NVMCTRL_CMD_PAGEWRITE_gc);
    }
    return nvmstat();
  }
#endif
}

// end of code
