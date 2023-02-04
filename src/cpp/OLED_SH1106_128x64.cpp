/**
 * @file OLED_SH1106_128x64.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2023-02-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <OLED_SH1106_128x64.h>
#include <api/delay_busywait.h>

const uint8_t oled_init[] PROGMEM = {
    0x00  // command stream
  , 0xAE  // Display >0=Off
  , 0xA4  // Set Entire display : >0=Off
  , 0xA6  // Set Display : >0=Normal, >1=Reverse
  , 0xC8  // Set Common output scan direction : 0=normal, >8=reverse (vertical flip)
  , 0xA1  // Set Segment re-map : 0=Normal, >1=Flip

  , 0xD5, 0x50  // Set Display Divide Ratio/Oscillator Frequency Mode (POR 0x50)
  , 0xD9, 0x22  // Set Pre-charge Period Mode (POR 0x22)
  , 0xDA, 0x12  // Set Common Pads Hardware Configuration Mode (POR 0x12)
  , 0xDB, 0x35  // Set VCOM Deselect Level Mode (POR 0x35)

  , 0xA8, OLED_H * OLED_L - 1   // Set multiplex ration
  , 0xD3, OLED_O                // Set display offset : top column
  , 0x40                        // Set display start line : offset column
};

OLED_SH1106_Class& OLED_SH1106_Class::disable (void) {
  TWIC
    .start(OLED_SH1106_ADDR, 2)
    .send(0x00)
    .send(0xAE)
  ;
  return *this;
}

void OLED_SH1106_Class::set_page (uint8_t _page, uint8_t _len) {
  TWIC
    .start(OLED_SH1106_ADDR)
    .send(0x00)
    .send(0x00)         /* Set Column Low */
    .send(0x10)         /* Set Column High */
    .send(0xB0 + _page) /* Set Page */
    .start(OLED_SH1106_ADDR, ++_len)
    .send(0x40)         /* RAM Write Mode */
  ;
}

OLED_SH1106_Class& OLED_SH1106_Class::clear (bool console_mode) {
  TWIC
    .start(OLED_SH1106_ADDR)
    .send(P(oled_init), sizeof(oled_init))
  ;
  uint8_t _x = OLED_PAGE;
  do {
    uint8_t _y = OLED_COLM;
    set_page(--_x, _y);
    do { TWIC.write(0x00); } while (--_y);
  } while (_x);
  _console_mode = console_mode;
  _cx = _cy = 0;
  _cz = OLED_V;
  return setContrast(_ct);
}

OLED_SH1106_Class& OLED_SH1106_Class::setFlip (bool flip_mode) {
  TWIC
    .start(OLED_SH1106_ADDR, 3)
    .send(0x00)
    .send(flip_mode ? 0xC0 : 0xC8)
    .send(flip_mode ? 0xA0 : 0xA1)
  ;
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::setRevesible (bool reverse_mode) {
  TWIC
    .start(OLED_SH1106_ADDR, 2)
    .send(0x00)
    .send(reverse_mode ? 0xA7 : 0xA6)
  ;
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::setContrast (uint8_t _contrast) {
  _ct = _contrast;
  TWIC
    .start(OLED_SH1106_ADDR, 4)
    .send(0x00)
    .send(0x81)
    .send(_contrast)
    .send(0xAF)     // Display >1=On
  ;
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::setScroll (uint8_t _offset) {
  TWIC
    .start(OLED_SH1106_ADDR, 2)
    .send(0x00)
    .send(0x40 + (_offset & 63))
  ;
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::setPosition (uint8_t _x, uint8_t _y) {
  _cx = _x;
  if (_console_mode) _cy = (_cz + _y) & (OLED_H - 1);
  else _cy = _y;
  return *this;
}

const uint8_t test_data[] PROGMEM = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

OLED_SH1106_Class& OLED_SH1106_Class::drawTestPattern (uint8_t _offset) {
  uint8_t _x = OLED_PAGE;
  do {
    uint8_t _y = OLED_COLM;
    set_page(--_x, _y);
    do {
      TWIC.write(pgm_read_byte(&test_data[(_offset + --_y) & 7]));
    } while (_y);
  } while (_x);
  TWIC.stop();
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::drawChar (uint8_t _ax, uint8_t _ay, uint8_t _c) {
  uint8_t* _p;
  _c -= 32;
  if (_c >= 240) _p = ExtraRAMSet + ((uint16_t)(_c - 240) << 4);
  else if (_c >= 96 && ExtraROMSet != nullptr) _p = ExtraROMSet + ((uint16_t)(_c - 96) << 4);
  else _p = (uint8_t*)&FontSet[(uint16_t)_c << 4];
  uint8_t _j = 2;
  uint8_t _k = 2 + (_ax << 3);
  uint8_t _m = _ay << 1;
  do {
    uint8_t _i = 8;
    TWIC
      .start(OLED_SH1106_ADDR)
      .send(0x00)
      .send(_k & 15).send(0x10 + ((_k >> 4) & 15))  // colum
      .send(0xB0 + ((_m++) & 7))
      .start(OLED_SH1106_ADDR)
      .send(0x40)
    ;
    if (_c >= 240) {
      do TWIC.write(*_p++); while (--_i);
    }
    else {
      do TWIC.write(pgm_read_byte(_p++)); while (--_i);
    }
  } while (--_j);
  TWIC.stop();
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::newLine (uint8_t _ay, bool _scroll) {
  uint8_t _d = 2;
  do {
    uint8_t _c = OLED_COLM;
    TWIC
      .start(OLED_SH1106_ADDR)
      .send(0x00)
      .send(0x00).send(0x10)
      .send(0xB0 + (((_ay << 1) + (2 - _d)) & 7))
      .start(OLED_SH1106_ADDR)
      .send(0x40)
    ;
    do TWIC.write(0x00); while (--_c);
    if (_scroll) setScroll(((_ay + OLED_V + 1) << 4) - (_d << 3) + 8);
  } while (--_d);
  TWIC.stop();
  return *this;
}

OLED_SH1106_Class& OLED_SH1106_Class::setExtFont (uint8_t* _ram_table, uint8_t _c, uint8_t _logic) {
  uint8_t *_p = (uint8_t*)&FontSet[(uint16_t)(_c - 32) << 4];
  uint8_t _i = OLED_L;
  do {
    uint8_t _d = (_c < 32) ? 0xff : pgm_read_byte(_p++);
    uint8_t _e = *_ram_table;
    if (_logic == OLED_XOR) _e ^= _d;
    else if (_logic == OLED_AND) _e &= _d;
    else if (_logic == OLED_OR) _e |= _d;
    else _e = _d;
    *(_ram_table++) = _e;
  } while (--_i);
  return *this;
}

size_t OLED_SH1106_Class::write (const uint8_t _c) {
  if (_c == '\b') { _cx -= _cx ? 1 : 0; }
  else if (_c == '\r') { _cx = 0; }
  else if (_c == '\f') { clear(_console_mode); }
  else if (_c == '\n') {
    if (++_cy != _cz) { newLine(_cy, false); }
    else { _cz++; newLine(_cy, _console_mode); }
  }
  else {
    if (_cx >= OLED_W) {
      _cx = 0;
      if (++_cy != _cz) { newLine(_cy, false); }
      else { _cz++; newLine(_cy, _console_mode); }
    }
    if (_c == '\t') _cx = (_cx & ~3) + 4;
    else drawChar(_cx++, _cy, _c);
  }
  return 1;
}

// end of code
