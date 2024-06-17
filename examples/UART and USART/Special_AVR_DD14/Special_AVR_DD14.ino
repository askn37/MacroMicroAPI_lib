/**
 * @file Serial_Echo_14pin.ino
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-07
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

// USART1 to Custom Logic Connection
//
// (TXD:PC0) not implemented!
//  RXD:PC1 -->
//  TXD:PC3 <-- CCL_LUT1OUT <-- internal USART1 TXD signal

void setup (void) {
  /* Serial1A initialize */
  Serial1A.begin(CONSOLE_BAUD);

  /* Catch INSEL1 bit to output */
  CCL_TRUTH1 = CCL_TRUTH_2_bm;

  /* Input select USART1 TX signal  */
  CCL_LUT1CTRLB = CCL_INSEL1_USART1_gc;

  /* LUT1 table enable and LUT1OUT generate enable */
  CCL_LUT1CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;

  /* CCL start */
  CCL_CTRLA = CCL_ENABLE_bm;

  /* Output LUT1OUT transfer PIN_PC3 */
  pinModeMacro(PIN_PC3, OUTPUT);
}

void loop (void) {
  /* Echoback */
  if (Serial1A.available()) Serial1A.write(Serial.read());
}

// end of code

