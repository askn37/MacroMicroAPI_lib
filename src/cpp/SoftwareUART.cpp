/**
 * @file SoftwareUART.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include "../SoftwareUART.h"
#include <util/atomic.h>

SoftwareUART_Class::SoftwareUART_Class (uint8_t PIN_TXD, uint8_t PIN_RXD) {
  _TXD = _BV(PIN_TXD & 7);
  _RXD = _BV(PIN_RXD & 7);
  _PORT = (PORT_t*)((uint16_t)&PORTA + (PIN_RXD & 0xE0));
  _PIN_CTRL = (volatile uint8_t*)(&(_PORT->PIN0CTRL) + (PIN_RXD & 7));
}

SoftwareUART_Class& SoftwareUART_Class::begin (uint32_t baudrate) {
  _bit_loop  =  ((F_CPU                 / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_busy  =  ((F_CPU * 10            / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_start = (((F_CPU + (F_CPU >> 1)) / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  *_PIN_CTRL = PORT_PULLUPEN_bm;
  _PORT->OUTSET = _TXD;
  _PORT->DIRSET = _TXD;
  _PORT->DIRCLR = _RXD;
  return *this;
}

SoftwareUART_Class& SoftwareUART_Class::beginRxDriveOnly (uint32_t baudrate) {
  _bit_loop  =  ((F_CPU                 / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_busy  =  ((F_CPU * 10            / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_start = (((F_CPU + (F_CPU >> 1)) / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  *_PIN_CTRL = PORT_PULLUPEN_bm;
  _PORT->DIRCLR = _RXD;
  return *this;
}

SoftwareUART_Class& SoftwareUART_Class::beginTxDriveOnly (uint32_t baudrate) {
  _bit_loop  =  ((F_CPU                 / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_busy  =  ((F_CPU * 10            / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _bit_start = (((F_CPU + (F_CPU >> 1)) / baudrate) >> __CLOCK_TICKS) - __CLOCK_CORRECT;
  _PORT->OUTSET = _TXD;
  _PORT->DIRSET = _TXD;
  return *this;
}

void SoftwareUART_Class::end (void) {
  *_PIN_CTRL = 0;
  _PORT->DIRCLR = _TXD;
}

/* 文字単位書き出し */
size_t SoftwareUART_Class::write (const uint8_t _c) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  uint8_t _count = 9, _bits = _c ^ ~0;
    _PORT->OUTCLR = _TXD;
    do {
      _busy_loop_4_16(_bit_loop);
      if (_bits & 1) _PORT->OUTCLR = _TXD;
      else           _PORT->OUTSET = _TXD;
      _bits >>= 1;
    } while (--_count);
  }
  _busy_loop_4_16(_bit_loop);
  return 1;
}

/* 文字単位入力はない */

/* 連続文字採取 */
/* 指定長読むか指定文字で終了（既定値はヌル文字） */
size_t SoftwareUART_Class::readBytes (void* _buffer, size_t _limit, char _terminate, uint8_t _swevent) {
  size_t _length = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    do {
      /* 最初のスタートマークを待つ */
      /* 10bit以上連続するスペース（アイドル）であるなら終了 */
      uint16_t _busy = _bit_busy;
      while (_PORT->IN & _RXD) {
        if (--_busy == 0) return _length;
      }
      /* 任意) EVSYS_CHANNEL0-7をトリガーする */
    #if defined(EVSYS_SWEVENTA)
      EVSYS_SWEVENTA = _swevent;
    #elif defined(EVSYS_STROBE)
      EVSYS_STROBE = _swevent;
    #elif defined(EVSYS_ASYNCSTROBE)
      EVSYS_ASYNCSTROBE = _swevent;
    #endif
      _swevent = 0;
      /* b0 採取時間まで待つ */
      _busy_loop_4_16(_bit_start);
      uint8_t _count = 8, _bits = 0;
      do {
        _bits >>= 1;
        /* b0-b8 採取 */
        if (_PORT->IN & _RXD) _bits |= 0x80;
        _busy_loop_4_16(_bit_loop);
      } while (--_count);
      /* ストップスペース */
      if (_PORT->IN & _RXD) {
        /* データ獲得 */
        ((uint8_t*)_buffer)[_length++] = _bits;
        /* 任意) 終端文字と一致するなら終了 */
        if (_terminate == _bits) break;
      }
      /* フレームエラーは読み捨て */
      /* 10bitマーク（ブレーク）は脱出 */
      else if (_bits == 0) break;
    } while (_length < _limit);
  }
  return _length;
}

// end of code
