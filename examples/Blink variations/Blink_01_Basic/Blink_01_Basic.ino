/**
 * @file Blink_01_Basic.ino
 * @author askn (K.Sato) multix.jp
 * @brief Blink using delay timer sketch code
 * @version 0.1
 * @date 2022-09-29
 *
 * @copyright Copyright (c) 2022
 *
 */
void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
}

void loop (void) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  delay_millis(1000);
}

// end of code
