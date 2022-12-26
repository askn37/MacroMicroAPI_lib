# XRTC_FreqCalibHF.ino

このサンプルスケッチは以下について記述している；

- CPU動作周波数校正の実演（PCF85063A用）

## 対象AVR

- modernAVR世代
  - AVR Dx系統

> megaAVR-0とtinyAVR-0/1/2系統は動作しない。\
> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`の`32.768kHz`周波数出力端子を使用して
CPUの`CLK_PER`（`F_CPU`）を校正するデモンストレーションだ。
ジャンパー端子の`PF0`は`COT`側に変更して使用する。

周波数計測表示には
[[コード実行時間を計測するベンチマーク実演（TCB2組32bit計数器使用）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Timer%20applications/Benchmark)
の手法を使っている。

```plain
<startup>
F_CPU=24000000
F_CPU=24011843,493.438ppm:493.438
F_CPU=24013543,564.313ppm:564.313
F_CPU=24009607,400.313ppm:400.313
F_CPU=24011044,460.188ppm:460.188
F_CPU=24009302,387.563ppm:387.563
```

> このプリント表示はシリアルプロッタに出力できる形式になっている。

校正が行われると`F_CPU`の理想値からの誤差は概ね ±600ppm以内（0.06%以内）に調整されるだろう。

`ENABLE_FREQCARIB_EXTERNAL`または`ENABLE_FREQCARIB_AUTOTUNE`マクロを
コメントアウトして無効にすると
±4000ppm（0.4%）程度に誤差が拡大する様子が観測できるだろう。

```plain
F_CPU=23918901,-3379.188ppm:-3379.188
F_CPU=23921308,-3278.813ppm:-3278.813
F_CPU=23918349,-3402.188ppm:-3402.188
F_CPU=23919037,-3373.500ppm:-3373.500
F_CPU=23916784,-3467.375ppm:-3467.375
```

### ENABLE_FREQCARIB_EXTERNAL

このマクロが定義済なら`RTC`周辺機能の`32.768kHz`クロック元を
`PIN_PF0`からの周波数「発振器」入力に切り替える。（発振子ではない）

### ENABLE_FREQCARIB_AUTOTUNE

このマクロが定義済かつ`ENABLE_FREQCARIB_EXTERNAL`マクロが定義済なら、
（AVR_DA/DB/DD系統の）自動校正機能を有効にする。
`RTC`周辺機能が外部周波数入力で動作していない場合は、効果がない。

### ENABLE_FREQCARIB_USER

`ENABLE_FREQCARIB_AUTOTUNE`マクロが未定義の場合、
`ENABLE_FREQCARIB_USER`マクロで手動校正値を与えることが出来る。
指定可能な範囲は`-64`〜`+63`で、調整単位は約300ppm刻み程度である。
ただしUART通信速度もまたこの校正に影響されるので、
通信可能範囲を外れるとシリアルモニター／プロッタ出力はできなくなる。

## F_CPUとの実効周波数の乖離とゆらぎ

AVRの内蔵周波数発振器は、メインの`OSCHF`も、サブの`OSCULP`も、常に数百ppmの変動する「ゆらぎ」を持っている。
精度誤差は温度変化も加味して±1%程度にはなる。
実用上は1.5%も外れなければ非同期USART動作に影響しないので気付きにくいが、
ADCの計測誤差には如実に影響する可能性が排除できない。
0.3%未満の高精度を追求するなら、内蔵発振器の使用は諦めることになるだろう。

AVR_DA/DB/DD系統の自動校正機能はこれを幾らか緩和して、精度を最大1%近く改善する。
ただしこれは`OSCULP`が温度保証付きの外部発振器／発振子で駆動していなければ
意図したようには機能しない。
さらにこれでも取り除けない数百ppmの「ゆらぎ」は、
温度保証付きの外部発振器を`EXTCLOCK`端子に入力して
`OSCHF`を外部駆動しなければ改善できないだろう。

> `OSCHF`を外部発信子で駆動するのは AVR_DB/DD系統だけが対応する。
当然そのぶんの外部入力端子数は減らされる。

## 余談

`PIN_PF0`への周波数入力は`OSCULP`を代替するので、
そのまま`CLK_PER`にすることもできる。
そして発振周波数は`1Hz`に変更することができるから
`F_CPU=1`が実現可能だ。さらにこれに主クロック分周比64を掛けると、
64秒にわずか1ステップしか進まない超々極低速CPU駆動もできてしまう。
もっともこれは理論上出来るというだけで実用価値は全く無いし、
回路のノイズ除去もしっかりやらないとまともに動かない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
