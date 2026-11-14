/**
 * @file USB/Utils.h
 * @author askn (K.Sato) multix.jp
 * @brief A collection of functions for USB devices.
 * @version 0.4.12
 * @date 2026-11-13
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#pragma once
#include "Descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16_t wVidPid[2];
  uint32_t dwSerialNumber;
} __attribute__((__packed__)) User_EEP_t;

extern size_t get_descriptor_copy_P (uint8_t* _buffer, const uint16_t _wValue, const uint8_t* _descriptor_set);
extern size_t get_descriptor_copy (uint8_t* _buffer, const uint16_t _wValue, const uint8_t* _descriptor_set);
extern size_t get_auto_generate_serial_number_string (uint8_t* _buffer, uint8_t _check_eeprom);

#ifdef __cplusplus
}
#endif

// end of header