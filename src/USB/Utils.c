/**
 * @file USB/Utils.cpp
 * @author askn (K.Sato) multix.jp
 * @brief A collection of functions for USB devices.
 * @version 0.4.12
 * @date 2026-11-13
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#include <avr/io.h>
#include <avr/pgmspace.h>   /* memcpy_P PROGMEM */
#include <string.h>         /* memcpy */
#include "api/btools.h"     /* btoh crc32 */
#include "api/capsule.h"
#include "Utils.h"
#include "Descriptor.h"

__attribute__((__noinline__))
__attribute__((section(".text")))
size_t get_descriptor_copy_P (uint8_t* _buffer, const uint16_t _wValue, const uint8_t* _descriptor_set) {
  uint8_t _request = _wValue >> 8;
  uint8_t _index = _wValue;
  uint8_t _hint, _type, _size;
  uint8_t* _pgmem = (uint8_t*)_descriptor_set;
  do {
    _hint = pgm_read_byte(_pgmem++);
    _size = pgm_read_byte(&_pgmem[0]);
    _type = pgm_read_byte(&_pgmem[1]);
    if ((_size + 2) < 4) break;
    if (_type == bDescriptorType_CONFIGURATION ||
        _type == bDescriptorType_OTHER_SPEED_CONFIGURATION) {
      _size = pgm_read_byte(&_pgmem[2]);
    }
    if (_type == _request && _hint == _index) {
      if (_type == bDescriptorType_STRING && _index != 0) {
        uint8_t* _src = (uint8_t*)_descriptor_set + pgm_read_byte(&_pgmem[2]);
        uint8_t* _dst = _buffer;
        uint8_t _len = ++_size << 1;
        *_dst++ = _len; *_dst++ = bDescriptorType_STRING;
        while (--_size > 0) {
          *_dst++ = pgm_read_byte(_src++); *_dst++ = 0;
        }
        return _len;
      }
      if (_type == bDescriptorType_HID_REPORT ||
          _type == bDescriptorType_HID_PHYSICAL) {
        _pgmem += 2;
        _size -= 2;
      }
      memcpy_P(_buffer, _pgmem, _size);
      return _size;
    }
    else if (_type == bDescriptorType_STRING && _hint != 0) {
      _size = 3;
    }
    _pgmem += _size;
  } while (!(_type == bDescriptorType_STRING && _hint == 0));
  return 0;
}

__attribute__((__noinline__))
__attribute__((section(".text")))
size_t get_descriptor_copy (uint8_t* _buffer, const uint16_t _wValue, const uint8_t* _descriptor_set) {
  uint8_t _request = _wValue >> 8;
  uint8_t _index = _wValue;
  uint8_t _hint, _type, _size;
  uint8_t* _pgmem = (uint8_t*)_descriptor_set;
  do {
    _hint = *_pgmem++;
    _size = _pgmem[0];
    _type = _pgmem[1];
    if ((_size + 2) < 4) break;
    if (_type == bDescriptorType_CONFIGURATION ||
        _type == bDescriptorType_OTHER_SPEED_CONFIGURATION) {
      _size = _pgmem[2];
    }
    if (_type == _request && _hint == _index) {
      if (_type == bDescriptorType_STRING && _index != 0) {
        uint8_t* _src = (uint8_t*)_descriptor_set + _pgmem[2];
        uint8_t* _dst = _buffer;
        uint8_t _len = ++_size << 1;
        *_dst++ = _len; *_dst++ = bDescriptorType_STRING;
        while (--_size > 0) {
          *_dst++ = *_src++; *_dst++ = 0;
        }
        return _len;
      }
      if (_type == bDescriptorType_HID_REPORT ||
          _type == bDescriptorType_HID_PHYSICAL) {
        _pgmem += 2;
        _size -= 2;
      }
      memcpy(_buffer, _pgmem, _size);
      return _size;
    }
    else if (_type == bDescriptorType_STRING && _hint != 0) {
      _size = 3;
    }
    _pgmem += _size;
  } while (!(_type == bDescriptorType_STRING && _hint == 0));
  return 0;
}

__attribute__((__noinline__))
__attribute__((section(".text")))
size_t get_auto_generate_serial_number_string (uint8_t* _buffer, uint8_t _check_eeprom) {
  /*
    * NOTE: Serial Number String Generation
    *
    * If the 4 bytes from offset 4 of the EEPROM are anything other
    * than 0xFFFF:FFFF, use them, otherwise generate a 32-bit random
    * serial number using CRC32 from the factory information.
    */
  uint32_t _sn[2];
  _sn[0] = 0xCEA1031A;
  _sn[1] = ((User_EEP_t*)EEPROM_START)->dwSerialNumber;
  if (_check_eeprom && !(_sn[1] + 1)) _sn[1] = crc32((uint8_t*)SIGNATURES_START, 32);
  uint8_t* _p = (uint8_t*)&_sn;
  *_buffer++ = *_p++;
  *_buffer++ = *_p++;
  for (uint8_t _i = 0; _i < 6; _i++) {
    uint8_t _c = *_p++;
    *_buffer++ = btoh(_c >> 4); *_buffer++ = 0;
    *_buffer++ = btoh(_c     ); *_buffer++ = 0;
  }
  return 26;
}


// end of code
