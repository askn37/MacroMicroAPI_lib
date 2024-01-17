/**
 * @file Blink_09_TaskChanger.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2024-01-17
 *
 * @copyright Copyright (c) 2024 askn37 at multix.jp
 *
 */
#include <TaskChanger.h>

volatile char blink_stack[64];
void yield (void) { TaskChanger::yield(); }

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  // Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  TaskChanger::attach_task_1st(blink_stack, sizeof(blink_stack), &blink);
  // TaskChanger::attach_task_2nd(blink_stack, sizeof(blink_stack), &blink);
  // TaskChanger::attach_task_3rd(blink_stack, sizeof(blink_stack), &blink);
}

static void blink (void) {
  while (true) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay(1000);
  }
}

void loop (void) {
  yield();
}

// end of code
