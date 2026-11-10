/**
 * @file USB/CDC/cpp/descripter.cpp
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <avr/io.h>
#if defined(__cplusplus) && defined(USB0)
#include <avr/pgmspace.h>   /* PROGMEM memcpy_P */
#include "../../Descriptor.h"
#include "../../Utils.h"
#include "../../CDC.h"
#include "../constants.h"

namespace USB_NAMESPACE {

  /*
   * Configuration callback Methods
   */

  /* Sets the specified device/configuration descriptor for the endpoint. */
  /* It then returns the payload size. */
  WEAK size_t cb_get_descriptor_data (uint8_t* _buffer, uint16_t _index) {
    if (_index == 0x03FF) {
      return get_auto_generate_serial_number_string(_buffer, false);
    }
    size_t _size = get_descriptor_copy_P(_buffer, _index, USB_DescriptorSet);
    if (_size == 0 && (_index >> 8) == bDescriptorType_STRING) {
      *_buffer++ = _size = 2;
      *_buffer++ = bDescriptorType_STRING;
    }
    return _size;
  }

} /* namespace USB_CDC */

#endif
// end of code
