/**
 * @file Benchmark.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
 * ベンチマーク計測
 *
 * 渡されたテスト関数の実効クロック数を計測する
 * テスト関数は合計3回呼ばれる
 * テスト関数は最大32bitクロック数まで計測可能（24Mhzで178.95秒）
 *
 * 使用リソース
 *  TCB0,TCB1のふたつのタイマー計数機を連結して32bit幅クロックカウンタとして使う
 *  EVSYSのch4とch5を使う
 */

#include "benchmark.h"

/* 被テスト関数 : 例えば割り込みベクタ */
static volatile uint32_t _timer_counts;
ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  _timer_counts++;
}

/* 被テスト関数 : 例えばdelay関数 */
__attribute__((noinline))
void delay_test (void) {
  delay_millis(500);
}

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
}

void loop (void) {
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.print(F("Call test: "));

  /* 被テスト関数を割り込み禁止で実行・計測する */
  uint32_t clock;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // clock = Benchmark::test(RTC_PIT_vect);
    clock = Benchmark::test(delay_test);
  }
  /* CPU実行サイクル数が戻るので実時間に換算する */
  float clk_per = clock / (F_CPU / 1000000.0);

  /* テスト計測結果表示 */
  Serial.print(clock, DEC, 9).print(F(" clk "));
  Serial.print(clk_per, 3, 12).print(F(" us "));
  Serial.print(clk_per / 1000.0, 3, 9).println(F(" ms"));

  /* 終了 : Enterで再試行 */
  Serial.println(F("<Hit ENTER Retry>"));
  Serial.flush();
  while (Serial.read() != '\n');
}

// end of code
