/**
 * @file Blink_09_PINALL.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-31
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

PORT_t* port_list[] = {
  &PORTA
#ifdef PORTB
, &PORTB
#endif
#ifdef PORTC
, &PORTC
#endif
#ifdef PORTD
, &PORTD
#endif
#ifdef PORTE
, &PORTE
#endif
#ifdef PORTF
, &PORTF
#endif
#ifdef PORTG
, &PORTG
#endif
};

uint8_t _rxd;
void setup (void) {
  if      (pgm_get_far_address(Serial) == pgm_get_far_address(Serial0A)) { _rxd = PIN_USART0_RXD     ; }
  #if defined(Serial0B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial0B)) { _rxd = PIN_USART0_ALT1_RXD; }
  #endif
  #if defined(Serial0C)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial0C)) { _rxd = PIN_USART0_ALT2_RXD; }
  #endif
  #if defined(Serial0D)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial0D)) { _rxd = PIN_USART0_ALT3_RXD; }
  #endif
  #if defined(Serial0E)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial0E)) { _rxd = PIN_USART0_ALT4_RXD; }
  #endif
  #if defined(Serial1A)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial1A)) { _rxd = PIN_USART1_RXD     ; }
  #endif
  #if defined(Serial1B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial1B)) { _rxd = PIN_USART1_ALT1_RXD; }
  #endif
  #if defined(Serial1C)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial1C)) { _rxd = PIN_USART1_ALT2_RXD; }
  #endif
  #if defined(Serial2A)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial2A)) { _rxd = PIN_USART2_RXD     ; }
  #endif
  #if defined(Serial2B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial2B)) { _rxd = PIN_USART2_ALT1_RXD; }
  #endif
  #if defined(Serial2C)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial2C)) { _rxd = PIN_USART2_ALT2_RXD; }
  #endif
  #if defined(Serial3A)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial3A)) { _rxd = PIN_USART3_RXD     ; }
  #endif
  #if defined(Serial3B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial3B)) { _rxd = PIN_USART3_ALT1_RXD; }
  #endif
  #if defined(Serial4A)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial4A)) { _rxd = PIN_USART4_RXD     ; }
  #endif
  #if defined(Serial4B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial4B)) { _rxd = PIN_USART4_ALT1_RXD; }
  #endif
  #if defined(Serial5A)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial5A)) { _rxd = PIN_USART5_RXD     ; }
  #endif
  #if defined(Serial5B)
  else if (pgm_get_far_address(Serial) == pgm_get_far_address(Serial5B)) { _rxd = PIN_USART5_ALT1_RXD; }
  #endif
}

void loop (void) {
  for (auto port : port_list) {
    uint8_t port_num = (((uint16_t)port) >> 5) & 7;
    for (uint8_t pin_num = 0; pin_num < 8; pin_num++) {
      uint8_t pin_check = (port_num << 5) + 0x10 + pin_num;
      if (pin_check == _rxd) continue;
      port->DIRSET = _BV(pin_num);
      port->OUTTGL = _BV(pin_num);
      delay_millis(10);
    }
  }
}

// end of code
