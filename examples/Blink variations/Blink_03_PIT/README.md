# Blink_03_PIT.ino

このサンプルスケッチは以下について記述している；

- PIT（Periodic Interrupt Timer）による周期割込

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> reduceAVR世代では動作しない

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}

void loop (void) {
  sleep_cpu();
}
```

## 解説

megaAVR世代以降には僅かな設定で
8192Hz〜1Hz または
256Hz〜1/32Hz の周期割込を発生させる
PIT周辺機能が組み込まれている。
これはまたCPUを深い休止状態から目覚めさせることができる特徴を持つ。
IoTセンサー等で省電力周期的間欠駆動をさせたい場合には便利な機能だ。

### PITの設定（標準動作）

PITを単純に1秒周期で動かしたい場合には、
32768分周比を与え、割込と計時機能を有効化するだけで良い。

```c
/* RTCは CPUクロックと非同期に動作しているので設定可能な同期状態を待つ */
loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);

/* PIT割込有効化 */
RTC_PITINTCTRL = RTC_PI_bm;

/* 32768分周比とともにPIT機能有効化 */
RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
```

PITは RTC周辺機能の一部で、OSC32K発振器からの
32768Hzクロックを供給されて動作する。
分周比には4から32768の、2の冪乗数を指定できるので
32768を指示すれば1秒周期での割込を発生させることが出来る。
割込処理の中で LED_BUILTIN出力を反転すれば、0.5Hzの信号を発生させられる。

```c
/* PIT割込 */
ISR(RTC_PIT_vect) {
  /* 割込フラグクリア */
  RTC_PITINTFLAGS = RTC_PI_bm;

  /* LED_BUILTIN出力反転 */
  digitalWrite(LED_BUILTIN, TOGGLE);
}
```

割込内でなすべき処理は完結するので、主ループにおいては何もする必要はない。
なので```SLEEP_MODE_PWR_DOWN```指定でCPUを深い休止状態に置いてもよく
割込発生で目覚めるたびに再度休止させれば結果的に何もしないようにできる。

この例では分周比を変更することで、以下の周波数の周期割込を実現できる。

|分周比|基準32K|基準1K|
|----:|-----:|----:|
|    4|8192Hz|256Hz|
|    8|4096Hz|128Hz|
|   16|2048Hz| 64Hz|
|   32|1024Hz| 32Hz|
|   64| 512Hz| 16Hz|
|  128| 256Hz|  8Hz|
|  256| 128Hz|  4Hz|
|  512|  64Hz|  2Hz|
| 1024|  32Hz|  1Hz|
| 2048|  16Hz|1/2Hz|
| 4096|   8Hz|1/4Hz|
| 8192|   4Hz|1/8Hz|
|16384|   2Hz|1/16Hz|
|32768|   1Hz|1/32Hz|

### 長周期設定

最長周期が1Hzでは短くて使いづらい場合、
基準周波数を 1024Hzに下げて得られる結果を32倍にすることが出来る。
これにはRTC周辺機能に供給される基準周波数の選択を変える。

```c
/* modernAVRの場合 */
RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;

/* それ以前は表記が異なる */
// RTC_CLKSEL = RTC_CLKSEL_INT1K_gc;
```

しかしこれはRTC周辺機能の全体設定が変わってしまうので
使用目的によってはあまり好ましいことではないだろう。
代替案については
[Blink_04_PIT_sleep.ino](../Blink_04_PIT_sleep/README.md)
を参照のこと。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
