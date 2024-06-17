/**
 * @file benchmark.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once

namespace Benchmark {
  void init (void);
  uint32_t test (void(*_testfunc)(void));
}

// end of code
