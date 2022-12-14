# Blink_05_PIT_PWM.ino

このサンプルスケッチは以下について記述している；

- PIT（Periodic Interrupt Timer）によるPWM出力

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-2系統
- modernAVR世代
  - AVR Dx系統

すべてをハードウェア周辺機能だけで全制御しCPU本体の処理ループや割込は使用しない。\
LED出力は PIN_PA7==EVOUTA_ALT1であると仮定。
28pin以上の品種で動作する。

> tinyAVR-0/1世代は EVSYS制御に互換性がないためここでは取り上げない。

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

動作を解りやすく示すために第2のLED（PIN_PA6）も使用する。

```c
void setup (void) {
  PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;
  EVSYS_CHANNEL0 = EVSYS_CHANNEL0_RTC_PIT_DIV2048_gc;
  EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;
  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;
  RTC_PITCTRLA = RTC_PITEN_bm;

  pinMode(PIN_PA6, OUTPUT); // LED2
}

void loop (void) {
  delay(1000);
  digitalWriteMacro(PIN_PA6, TOGGLE);
}
```

## 事象システム経由の直接LED駆動

2個のLEDをそれぞれ異なる方法で駆動する。
一方は主ループで時間遅延による普通のLチカ、
もう一方は PITの周期出力を事象システムを介して直接Lチカする。
それぞれは完全に独立しており、互いに影響を及ぼさないので
異なる周期でLチカさせることが出来る。

PIT用の LED（LED_BUILTIN）は`PIN_PA7`を示し、
かつこれは`EVOUTA`信号出力ポートであるものとする。

### RTC_PITの設定

していることはふたつしかない。
RTC周辺機能ブロックへのクロック配給に 1kHz内蔵発振器を選択し、
RTC_PIT周辺機能を有効化するだけである。
このクロックはDuty比50:50を持つ。

```c
/* クロック元に OSC1K を選択 */
RTC_CLKSEL = RTC_CLKSEL_OSC1K_gc;

/* 周辺機能有効化 */
RTC_PITCTRLA = RTC_PITEN_bm;
```

### EVSYSの設定

`EVSYS`事象システム周辺機能では、
RTC_PITから送られた`OSC1K`クロックを2048分周して
`EVOUTA`へ分配する。
Duty比は維持されるから`EVOUTA`は
`1/1024/2048==0.5Hz`周期でON/OFFを繰り返すことになる。

```c
/* RTC_PITから 2048分周したクロックを受給 */
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_RTC_PIT_DIV2048_gc;

/* EVOUTA にクロックを配給 */
EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;
```

> tinyAVR-0/1ではEVSYS記述方法が異なる。\
> tinyAVR-0ではEVSYSにPIT分周クロックを入力する機能がない。

### PORTMUXの設定

`PORTMUX`ポート多重化器では`EVOUTA`信号の出力先を`LED_BUILTIN`に一致する`PIN_PA7`に変更している。
`pinMode()`を使う必要はなく`EVSYS_USER*`を設定した時点で該当ポートは出力方向に切り替えられる。

```c
/* EVOUTAを既定の PIN_PA2ではなく代替の PIN_PA7 に変更 */
PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;
```

> tinyAVR-0/1ではEVOUT設定方法が異なる。

## 完全ハードウェア周辺機能駆動

以上の設定で`PIN_PA7`に繋がるLEDは 0.5Hz周期で Lチカを始めるが、
その継続動作に CPUは一切関わらない。
CPUが休止状態で停止しようとも、無限ループしていようとも、
0.5Hz周期の Lチカはお構いなく動作を続ける。

例示の第2LEDは メイン処理ループ内で`delay()`を使って点滅させるが、
その待機時間は CPUサイクル数を基準にしている。
そのため互いに関わりなく独立した計時器で動作している LEDの
点滅周期がゆっくりとずれていく様子が観察できるだろう。

より高度なハードウェア周辺機能独立制御については
[Blink_06_PFM.ino](../Blink_06_PFM/)
でも触れているので参照すると良い。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)