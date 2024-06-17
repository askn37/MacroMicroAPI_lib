# XRTC_Blink_PFM.ino

このサンプルスケッチは以下について記述している；

- 外部RTCと内蔵RTCでのPFM信号生成"Blink"実演（ホタル明滅）（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`の`32.768kHz`周波数出力端子を使用して
`OSCULP`用の内蔵周波数発振器との速度差でPFM信号を生成し、LEDの明滅速度を変化させる。
内容的には
[[PITでPFM信号生成して"Blink"実演（ホタル明滅）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_07_PIT_PFM)
の変種である。
明滅速度は、周波数誤差が少なければ少ないほど、遅い。

## XRTC 周期タイマーカスタマイズ

通常の任意時間周期タイマーは、長周期割込用途で
`startPeriodTimer`または
`startCountdownTimer`を使って制御する。
だがここでは内蔵発振器による`PIT`周期幅と並べたいので、
秒精度ではおそすぎる。
そこで`activateTimer`のカスタマイズ機能を使用して
より高い周波数を`PIN_PF1`に送り込ませる。

```c
XRTC_TIMER_SETTINGS _settings = {
  64    // .Value           : 4096Hz / DIV64 = 64Hz
, true  // .InterruptMode   : Periodic Interrupt
, true  // .InterruptEnable : Interrupt Enable
, true  // .Enable          : Timer Enable
, 0     // .ClockFrequency  : Select Freq 4096Hz
};
XRTC.activateTimer(_settings);
```

`XRTC_TIMER_SETTINGS`構造体で指定可能な最速周波数は`4096Hz`だ。
これを64分周して`64Hz`を得ている。
他のフラグは周期出力機能、割込機能、タイマー機能ともすべて使うので、
みな`true`とする。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
