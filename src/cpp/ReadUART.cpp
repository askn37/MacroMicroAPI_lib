/**
 * @file ReadUART_Class.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2023-12-20
 *
 * @copyright Copyright (c) 2023 askn37 at github.com
 *
 */
#if !defined(__AVR_TINY__)
#include "../ReadUART.h"
#include <avr/pgmspace.h>
#include <util/atomic.h>

ReadUART_Class& ReadUART_Class::initiate (const uint16_t _baudrate) {
  uint8_t _usart_ctrl_b = USART_RXEN_bm | USART_TXEN_bm;
  uint8_t _baud2x = 0;
  if (_baudrate <= 63) {
    _baud2x = 1;
    _usart_ctrl_b = USART_RXEN_bm | USART_TXEN_bm | USART_RXMODE_CLK2X_gc;
  }
  const UART_portmux_t* _mux = portmux;
  register8_t *_portmux = (register8_t*)pgm_read_ptr(&_mux->portmux_reg);
  *_portmux = (*_portmux & pgm_read_byte(&_mux->portmux_mask))
                         | pgm_read_byte(&_mux->portmux_set);
  PORT_t *_port = pgm_read_ptr(&_mux->port_reg);
  _port->DIRSET = pgm_read_byte(&_mux->tx_pin);
  _port->DIRCLR = pgm_read_byte(&_mux->rx_pin);
  (*(register8_t*)pgm_read_ptr(&_mux->rx_pin_ctrl)) = PORT_PULLUPEN_bm
                                                    | PORT_ISC_INTDISABLE_gc;
  USART_t* _usart = usart;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _usart->BAUD = (_baudrate << _baud2x);
    _usart->CTRLA = USART_RXCIE_bm;
    _usart->CTRLC = USART_CHSIZE_8BIT_gc
                  | USART_PMODE_DISABLED_gc
                  | USART_CMODE_ASYNCHRONOUS_gc
                  | USART_SBMODE_1BIT_gc;
    _usart->CTRLB = _usart_ctrl_b;
  }
  return *this;
}

void ReadUART_Class::end (void) {
  flush();
  USART_t* _usart = usart;
  const UART_portmux_t* _mux = portmux;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _usart->CTRLA = 0;
    _usart->CTRLB = 0;
    (*(PORT_t*)pgm_read_ptr(&_mux->port_reg)).DIRCLR = pgm_read_byte(&_mux->tx_pin);
    (*(register8_t*)pgm_read_ptr(&_mux->rx_pin_ctrl)) = PORT_ISC_INPUT_DISABLE_gc;
  }
}

void ReadUART_Class::interrupt (void) {
  USART_t* _usart = usart;
  while (bit_is_set(_usart->STATUS, USART_RXCIF_bp)) {
    _status = _usart->RXDATAH;
    uint8_t _data = _usart->RXDATAL;
    if (!(_status & (USART_FERR_bp | USART_BUFOVF_bm | USART_PERR_bm ))) {
      size_t __top = _top;
      _work[__top] = _data;
      _last = _data;
      if (++__top >= _size) __top = 0;
      #ifdef ENABLED_WRAP_READUART
      if (__top == _end) {
        if (++_end >= _size) _end = 0;
      }
      #endif
      _top = __top;
    }
  }
}

size_t ReadUART_Class::write (const uint8_t _c) {
  loop_until_bit_is_set(usart->STATUS, USART_DREIF_bp);
  usart->STATUS = USART_TXCIF_bm;
  usart->TXDATAL = _c;
  return 1;
}

int ReadUART_Class::read (void) {
  int _c = ~0;
  if (_top != _end) {
    _c = _work[_end];
    if (++_end >= _size) _end = 0;
  }
  return _c;
}

int ReadUART_Class::peek (void) {
  int _c = ~0;
  if (_top != _end) _c = _work[_end];
  return _c;
}

size_t ReadUART_Class::available (void) {
  size_t _s = _top - _end;
  if (_s < 0) _s += _size;
  return _s;
}

bool ReadUART_Class::find (const uint8_t _c) {
  size_t __top = _top;
  size_t __end = _end;
  while (__top != __end) {
    if (_c == _work[__end++]) return true;
    if (__end >= _size) __end = 0;
  }
  return false;
}

size_t ReadUART_Class::readBytes (void* _buffer, size_t _limit, char _terminate, uint8_t _swevent) {
  size_t _length = 0;
  do {
    uint16_t _busy = usart->BAUD;
    while (!available() && bit_is_clear(usart->STATUS, USART_RXCIF_bp)) {
      if (--_busy == 0) return _length;
    }
  #if defined(EVSYS_SWEVENTA)
    EVSYS_SWEVENTA = _swevent;
  #elif defined(EVSYS_STROBE)
    EVSYS_STROBE = _swevent;
  #elif defined(EVSYS_ASYNCSTROBE)
    EVSYS_ASYNCSTROBE = _swevent;
  #endif
    _swevent = 0;
    if (bit_is_clear(SREG, 7)) {
      /* Executes only when interrupts are disabled */
      interrupt();
    }
    int _c = read();
    if (_c >= 0) {
      ((uint8_t*)_buffer)[_length++] = (uint8_t)_c;
      if (_terminate == (uint8_t)_c) break;
    }
    else break;
  } while (_length < _limit);
  return _length;
}

#endif
// end of code
