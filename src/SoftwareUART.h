/**
 * @file SoftwareUART.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <avr/io.h>
#include <api/delay_busywait.h>
#include <api/Print.h>

#define __CLOCK_TICKS   2
#define __CLOCK_CORRECT 4

class SoftwareUART_Class : public Print {
private:
  PORT_t* _PORT;
  volatile uint8_t* _PIN_CTRL;
  uint8_t _TXD, _RXD;
  uint16_t _bit_loop, _bit_busy, _bit_start;

public:
  SoftwareUART_Class (uint8_t PIN_TXD, uint8_t PIN_RXD);

  using Print::write; // pull in write(str) and write(buf, size) from Print
  SoftwareUART_Class& begin (uint32_t baudrate);
  SoftwareUART_Class& beginRxDriveOnly (uint32_t baudrate);
  SoftwareUART_Class& beginTxDriveOnly (uint32_t baudrate);
  void end (void);
  void flush (void) {}
  uint16_t is_baud (void) { return _bit_loop; }
  size_t write (const uint8_t _c);
  size_t readBytes (void* _buffer, size_t _limit, char _terminate = 0, uint8_t _swevent = 0);
};

// end of code
