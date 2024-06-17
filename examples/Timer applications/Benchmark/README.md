# Benchmark.ino

このサンプルスケッチは以下について記述している；

- TCB計数器2組による32bit幅計数捕獲

## 対象AVR

- megaAVR世代
  - tinyAVR-2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

> megaAVR-0、tinyAVR-0/1系統は対応しない。

## 32bit幅計数捕獲器

__tinyAVR-2__ 以降の新世代 AVRでは、隣接する`TCB`計数器2組を連結して
32bit計数捕獲する「カスケード」機能が追加された。
それ以前の品種でも 32bit計時自体はできたのだが、
初段には必ず`TCA`計数器が必要だったり、
計時には使えるが正確な計数捕獲ができない（下位計数を取りこぼす）などの制約があった。

この 32bit計数捕獲ができて何が嬉しいかというと；

- 24MHz駆動時に`F_CPU`等速で 178.956秒までの連続計時が可能になった。
  - 16bit幅では等速だと 2.73ミリ秒で計数器が溢れた。
  - 計測時間を拡大するには精度を犠牲にして分周器を通すしか対策できなかった。
- 周波数計測のダイナミックレンジが、0.006Hz〜24MHzに拡大した。
  - 従来だと 366Hz〜24MHzが限界だった。

この 32bit計数捕獲を設定するには次のようにする；

- 下位 16bit計数器と 上位 16bit計数器 に同じ動作モードを指示する。
- 上位 16bit計数器に「カスケード」利用を指示する。
- 下位 16bit計数器の溢れ事象を`EVSYS`経由で
上位 16bit計数器の計数入力に接続する。
- 計数捕獲モードでは、捕獲事象を`EVSYS`経由で
両計数器に同時に送るようにする。

例えば、計数入力元を`CLK_PER`（`F_CPU`）とし、
計数捕獲モードに設定するには次のようになる。
`EVSYS_CHANNEL4`が計数捕獲事象、
`EVSYS_CHANNEL5`が`TCB0`の溢れ事象を
`TCB1`の計数入力に送る事象チャネルとして使っている。

```c
/* TCB0とTCB1の捕獲事象に接続 */
EVSYS_CHANNEL4 = EVSYS_CHANNEL4_OFF_gc;
EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL4_gc;
EVSYS_USERTCB1CAPT = EVSYS_USER_CHANNEL4_gc;

/* ch5=TCB0溢れ*/
/* TCB1の計数事象に接続 */
EVSYS_CHANNEL5 = EVSYS_CHANNEL5_TCB0_OVF_gc;
EVSYS_USERTCB1COUNT = EVSYS_USER_CHANNEL5_gc;

/* TCB1は計数捕獲周波数測定動作かつ連結上位 */
TCB1_EVCTRL = TCB_CAPTEI_bm;
TCB1_CTRLB = TCB_CNTMODE_FRQ_gc;
TCB1_CTRLA = TCB_RUNSTDBY_bm | TCB_CASCADE_bm | TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm;

/* TCB0は計数捕獲周波数測定動作かつ連結下位 */
/* CLK元は主クロック（F_CPU）*/
TCB0_EVCTRL = TCB_CAPTEI_bm;
TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
TCB0_CTRLA = TCB_RUNSTDBY_bm | TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
```

計数捕獲モード（`TCB_CNTMODE_FRQ_gc`）は計数事象を受け取る度に
現在の計数値`CNT`を`CCMP`レジスタに複写し`CNT`はゼロクリアする動作を繰り返す。
なので定期的に計数捕獲事象を送ればその時点の 32bit計数値が正しく得られる仕組みだ。

```c
EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm; /* 計数捕獲（計数リセット） */
/* 時間経過 */
EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm; /* 次の計数捕獲 */
uint32_t _count = TCB0_CCMP           /* 下位16bit */
     + ((uint32_t)TCB1_CCMP) << 16;   /* 上位16bit */
/* 得られる _count は時間経過の間の CLK_PER カウント数 */
```

以上を基本設定として、
外部端子からの入力を`EVSYS`経由で捕獲事象に送るようにすると
2回の入力信号の間隔を`CLK_PER`精度で計数捕獲できるので
外部端子に接続された信号元の「パルス幅時間測定」になる。

また計数入力に外部端子からの入力を選び（これも`EVSYS`で設定できる）
捕獲事象を別の計時器で例えば1秒間隔で送ると、
捕獲された計数値は1秒間の信号数そのものであるから「周波数直接測定」になる。

## ベンチマーク測定

32bit計数捕獲器の簡単な実演例として
適当な実行プログラムの実行時間を計測するベンチマークを紹介する。

```c
#include "benchmark.h"

/* 被テスト関数 : 例えばdelay */
__attribute__((noinline))
void delay_test (void) {
  delay_millis(500);
}

void setup (void) {
  Serial.begin(CONSOLE_BAUD);
  Benchmark::init(); /* 計数器初期化 */

  uint32_t clk_per;
  clk_per = Benchmark::test( test_function );

  /* CPU実行サイクル数が戻るので実時間に換算する */
  float per_us = clk_per / (F_CPU / 1000000.0);
  Serial.println(clk_per, DEC, 12); /* CLK_PER */
  Serial.println(per_us, 3, 10);  /* per microseconds */
}
```

`"benchmark.h"`で定義される
`Benchmark::test`は関数ポインタで与えられた`void`関数を実行し、
その実行CPUサイクル数を返す。
これを`F_CPU`から求まる時間サイクル数で除すれば
経過実行時間を得ることができる。
マイクロ秒当たりのサイクル数で除すれば、結果はマイクロ秒単位だ。

> `"benchmark.h"`は
`EVSYS_CHANNEL4`、
`EVSYS_CHANNEL5`、
`TCB0`、
`TCB1`の4つの周辺機能資源を専有することに注意されたい。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
