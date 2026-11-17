/**
 * @file USB_Descriptor_Build.ino
 * @author askn (K.Sato) multix.jp
 * @brief USB Descriptor Sample
 * @version 0.1
 * @date 2026-11-10
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#include <avr/io.h>
#include <avr/pgmspace.h>   /* memcpy_P PROGMEM */
#include <string.h>         /* memcpy */
#include "USB_Utils.h"

const uint16_t _wValues[] = {
  0x0100, 0x0200, 0x0201, 0x2200, 0x0600, 0x0700,
  0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0300
};

extern "C" {
  extern const uint8_t* USB_Descriptor_CDC_EDBG_Set;
  extern const size_t   USB_Descriptor_CDC_EDBG_SetSize;
  extern const uint8_t  USB_Descriptor_CDC_EDBG_SetSection;

  extern const uint8_t* USB_Descriptor_CDC_Set;
  extern const size_t   USB_Descriptor_CDC_SetSize;
  extern const uint8_t  USB_Descriptor_CDC_SetSection;

  extern const uint8_t* USB_Descriptor_EDBG_Set;
  extern const size_t   USB_Descriptor_EDBG_SetSize;
  extern const uint8_t  USB_Descriptor_EDBG_SetSection;
}

const uint8_t* USB_Descriptor_Set[] = {
  USB_Descriptor_CDC_EDBG_Set
, USB_Descriptor_CDC_Set
, USB_Descriptor_EDBG_Set
};
const size_t USB_Descriptor_SetSize[] = {
  USB_Descriptor_CDC_EDBG_SetSize
, USB_Descriptor_CDC_SetSize
, USB_Descriptor_EDBG_SetSize
};
const uint8_t USB_Descriptor_SetSection[] = {
  USB_Descriptor_CDC_EDBG_SetSection
, USB_Descriptor_CDC_SetSection
, USB_Descriptor_EDBG_SetSection
};

const static char sepltr[] = "\r\n        ";

void stringView (uint8_t* _buffer) {
  uint8_t _length = *_buffer++;
  uint8_t _request = *_buffer++;
  if (_request != 3) return;
  --_length;
  while (--_length > 0) {
    uint8_t c = *_buffer++;
    if (c < 0x20 || c > 0x7E) c = '_';
    Serial.write(c);
  }
  Serial.ln();
}

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  delay(800);
  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("\r<startup>"));
  Serial.println(F(_AVR_IOXXX_H_)).ln();

  uint8_t _buff[128];
  size_t _size;

  for (uint8_t j = 0; j < sizeof(USB_Descriptor_SetSection); j++) {
    const uint8_t* _Set = USB_Descriptor_Set[j];
    size_t _SetSize = USB_Descriptor_SetSize[j];
    uint8_t _SetSection = USB_Descriptor_SetSection[j];

    Serial.printf(F("%04X,%02X "),_Set, _SetSize);
    if (_SetSection == 0)
      Serial.printHex(P(_Set), _SetSize, ':', 16, sepltr).ln();
    else
      Serial.printHex(_Set, _SetSize, ':', 16, sepltr).ln();

    for (uint8_t i = 0; i < sizeof(_wValues) / sizeof(uint16_t); i++) {
      if (_SetSection == 0)
        _size = get_descriptor_copy_P(_buff, _wValues[i], _Set);
      else
        _size = get_descriptor_copy(_buff, _wValues[i], _Set);
      Serial.printf(F("%04X,%02X "), _wValues[i], _size);
      Serial.printHex(_buff, _size, ':', 16, sepltr).ln();
      if (_size > 0) stringView(_buff);
    }
    Serial.ln();
  }

  Serial.flush();
}

void loop (void) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  delay_millis(1000);
}

// end of code
