/**
 * @file UrowNVM.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.3
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024 askn at github.com
 *
 */
#include "../UrowNVM.h"
#include <string.h>

namespace UrowNVM {

bool nvm_result (void) { return NVMCTRL_STATUS == 0; }
void nvm_wait (void) { while (NVMCTRL_STATUS & 3); }
void nvm_cmd (uint8_t _nvm_cmd) {
  _PROTECTED_WRITE_SPM(NVMCTRL_CTRLA, _nvm_cmd);
}

#if (AVR_NVMCTRL == 2 || AVR_NVMCTRL == 4)
  /* AVR_Dx */

  bool userrow_clear (void) {
    nvm_cmd(NVMCTRL_CMD_FLPER_gc);
    *((uint8_t*)USER_SIGNATURES_START) = 0; /* write dummy byte */
    nvm_cmd(NVMCTRL_CMD_NONE_gc);
    nvm_wait();
    return nvm_result();
  }

  void userrow_load (void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    memcpy(_data_addr, (const void*)USER_SIGNATURES_START, _save_size);
  }

  bool userrow_save (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_FLWR_gc);
    memcpy((void*)USER_SIGNATURES_START, _data_addr, _save_size);
    nvm_cmd(NVMCTRL_CMD_NONE_gc);
    nvm_wait();
    return nvm_result();
  }

#elif (AVR_NVMCTRL == 3 || AVR_NVMCTRL == 5)
  /* AVR_Ex */

  bool userrow_clear (void) {
    nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
    *((uint8_t*)USER_SIGNATURES_START) = 0; /* write dummy byte */
    nvm_cmd(NVMCTRL_CMD_FLPER_gc);
    nvm_wait();
    return nvm_result();
  }

  void userrow_load (void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    memcpy(_data_addr, (const void*)USER_SIGNATURES_START, _save_size);
  }

  bool userrow_save (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    nvm_wait();
    nvm_cmd(NVMCTRL_CMD_NOCMD_gc);
    memcpy((void*)USER_SIGNATURES_START, _data_addr, _save_size);
    nvm_cmd(NVMCTRL_CMD_FLPW_gc);
    nvm_wait();
    return nvm_result();
  }

#else
  /* tinyAVR, megaAVR 16bit : not SPM+ */

  bool userrow_clear (void) {
    memset((void*)USER_SIGNATURES_START, 0xFF, USER_SIGNATURES_SIZE);
    nvm_cmd(NVMCTRL_CMD_PAGEERASEWRITE_gc);
    nvm_wait();
    return nvm_result();
  }

  void userrow_load (void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    memcpy(_data_addr, (const void*)USER_SIGNATURES_START, _save_size);
  }

  bool userrow_save (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    memcpy((void*)USER_SIGNATURES_START, _data_addr, _save_size);
    nvm_cmd(NVMCTRL_CMD_PAGEWRITE_gc);
    nvm_wait();
    return nvm_result();
  }

#endif /* leave AVR_NVMCTRL == 0 */

  bool userrow_verify (const void* _data_addr, size_t _save_size) {
    assert(_save_size > 0);
    assert(_save_size <= USER_SIGNATURES_SIZE);
    nvm_wait();
    return !memcmp((const void*)USER_SIGNATURES_START, _data_addr, _save_size);
  }
}

// end of code
