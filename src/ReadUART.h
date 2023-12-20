/**
 * @file ReadUART_Class.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2023-12-20
 *
 * @copyright Copyright (c) 2023 askn37 at github.com
 *
 */
#pragma once
#include <avr/io.h>
#if defined(__cplusplus) && !defined(__AVR_TINY__)
#include <api/Portmux.h>
#include <api/Print.h>

/*** class definition ***/

class ReadUART_Class : public Print {
private:
  const UART_portmux_t* portmux;
  uint8_t *_work;
  size_t _size, _top, _end;
  uint8_t _status, _last;

public:
  USART_t* usart;

  inline ReadUART_Class (USART_t* _usart, const UART_portmux_t* _portmux, void *_buffer_ptr, size_t _buffer_size) :
    usart(_usart), portmux(_portmux), _work((uint8_t*)_buffer_ptr), _size(_buffer_size) {}

  inline ReadUART_Class& begin (const uint32_t _baudrate) {
    return initiate((uint16_t)(((F_CPU / (_baudrate >> 3)) + 1) / 2));
  }

  ReadUART_Class & initiate (const uint16_t _baudrate);
  void end (void);

  size_t write (const uint8_t _c);
  using Print::write; // pull in write(str) and write(buf, size) from Print

  int read (void);
  int peek (void);
  size_t available (void);

  size_t availableForWrite (void) {
    return bit_is_set(usart->STATUS, USART_DREIF_bp) ? 1 : 0;
  }

  void flush (void) {
    loop_until_bit_is_set(usart->STATUS, USART_TXCIF_bp);
  }

  void interrupt (void);
  inline uint16_t is_baud (void) { return usart->BAUD; }
  inline uint8_t status (void) { return _status; }
  inline uint8_t last (void) { return _last; }
  bool find (const uint8_t _c = '\n');
  size_t readBytes (void* _buffer, size_t _limit, char _terminate = 0, uint8_t _swevent = 0);
  explicit operator bool (void) { return true; }
};

/*** initialization definition ***/

#if RAMSIZE >= 1024
#define ReadUART_BUFFSIZE 64
#else
#define ReadUART_BUFFSIZE 32
#endif

