/**
 * @file Blink_09_PFM.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-05
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#if defined(AVR_TINYAVR_8)
  #include "Blink_06_PFM_8P.cpp"

#elif defined(AVR_AVRDD14) || defined(AVR_AVRDD20) || defined(AVR_AVRDU14)
  #include "Blink_06_PFM_DD14.cpp"

#elif defined(AVR_TINYAVR_14) || defined(AVR_TINYAVR_20) || defined(AVR_TINYAVR_24)
  #include "Blink_06_PFM_14P.cpp"

#elif defined(AVR_AVREB)
  #include "Blink_06_PFM_AVREB.cpp"

#elif defined(AVR_MEGAAVR) || defined(AVR_MODERNAVR)
  #include "Blink_06_PFM_28P.cpp"

#else
  #error This MCU model not supported

#endif

// end of code
