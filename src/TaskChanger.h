/**
 * @file TaskChanger.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1.2
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once
#include <stdint.h>
#include "TimeoutTimer.h"

#define __ASSERT_USE_STDERR
#include <assert.h>

struct TaskChanger_work_t {
  char* stack[4];
  uint8_t current;
};

namespace TaskChanger {
  extern volatile TaskChanger_work_t __worker;
  void yield (void);
  void attach_task (uint8_t __task_index, volatile char __local_stack[], size_t __local_stack_size, void (*__start_task)());
  inline void attach_task_1st (volatile char __local_stack[], size_t __local_stack_size, void (*__start_task)()) {
    attach_task(1, __local_stack, __local_stack_size, __start_task);
  }
  inline void attach_task_2nd (volatile char __local_stack[], size_t __local_stack_size, void (*__start_task)()) {
    attach_task(2, __local_stack, __local_stack_size, __start_task);
  }
  inline void attach_task_3rd (volatile char __local_stack[], size_t __local_stack_size, void (*__start_task)()) {
    attach_task(3, __local_stack, __local_stack_size, __start_task);
  }
  bool joined_task (uint8_t __task_index);
  inline bool joined_task_1st (void) {
    return joined_task(1);
  }
  inline bool joined_task_2nd (void) {
    return joined_task(2);
  }
  inline bool joined_task_3rd (void) {
    return joined_task(3);
  }
  void detach_task (uint8_t __task_index);
  inline void detach_task_1st (void) {
    return detach_task(1);
  }
  inline void detach_task_2nd (void) {
    return detach_task(2);
  }
  inline void detach_task_3rd (void) {
    return detach_task(3);
  }
  size_t remaining_stack (uint8_t __task_index, volatile char __local_stack[]);
  inline size_t remaining_stack_1st (volatile char __local_stack[]) {
    return remaining_stack(1, __local_stack);
  }
  inline size_t remaining_stack_2nd (volatile char __local_stack[]) {
    return remaining_stack(2, __local_stack);
  }
  inline size_t remaining_stack_3rd (volatile char __local_stack[]) {
    return remaining_stack(3, __local_stack);
  }
}

// end of code