#ifdef HAVE_USART0
  #ifdef _PORTMUX_USART0A
    #define Settings_SerialR0A(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0a = _PORTMUX_USART0A; \
      ReadUART_Class SerialR0A {&USART0, &_portmux_usart0a, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0A; \
      ISR (USART0_RXC_vect) { SerialR0A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0B
    #define Settings_SerialR0B(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0b = _PORTMUX_USART0B; \
      ReadUART_Class SerialR0B {&USART0, &_portmux_usart0b, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0B; \
      ISR (USART0_RXC_vect) { SerialR0B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0C
    #define Settings_SerialR0C(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0c = _PORTMUX_USART0C; \
      ReadUART_Class SerialR0C {&USART0, &_portmux_usart0c, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0C; \
      ISR (USART0_RXC_vect) { SerialR0C.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0D
    #define Settings_SerialR0D(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0d = _PORTMUX_USART0D; \
      ReadUART_Class SerialR0D {&USART0, &_portmux_usart0d, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0D; \
      ISR (USART0_RXC_vect) { SerialR0D.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0E
    #define Settings_SerialR0E(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0e = _PORTMUX_USART0E; \
      ReadUART_Class SerialR0E {&USART0, &_portmux_usart0e, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0E; \
      ISR (USART0_RXC_vect) { SerialR0E.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0F
    #define Settings_SerialR0F(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0f = _PORTMUX_USART0F; \
      ReadUART_Class SerialR0F {&USART0, &_portmux_usart0f, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0F; \
      ISR (USART0_RXC_vect) { SerialR0F.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART0G
    #define Settings_SerialR0G(SIZE) \
      static uint8_t __readuart0[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart0g = _PORTMUX_USART0g; \
      ReadUART_Class SerialR0G {&USART0, &_portmux_usart0g, &__readuart0, sizeof(__readuart0)}; \
      extern ReadUART_Class SerialR0G; \
      ISR (USART0_RXC_vect) { SerialR0G.interrupt(); }
  #endif
#endif

#ifdef HAVE_USART1
  #ifdef _PORTMUX_USART1A
    #define Settings_SerialR1A(SIZE) \
      static uint8_t __readuart1[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart1a = _PORTMUX_USART1A; \
      ReadUART_Class SerialR1A {&USART1, &_portmux_usart1a, &__readuart1, sizeof(__readuart1)}; \
      extern ReadUART_Class SerialR1A; \
      ISR (USART1_RXC_vect) { SerialR1A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART1B
    #define Settings_SerialR1B(SIZE) \
      static uint8_t __readuart1[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart1b = _PORTMUX_USART1B; \
      ReadUART_Class SerialR1B {&USART1, &_portmux_usart1b, &__readuart1, sizeof(__readuart1)}; \
      extern ReadUART_Class SerialR1B; \
      ISR (USART1_RXC_vect) { SerialR1B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART1C
    #define Settings_SerialR1C(SIZE) \
      static uint8_t __readuart1[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart1c = _PORTMUX_USART1C; \
      ReadUART_Class SerialR1C {&USART1, &_portmux_usart1c, &__readuart1, sizeof(__readuart1)}; \
      extern ReadUART_Class SerialR1C; \
      ISR (USART1_RXC_vect) { SerialR1C.interrupt(); }
  #endif
#endif

#ifdef HAVE_USART2
  #ifdef _PORTMUX_USART2A
    #define Settings_SerialR2A(SIZE) \
      static uint8_t __readuart2[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart2a = _PORTMUX_USART2A; \
      ReadUART_Class SerialR2A {&USART2, &_portmux_usart2a, &__readuart2, sizeof(__readuart2)}; \
      extern ReadUART_Class SerialR2A; \
      ISR (USART2_RXC_vect) { SerialR2A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART2B
    #define Settings_SerialR2B(SIZE) \
      static uint8_t __readuart2[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart2b = _PORTMUX_USART2B; \
      ReadUART_Class SerialR2B {&USART2, &_portmux_usart2b, &__readuart2, sizeof(__readuart2)}; \
      extern ReadUART_Class SerialR2B; \
      ISR (USART2_RXC_vect) { SerialR2B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART2C
    #define Settings_SerialR2C(SIZE) \
      static uint8_t __readuart2[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart2c = _PORTMUX_USART2C; \
      ReadUART_Class SerialR2C {&USART2, &_portmux_usart2c, &__readuart2, sizeof(__readuart2)}; \
      extern ReadUART_Class SerialR2C; \
      ISR (USART2_RXC_vect) { SerialR2C.interrupt(); }
  #endif
#endif

#ifdef HAVE_USART3
  #ifdef _PORTMUX_USART3A
    #define Settings_SerialR3A(SIZE) \
      static uint8_t __readuart3[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart3a = _PORTMUX_USART3A; \
      ReadUART_Class SerialR3A {&USART3, &_portmux_usart3a, &__readuart3, sizeof(__readuart3)}; \
      extern ReadUART_Class SerialR3A; \
      ISR (USART3_RXC_vect) { SerialR3A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART3B
    #define Settings_SerialR3B(SIZE) \
      static uint8_t __readuart3[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart3b = _PORTMUX_USART3B; \
      ReadUART_Class SerialR3B {&USART3, &_portmux_usart3b, &__readuart3, sizeof(__readuart3)}; \
      extern ReadUART_Class SerialR3B; \
      ISR (USART3_RXC_vect) { SerialR3B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART3C
    #define Settings_SerialR3C(SIZE) \
      static uint8_t __readuart3[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart3c = _PORTMUX_USART3C; \
      ReadUART_Class SerialR3C {&USART3, &_portmux_usart3c, &__readuart3, sizeof(__readuart3)}; \
      extern ReadUART_Class SerialR3C; \
      ISR (USART3_RXC_vect) { SerialR3C.interrupt(); }
  #endif
#endif

#ifdef HAVE_USART4
  #ifdef _PORTMUX_USART4A
    #define Settings_SerialR4A(SIZE) \
      static uint8_t __readuart4[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart4a = _PORTMUX_USART4A; \
      ReadUART_Class SerialR4A {&USART4, &_portmux_usart4a, &__readuart4, sizeof(__readuart4)}; \
      extern ReadUART_Class SerialR4A; \
      ISR (USART4_RXC_vect) { SerialR4A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART4B
    #define Settings_SerialR4B(SIZE) \
      static uint8_t __readuart4[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart4b = _PORTMUX_USART4B; \
      ReadUART_Class SerialR4B {&USART4, &_portmux_usart4b, &__readuart4, sizeof(__readuart4)}; \
      extern ReadUART_Class SerialR4B; \
      ISR (USART4_RXC_vect) { SerialR4B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART4C
    #define Settings_SerialR4C(SIZE) \
      static uint8_t __readuart4[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart4c = _PORTMUX_USART4C; \
      ReadUART_Class SerialR2C {&USART4, &_portmux_usart4c, &__readuart4, sizeof(__readuart4)}; \
      extern ReadUART_Class SerialR4C; \
      ISR (USART4_RXC_vect) { SerialR4C.interrupt(); }
  #endif
#endif

#ifdef HAVE_USART5
  #ifdef _PORTMUX_USART5A
    #define Settings_SerialR5A(SIZE) \
      static uint8_t __readuart5[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart5a = _PORTMUX_USART5A; \
      ReadUART_Class SerialR5A {&USART5, &_portmux_usart5a, &__readuart5, sizeof(__readuart5)}; \
      extern ReadUART_Class SerialR5A; \
      ISR (USART5_RXC_vect) { SerialR5A.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART5B
    #define Settings_SerialR5B(SIZE) \
      static uint8_t __readuart5[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart5b = _PORTMUX_USART5B; \
      ReadUART_Class SerialR5B {&USART5, &_portmux_usart5b, &__readuart5, sizeof(__readuart5)}; \
      extern ReadUART_Class SerialR5B; \
      ISR (USART5_RXC_vect) { SerialR5B.interrupt(); }
  #endif
  #ifdef _PORTMUX_USART5C
    #define Settings_SerialR5C(SIZE) \
      static uint8_t __readuart5[SIZE]; \
      const UART_portmux_t PROGMEM _portmux_usart5c = _PORTMUX_USART5C; \
      ReadUART_Class SerialR2C {&USART5, &_portmux_usart5c, &__readuart5, sizeof(__readuart5)}; \
      extern ReadUART_Class SerialR5C; \
      ISR (USART5_RXC_vect) { SerialR5C.interrupt(); }
  #endif
#endif

#endif

// end of header
