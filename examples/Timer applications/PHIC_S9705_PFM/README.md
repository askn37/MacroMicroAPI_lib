# PHIC_S9705_PFM.ino

このサンプルスケッチは以下について記述している；

- フォトICで得た照度でLEDの明滅速度を変化させる実演

## 対象AVR

- megaAVR世代
  - tinyAVR-2系統
- modernAVR世代
  - AVR Dx系統

> megaAVR-0、tinyAVR-0/1系統は対応しない。

## 明るさでLEDの明滅速度を変化させる

このサンプルスケッチは
[[フォトICで照度を計測表示するシリアルプロッタ実演（S9705用）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Timer%20applications/PHIC_S9705)
の変種で、照度計測結果でLEDの明滅速度をPFM信号で実演する。
LED駆動部分は
[[PITでPFM信号生成して"Blink"実演（ホタル点滅）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_07_PIT_PFM)
と原理は同じだ。単なる「Lチカ」ではなく「ホタル明滅」である。

## 解説

照度計測でしていることは
[前回とおなじ](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Timer%20applications/PHIC_S9705)
である。
`EVSYS`と`TCB0`で`S9705`の出力周波数を計測できるようにする。

```c
/* TCB0の計数を捕獲する */
/* DIV2048 == 16 Hz 周期 */
EVSYS_CHANNEL2 = EVSYS_CHANNEL2_RTC_PIT_DIV2048_gc;
EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;

/* S9705からの入力を TCB0で計数する */
EVSYS_CHANNEL3 = EVSYS_CHANNEL3_PORTD_PIN2_gc;
EVSYS_USERTCB0COUNT = EVSYS_USER_CHANNEL3_gc;

/* TCB0を計数捕獲モードに設定 */
TCB0_EVCTRL = TCB_CAPTEI_bm;     /* 事象捕獲有効 */
TCB0_CTRLB = TCB_CNTMODE_FRQ_gc; /* 計数捕獲モード */
TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc; /* 事象から計数入力 */
```

そしてもう一つの`TCB1`で可変周期のPWM信号を生成する。
ここでは`TCB`が持つ「PWM出力モード」は使用しない。
それは周期一定＋デューティサイクル変化のPWM信号生成なので
今回の目的に合わないからだ。
設定は「比較一致割込モード」（`TCB_CNTMODE_INT`）と
`TCB_CAPT`割込を有効にする。
このとき`TCB1_CCMP`は`0`以外の適当な数値で初期化しておく。
初期値`0`のままだと初回割込が生成されずに立ち往生してしまう。

```c
/* TCB1を周期割込モードに設定 */
TCB1_CCMP = ~0;
TCB1_INTCTRL = TCB_CAPT_bm;
TCB1_CTRLB = TCB_CNTMODE_INT_gc;
TCB1_CTRLA = TCB_ENABLE_bm;
```

`TCB1`の割込ハンドラは`LED`出力値をトグルしつつ
`TCB1_CCMP`比較一致レジスタに別途更新される`new_CCMP`値を代入する。
こうすることで次回はその新たな比較一致値で割込が生起する。

```c
volatile uint16_t new_CCMP = ~0;

/* TCB1 比較一致割込 */
ISR(TCB1_INT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  TCB1_CCMP = new_CCMP;
  TCB1_INTFLAGS = TCB_CAPT_bm;
}
```

一方`PIT`周辺機能でもうひとつの周期割込を生成する。
割込間隔は`512Hz`とした。

```c
/* PIT有効化 */
/* CYC64 = 512Hz PIT周期割込 */
loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
RTC_PITINTCTRL = RTC_PI_bm;
RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC64_gc;
```

`PIT`割込ハンドラは
`LED`出力値をトグルするだけだ。

```c
/* PIT周期割込 */
ISR(RTC_PIT_vect) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  RTC_PITINTFLAGS = RTC_PI_bm;
}
```

主ループは`TCB0`の計数捕獲を待ち、
取得した計数値（照度を表す周波数値）で
`new_CCMP`変数を更新する。
この時`F_CPU`が如何なる値であっても
PWM周波数の比率が変化しないよう補正係数`(F_CPU / 100000L)`
を計数値に乗じる。
この計算部分は`ATOMIC_BLOCK`で括って割込から保護する。
もし`new_CCMP`の上下2つのバイト書換のあいだに
`TCB1_INT_vect`割込ハンドラが挟まると、不正動作になるからだ。

```c
#define FREQ_SCALE 516

void loop (void) {
  loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    new_CCMP = (F_CPU - ((uint32_t)TCB0_CCMP * (F_CPU / 1000000L))) / FREQ_SCALE;
  }
  TCB0_INTFLAGS = TCB_CAPT_bm;
}
```

> `new_CCMP`値は`0`になると周期割込が停止してしまう。
このため`F_CPU`値はある程度の大きさ以上でなければならないが
このコードは一応`2MHz`駆動以上であるなら問題はない。
OSC-ULP（32.768kHz）駆動は全く不可だ。

望む PWM周波数の基準値は`512Hz`なのだが、
`PIT`と`CLK_PER`はそれぞれ異なる発振器に由来するクロックで動作しているから
理想的な値とは必ずしも適合しないので、
基準周波数値`FREQ_SCALE`はマクロ定義として調整可能とした。
個体差もあるが`516`前後の値が丁度良いようだ。

以上のスケッチプログラムを実行すると、
照度が暗いときはLED明滅速度が遅く、
明るければLED明滅が早くなるように変化する。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
