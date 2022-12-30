# Blink_07_PIT_PFM.ino

このサンプルスケッチは以下について記述している；

- ふたつの PWM波形を合成して PFM信号出力を得る
- RTC割込で擬似PWM信号を生成する
- PIT割込で擬似PWM信号を生成する

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## Blinkの要件

- 0.5HzのPWM信号を出力する
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC128_gc;

  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_INTCTRL = RTC_OVF_bm | RTC_CMP_bm;
  RTC_PER = 254;
  RTC_CMP = 128;
  RTC_CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV1_gc | RTC_RUNSTDBY_bm;

  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
}

ISR(RTC_CNT_vect) {
  RTC_INTFLAGS = RTC_OVF_bm | RTC_CMP_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
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

このサンプルスケッチは
[[PFM信号を生成して"Blink"実演（ホタル明滅：CPU不使用）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_06_PFM)
と同等の効果をRTCおよびPIT割込で実装したものだ。

PFM周波数を得る式

$$ N = \lvert f_1 - f_2 \rvert $$

で必要とされる
f1を`PIT`割込で、
f2を`RTC`割込で生成する。

割り込みを使って擬似的に
PWM信号を生成するからには
CPU占有率が高く、電力効率も低下する。
一方で`CCL`や`EVSYS`を使用しないため
MCU品種の差異に実装が左右されにくい。
記述量も最小限で済む。
要するに、割とつぶしが効く。

### PIT割込疑似PWM

PITの設定はこれまでも取り上げているが、次のようになる。
ここでは分周比128を選び、そのたびにLED出力を反転するので
`(CLK_RTC=32768Hz) / 128 / 2 = 128Hz`
で Duty Cycle 50% のPWM信号出力が生成される。

```c
RTC_PITINTCTRL = RTC_PI_bm;
RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC128_gc;

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}
```

### PTC割込疑似PWM

`RTC`割込は 16bit幅の計数器を持ち、
`RTC_OVF`溢れ割り込みと
`RTC_CMP`比較割り込みのふたつを1周期のなかで発生させることが出来る。
それぞれは個別に`EVSYS`へ事象を送ることは出来るが、
直接の外部ピン信号出力機能はない。
`CLK_RTC == 32768Hz`に対して
1から 32768までの16段階の分周比を選べる前置分周器が使えるので
最大周期長は 65536秒となり、AVRが持つ単一計数器では最長の周期長を持つ。
ただし`PIT`と違って一番深い CPU休止状態では活動させる事ができず、
スタンバイ休止状態までという制約がある。

> `CLK_RTC`元に`OSC1K`を選択した場合`2097152秒＝24.27日`周期を作れるが、実用性は不明。

```c
RTC_INTCTRL = RTC_OVF_bm | RTC_CMP_bm;
RTC_PER = 256;
RTC_CMP = 128;
RTC_CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV1_gc | RTC_RUNSTDBY_bm;

ISR(RTC_CNT_vect) {
  RTC_INTFLAGS = RTC_OVF_bm | RTC_CMP_bm;
  digitalWrite(LED_BUILTIN, TOGGLE);
}
```

割り込みベクタはひとつだけ持ち、比較一致と溢れ割り込みで共有している。
どちらの割り込みで起動されたかは`RTC_INTFLAGS`の該当ビットを調べることで区別できるが、
ここでの使い方では両者を区別しないので共通処理だ。
1周期を257計数とし、その中間で比較一致割り込みを起こして
Duty Cycle 50% としている。
得られるPWM信号出力周波数は
`(CLK_RTC=32768Hz) / 257 = 127.502Hz`
だ。

## PFM出力

PFM出力周波数 N は次式で求められる。

$$\frac{CLK_{RTC}}{f1_{PER}}-\frac{CLK_{RTC}}{f2_{PER}} = N$$

従って結果は、

$$\frac{32768}{256}-\frac{32768}{257} = 0.498Hz$$

となる。

## 割込遅延の注意

割込を使っているので主処理の実効性能が引き換えに低下する。
このスケッチでのひとつの割込ベクタは概ね次のようなアセンブラ記述に変換される。

```plain
; ISR(RTC_PIT_vect) {
;   RTC_PITINTFLAGS = RTC_PI_bm;
;   digitalWrite(LED_BUILTIN, TOGGLE);
; }
 2c2: 1f 92       push  r1
 2c4: 0f 92       push  r0
 2c6: 0f b6       in    r0, 0x3f    ; save SREG
 2c8: 0f 92       push  r0
 2ca: 11 24       eor   r1, r1      ; setup __zero_reg__
 2cc: 8f 93       push  r24
 2ce: 81 e0       ldi   r24, 0x01   ; <-- RTC_PI_bm
 2d0: 80 93 53 01 sts   0x0153, r24 ; RTC_PITINTFLAGS <-- RTC_PI_bm
 2d4: 17 9a       sbi   0x02, 7     ; toggle output PIN_PA7
 2d6: 8f 91       pop   r24
 2d8: 0f 90       pop   r0
 2da: 0f be       out   0x3f, r0    ; restore SREG
 2dc: 0f 90       pop   r0
 2de: 1f 90       pop   r1
 2e0: 18 95       reti
```

このような単純な割込ハンドラ記述ですら、
割込要求〜ハンドラ起動〜復帰まで 25から 30サイクルを消費する。
そのCPUサイクル数は CPU駆動速度を増減しても変わらない。

> この割込ハンドラ自体は 20サイクル量だが、割込要求〜起動遅延が
最低5サイクル、sleep命令からの復帰時には都合 10サイクル消費する。

これがもし毎秒 32768回起動されるとして`F_CPU=1000000L`であった場合、

$$\frac{1000000}{32768} = 30.517...$$

なので僅か30クロック周期間隔で起動することとなり、
主処理への実行割り当ては差し引き 1〜2命令ぶんしか残らない。
当然複数割込が同時に要求された場合は処理がとうてい間に合わなくなるので
割込間隔タイミングが遅延していくこととなる。
ゆえに`F_CPU`値を下げたい場合は割込周期量を減らして、
割込要求間隔を十分長く取らなければならない。

> `SREG`保存とゼロレジスタ設定および復帰は例えそれが無用であっても
C/C++言語で記述する限り（コンパイル最適化の対象外で）定型的に挿入される。
この 9サイクル量を取り除くにはアセンブラで直接割込ハンドラを記述しなければならない。\
\
> ちなみによくある（delay関数などで使う） 32bit変数カウントアップ割込の場合は少なくとも
52サイクル量となるので、割込周期の下限は 57サイクル以上となる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
