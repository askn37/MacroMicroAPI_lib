# Blink_02_delay.ino

このサンプルスケッチは以下について記述している；

- TCB計時機で"Blink"実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

なお`F_CPU`は65536000L未満かつ1000で割り切れなければならない。\
従って`OSC32K`を主クロック元に選ぶと正しい時間精度は得られない。

`OSC32K`等の超低速駆動用 delay は
[[RTC計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_08_delay_SLOW)
を参照のこと。

> `<timer_delay.h>`は選択された
`TCBn`計時器別の実装ヘッダファイルを読み込むラッパーになっている。
実際の応用では必要な実装を選んでコピーすると良い。

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
/* 使用する計数器周辺機能を選択 */
#define USE_TIMER_DELAY HAVE_TCB0
#include "timer_delay.h"

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  Timer::init();
}
void loop (void) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  Timer::delay(1000);
}
```

## 解説

`<timer_delay.h>`は
Arduino互換APIの`delay()`と同等の実装を行う。

その動作原理は次のようなものだ。

1. 1ms毎に1進むミリ秒計数を`TCBn`計時器と割込で実装する。
2. `delay()`ではそれを参照して指定時間量の遅延ループを実行する。

### TCB計時器の設定

計時器に`TCB0`計時器周辺機能を選択した場合、
その初期設定は次のようになる。

```c
TCB0.CCMP = F_CPU / 1000 - 1;                       // 比較一致レジスタに1ミリ秒相当の計数値を書く
TCB0.INTCTRL = TCB_CAPT_bm;                         // 比較一致割込を有効にする。
TCB0.CTRLB = TCB_CNTMODE_INT_gc;                    // 計時器に継続周回動作を指示
TCB0.CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV1_gc;    // 1カウント=1CPUサイクルとして計時器を起動
```

この設定で次の割込ハンドラが 1ミリ秒毎に周期的に呼び出される。

```c
static volatile uint32_t _timer_millis;             // ミリ秒カウンタ変数を確保

ISR(TCB0_INT_vector) {                              // 割込発生
  TCB0.INTFLAGS = TCB_CAPT_bm;                      // 割込フラグをクリア
  _timer_millis++;                                  // ミリ秒カウンタを1進める
}
```

ミリ秒カウンタには結果として計時器初期化時を元期とする
経過ミリ秒が格納されるので、
これを読み出して調べることで任意処理間の経過時間を測ることができる。

> カウント変数にはコンパイル最適化によるコード欠損を避けるために`volatile`修飾子が必要とされる。
なおこの割込ハンドラは割込周期に最低でも 57 CPUサイクルを消費する。

### ミリ秒取得

ただしミリ秒カウンタは32bit幅=4レジスタ幅を持つので、
読み出し途中に割込で更新されると正しいカウント数が得られない。
そのため`millis()`関数は一時的に全体割込を禁止して、これを読む。

```c
inline uint32_t millis (void) {
  uint32_t _ms;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {               // 全体割込一時禁止
    _ms = _timer_millis;                            // ミリ秒カウンタを読む
  }
  return _ms;
}
```

### マイクロ秒取得

一方`TCB0`計時機が持つ計数値は
1ミリ秒の間に実行されたCPUサイクル数を直接保持している。
そこでこれを読み出してCPUサイクル数からマイクロ秒に換算し、
ミリ秒を1000倍したものと足し合わせることで経過マイクロ秒数を得ることができる。

```c
/* マイクロ秒取得：しかしこれは正しく動作しない */
uint32_t micros (void) {
  uint32_t _ms;
  uint16_t _tc;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {               // 全体割込一時禁止
    _ms = _timer_millis;                            // ミリ秒カウンタを読む
    _tc = USE_TIMER_TIMER.CNT;                      // 計時器カウンタを読む
  }
  return (_ms * 1000) + (((uint32_t)_tc * 1000000UL) / F_CPU);
}
```

しかしこの関数は、このままでは正しく動作しない。
割込を禁止して計時器カウンタを読んでいる間にも
割込発生条件が満たされ、計時器カウンタが
`MAX->0`に巻き戻される瞬間を見過ごすと問題が生じるため
割込禁止解除前に割込フラグがセットされていないか調べ、
計時器カウンタの読み直しとミリ秒カウンタの繰り上げ訂正を行う。

```c
/* 取得方法の修正 */
ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  _ms = _timer_millis;
  _tc = USE_TIMER_TIMER.CNT;
  if (bit_is_set(USE_TIMER_TIMER.INTFLAGS, TCB_CAPT_bp)) { // 割込フラグが立っていたら
    _tc = USE_TIMER_TIMER.CNT;                      // 計時器カウンタを読みなおす
    _ms++;                                          // ミリ秒カウンタを繰り上げる
  }
}
```

もう一つの問題は計時器カウンタ値からマイクロ秒を導く計算式が
除算を含むがために演算コストが重いことだ。
これは数百CPUサイクルを消費する。
それがそのまま計時誤差となるので
主クロックが低い場合は数百マイクロ秒の狂いとなって現れる。

$$ us = \frac{COUNT \times 10^6}{CLK_{PER}} $$

`US <-- COUNT * 1000000L / F_CPU`

乗算はハードウェア乗算器があるためそこまで極端に演算コストは高くない。
問題は除算を使うことなので、これを「除数の逆数との乗算とシフト演算」に置き換える。
このとき逆数は当然小数点以下を含むから、シフト演算で固定小数点位置を動かす事を考える。
除算代わりに16ビット右シフトするなら、逆数は16ビット左シフトした65536倍して
事前計算（プリプロセス任せに）にしておけばよい。

$$ us = \frac{ ( \frac{COUNT \times 10^6 \times 2^{16}}{CLK_{PER}} ) }{2^{16}} $$

`US <-- ((uint32_t)COUNT * (uint16_t)(65536000000UL / F_CPU)) >> 16`

> この例は被除数の精度が16bit幅以下に限られるが、
24bit幅精度を残すなら 256倍して8ビット右シフトする等と変形できる。

ところで`F_CPU`（CLK_PER）が特定の値`pow(2, n) * 1000000L`に一致
する場合はこの演算をもっと単純化できる。
例えば`F_CPU=16MHz`の場合の計数値は
`F_CPU/1000==16000`未満となるので
単に4ビット右シフトすれば望むマイクロ秒に一致させることができるから、
右シフトだけで計算ができる
`F_CPU`についてはプリプロセス処理で例外化する。

```c
#if (F_CPU == 32000000L)
  /* top 0b_0111_1101_1100_0000 == 0x7dc0 == 32000    */
  /* div 0b.0000_1000_0000_0000 == 0x0800 == 65536/32 */
  /*      0.---->5                           right 5  */
  return (_ms * 1000) + (_tc >> 5);

