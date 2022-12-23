/**
 * @file benchmark.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace Benchmark {
  void init (void);
  uint32_t test (void(*_testfunc)(void));
}

// end of code
