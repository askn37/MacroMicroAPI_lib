# Blink_08_delay_SLOW.ino

このサンプルスケッチは以下について記述している；

- RTC計時機で"Blink"実演（低速駆動用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Dx系統

これは
[[TCB計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_02_delay)
の変種で`RTC`を用い、
`TCB`計時器に変えて
`OSC32K`内蔵発振器による`F_CPU=32768`駆動に対応した版である。
最低動作速度`F_CPU=4096`以上で機能する。

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
#include "timer_delay_SLOW.h"

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

`<timer_delay_SLOW.h>`は
Arduino互換APIの`delay()`とおおむね同等の実装を行う。
原典である
[[TCB計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_02_delay)
との違いは計時器資源として`RTC`周辺機能を使うことだ。
これによって従前の最低対応速度 1Mhzより遅い CPUクロック設定に対応している。
これの基本動作速度は`CLK_RTC=32768`なので
最小時間粒度は`30.5us`となる。

ただし`CLK_PER`(F_CPU)周辺機能クロックは
`CLK_RTC`より最低4倍早くなければならない制約があるため、
`F_CPU=131072`未満ではクロック供給元に
`OSC1K`内蔵発振器を選択し
`CLK_RTC=1024`とする。
この場合の時間粒度は`976.5us`だ。
そしてこれ以上動作速度を下げる設定はできないため、
周辺機能クロック`CLK_PER`の下限は`F_CPU=4096`である。

## 割込実装

RTC計時器の設定は以下となる。

```c
#if (F_CPU < 131072L)
RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;
#endif
RTC_PER = UINT16_MAX;
RTC_INTCTRL = RTC_OVF_bm;
RTC_CTRLA = RTC_RTCEN_bm;
```

```c
static volatile uint32_t _timer_seconds;

ISR(RTC_CNT_vect) {
  RTC_INTFLAGS = RTC_OVF_bm;
  _timer_seconds++;
}
```

計数レジスタ`RTC_CNT`は16bit幅一杯を使うので、
溢れ割込発生周期は標準 2秒間隔、低速 64秒間隔となる。

> この割込ハンドラは必要最小限の記述しかないがそれでも
1回の割込で 57クロックを消費する。

この`RTC_CNT`値から経過ミリ秒、経過マイクロ秒を求めるには
何れも乗除算が必要だ。

```c
uint32_t millis (void) {
  uint32_t _sc, _tc;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _sc = _timer_seconds;
    _tc = RTC_CNT;
    if (bit_is_set(RTC_INTFLAGS, RTC_OVF_bp)) {
      _tc = RTC_CNT;
      _sc++;
    }
  }

#if (F_CPU < 131072L)
  _tc = (_tc * 64000) >> 16;
  return _sc * 64000 + _tc;
#else
  _tc = (_tc * 2000) >> 16;
  return _sc * 2000 + _tc;
#endif

}
```

```c
uint32_t micros (void) {
  uint32_t _sc, _tc;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _sc = _timer_seconds;
    _tc = RTC_CNT;
    if (bit_is_set(RTC_INTFLAGS, RTC_OVF_bp)) {
      _tc = RTC_CNT;
      _sc++;
    }
  }

#if (F_CPU < 131072L)
  _tc = _tc * 976 + (_tc >> 1);
  return _sc * 64000000 + _tc;
#else
  _tc = _tc * 30 + (_tc >> 1);
  return _sc * 2000000 + _tc;
#endif

}
```

元の`CLK_PER`が低い場合、これらの関数の実行速度も自ずから低い。
数百クロックを要するので
`OSC32K`駆動では数ミリ〜数十ミリ秒のオーダーになってしまう。

## 遅延ループ

遅延ループ`Timer::delay()`の実装も
[[TCB計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_02_delay)
にはない特別な配慮が必要だ。

```diff
 void delay (uint32_t _ms) {
   uint32_t start_us = micros(), step_us;
   while (_ms > 0) {
     yield();
-    while (_ms > 0 && (micros() - start_us) >= 1000) {
-      _ms--;
-      start_us += 1000;
-    }
+    while (_ms > 0 && (step_us = micros() - start_us) >= 1000) {
+      do {
+        _ms--;
+        start_us += 1000;
+        step_us -= 1000;
+      } while (_ms > 0 && step_us >= 1000);
+    }
   }
 }
```

`Timer::micros()`の呼び出しを含む第2ループの実行に
1ミリ秒以上を要する場合、従前の記述では脱出困難となってしまう。
その事象を避けるため内側のループに追加の脱出条件を加えなければならない。

## 実用性

`OSC32K`による低速駆動においては、割込事象処理は非常に実行コストが高い。
必要最小限のカウント変数処理でも数十サイクル以上を見込まねばならないので、
相対的にその負担が増すからだ。
`F_CPU=512`動作クロック設定での超低速駆動ともなるとなおさらで、
割込の使用はほぼ障害対応用途に限られなければならない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