#elif (F_CPU == 16000000L)
  /* top 0b_0011_1110_1000_0000 == 0x3e80 == 16000    */
  /* div 0b.0001_0000_0000_0000 == 0x1000 == 65536/16 */
  /*      0.-->4                             right 4  */
  return (_ms * 1000) + (_tc >> 4);

...

#elif (F_CPU == 1000000L)
  /* top 0b_0000_0011_1110_1000 == 0x03e8 == 1000     */
  /* div 0b.0000_0000_0000_0000 == 0x0000 == 65536/1  */
  /*      0.                                 right 0  */
  return (_ms * 1000) + (_tc);
```

他のケースも同様にシフト演算の羅列に置き換えられるのだが
`10`や`20`の除算は2進数で見ると循環小数になり、
有効桁数全桁足し合わせる場合はハードウェア乗算器に比べると手数で不利だ。

> ミリ秒計算で既に乗算関数が結合されているのは確実だから再利用するのはやぶさかでない。\
> シフト演算に置き換えても AVR-GCC 組込乗除算より高度な最適化ができる状況は稀だろう

### 遅延ループの実装

`delay()`関数の実装は幾つか考えられるが、例えば次のようなコードが書ける。

```c
void delay (uint32_t _ms) {
  uint32_t start_us = micros();
  while (_ms > 0) {
    yield();
    while (_ms > 0 && (micros() - start_us) >= 1000) {
      _ms--;
      start_us += 1000;
    }
  }
}
```

`micros()`をポーリングして
元期から 1000以上増加する毎に`_ms`を減数し、
それが0になったら処理を終える。
これだけを盲目的ループとしてもよいのだが、
1ミリ秒経過毎に`yield()`関数を呼ぶ外側のループを追加することで
待機中に他の処理をできるようにも書いてある。

> この処理中に`micros()`が巻き戻って`start_us`より小さくなっても
内側のループは完了するので暴走はしない。
逆に`micros()`の返却値値幅が毎回`1000`を超えると破綻してしまう。
これについては[[RTC計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_08_delay_SLOW)が詳しい。\
\
> `yield()`は`week`修飾宣言された空関数なので、任意で再宣言できる。
その応用は[[協調的マルチタスク実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Timer%20applications/TaskChanger)が詳しい。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
