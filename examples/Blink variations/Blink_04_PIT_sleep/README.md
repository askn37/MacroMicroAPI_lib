# Blink_04_PIT_sleep.ino

このサンプルスケッチは以下について記述している；

- PIT（Periodic Interrupt Timer）による周期割込＋CPU連続休止

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
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
}

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
}

void sleep_ms (uint32_t _ms) {
  _ms = (_ms + (_ms >> 6) + (_ms >> 7) + 1024) >> 10;
  RTC_PITINTCTRL = RTC_PI_bm;
  do power_down(); while (--_ms > 0);
  RTC_PITINTCTRL = 0;
}

void loop (void) {
  digitalWrite(LED_BUILTIN, TOGGLE);
  sleep_ms(1000);
}
```

## 解説

これは[Blink_03_PIT.ino](../Blink_03_PIT/README.md)の変種で
指定時間のあいだ CPUを休止状態に置く
```sleep_ms()```を実装したものだ。

PITは最長1Hz（またはOSC1K選択時に1/32Hz）までしか割込周期を設定できないため
それ以上の時間を連続して休止状態に置くには割込発生回数を数え上げなければならない。

```c
_ms = (_ms + (_ms >> 6) + (_ms >> 7) + 1024) >> 10;
RTC_PITINTCTRL = RTC_PI_bm;
do power_down(); while (--_ms > 0);
RTC_PITINTCTRL = 0;
```

ここでは単純に 1Hz周期の割込発生回数をカウントしようとしているが、
当然他の割込での休止解除もカウントされることに注意されたい。
目的外の割込（外部ピン割込）は事前に無効としておくか、
ループの脱出条件を修正しなければならない。
本件を汎用ライブラリとしていないのはこの理由による。

> 例えば外部スイッチからの割込入力で直ちに休止ループから抜けだしたい場合など。

休止時間の指定は直接秒数指定でも良いのだが、ここでは
```delay()```など他の関数との直交性に配慮してミリ秒指定とした。
ミリ秒から実効周期数への換算はシフト演算（近似値）として
除算の使用を避けている。
補正は 0を指定した場合も最低1回は休止回数をカウントするためだ。

なお初回の割込発生は 1周期未満の不定長である。
よって```1000```を指定した場合の休止長は```0~1000ms```の範囲だ。
この誤差を減らすには周期割込回数を増やして時間粒度を下げる。
例えば 4Hz周期割込とすれば誤差範囲 250ms以下に向上する。
ただし引き換えに省電力効果はそのぶん低下するのでケースバイケースだ。

```diff
- RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
+ RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC8192_gc;

- _ms = (_ms + (_ms >> 6) + (_ms >> 7) + 1024) >> 10;
+ _ms = (_ms + (_ms >> 6) + (_ms >> 7) + 256) >> 8;
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
