/**
 * @file SerialUSB_Class.cpp
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
#include <avr/interrupt.h>

#include "../SerialUSB.h"

extern "C" {
  SerialUSB_Class SerialUSB;  /* Instance allocation */
} /* extern "C" */

ISR(USB0_BUSEVENT_vect) {
  USB_NAMESPACE::handling_bus_events();
}

ISR(USB0_TRNCOMPL_vect) {
  USB_NAMESPACE::handling_transaction_events();
}

#endif
// end of code
