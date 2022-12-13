/**
 * @file TaskChanger.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <TaskChanger.h>
#include <avr/io.h>

volatile char task1_stack[64];
volatile char task2_stack[64];
#if (INTERNAL_SRAM_SIZE >= 512)
volatile char task3_stack[64];
#endif

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
  TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);
  TaskChanger::attach_task_2nd(task2_stack, sizeof(task2_stack), &task2);
#if (INTERNAL_SRAM_SIZE >= 512)
  TaskChanger::attach_task_3rd(task3_stack, sizeof(task3_stack), &task3);
#endif
}

void loop (void) {
	digitalWriteMacro(LED_BUILTIN, TOGGLE);
  Serial.print(F("0:")).println(TimeoutTimer::millis_left(), DEC);
  Serial.flush();
  delay(1000);
}

void task1 (void) {
  while (true) {
    Serial.print(F("1:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000);
  }
}

void task2 (void) {
  while (true) {
    Serial.print(F("2:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000);
  }
}

void task3 (void) {
  while (true) {
    Serial.print(F("3:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000);
  }
}

// end of code
