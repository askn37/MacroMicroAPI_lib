# Blink_06_PFM.ino

このサンプルスケッチは以下について記述している；

- ふたつの PWM波形を合成して PFM信号出力を得る
- TCA計数器で PWM信号を生成する
- TCB計数器で PWM信号を生成する

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統 14pin以上
  - tinyAVR-0/1系統 8pin
- modernAVR世代
  - AVR Dx系統

以下の記述にある周辺機能の説明は 28pin以上を対象に書かれている。\
14pin/20pin品種については周辺機能割当が異なる。

> `スケッチ.ino`は実際には該当品種別に書かれた`.cpp`を読み込むラッパーになっている。

## Blinkの要件

- 0.5HzのPWM信号を出力する
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

ここでは LED明滅を単なる消灯・点灯ではなく
PWMによる連続的な明度変化として視覚化する。
さらにすべてをハードウェア周辺機能だけで全制御し
CPU本体の処理ループや割込は使用しない。

> 8pin品種では割込を補助的に使用する例を上げる

## PWM出力のおさらい

### TCA

以下にPWM信号を`TCA0`計時器での 8bitPWM動作で出力する基本設定を記す。

```c
/* TCA0の 分割動作で WOA0に Duty比50％信号を出力する */
/* 駆動クロックは CLK_PER から受け 分周比1024とする */
TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
TCA0_SPLIT_LPER = periodic - 1;                       // WOA0 PER
TCA0_SPLIT_LCMP0 = harfperi;                          // WOA0 CMP
TCA0_SPLIT_CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV1024_gc;
```

`TCA`計時器は PWM波形生成器として見た場合

- 16bit幅3ch または 8bit幅6ch の PWM信号出力が可能
- システムクロックを受けて分周比は`(1|2|4|8|16|64|256|1024)`から選択可能\
ただし`(32|128|512)`は選択不可

といった設定ができる。
複数チャネル出力ができるが周期幅は3チャネルセットで共通の設定となり、個別には変更できない。
ここでは分周比1024を選んでいる。

### TCB

以下はPWM信号を`TCB1`計時器での 8bitPWM動作で出力する基本設定だ。

```c
/* TCB1の 8bitPWM動作で WOB1に Duty比50％信号を出力する */
/* 駆動クロックは CLK_TCA から受ける */
TCB1_CCMP = (harfperi << 8) | periodic;               // WOB1 CMP:PER
TCB1_CTRLB = TCB_CNTMODE_PWM8_gc;                     // WOB1
TCB1_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_TCA0_gc;      // CLK_TCA
```

`TCB`計時器は 16bit幅計数機能を持つが、
PWM出力用に設定する場合は 8bit幅動作となる。
`CCMP`レジスタ（これは16bit幅だ）の下位 8bitに 1〜255の周期幅を、
上位 8bitにその Duty比を表す比較値を書かなければならない。

> 一部品種のエラッタに関係するため 8bitレジスタ2個としては扱わない。

`TCB`計時器へ供給クロックは、
システムクロック`CLK_PER`等速、
その2分周、
あるいは`CLK_TCA`等速が選べる。
ここでは`TCA`動作クロックに同期する`CLK_TCA`を使用する。
この他`EVSYS`経由で任意の動作クロックを受けることも可能。

### 8bit幅 PWMの最長周期

上記は何れの場合も PWM周期は 8bit幅指定なので周期長は 256であり、
例えば`F_CPU=24MHz`かつ分周比 1024とした場合、

$$ \frac{24000000}{1024 \times 256} \risingdotseq 91.552734375...Hz $$

より長い周期は得られない。

## PFM変調

ふたつの異なる周波数の波形を重ね合わせたとき
「うなり」という物理現象が現れる。
これは元の波形には含まれない新たな周波数の波形が合成されるものだが、
その合成周波数 N（Hz）は 元の周波数 f1、f2 から次の式で導くことが出来る。

$$ N = \lvert f_1 - f_2 \rvert $$

つまりふたつの周波数の単純な差であり、元の周波数の大小は関係しない。
`140Hz:141Hz`の比からでも
`1000000Hz:1000001Hz`の比からでも
合成して得られる「うなり」は`1Hz`である。

このスケッチではこの性質を利用して目的の`Blink_Hz=0.5`周期を得る。

### 波形合成

PWM信号出力は論理的には単なる0と1の羅列で、
その変化する周期が周波数として現れているにすぎない。
そこでふたつの PWM信号を排他的論理和（XOR）で足し合わせることを考える。

```plain
F1 111000111000111000111000
    v     v     v     v     4 Hz
   ---___---___---___---___
       ^     ^     ^     ^

              XOR

F2 110011001100110011001100
   v   v   v   v   v   v    6 Hz
   --__--__--__--__--__--__
     ^   ^   ^   ^   ^   ^

              ||

F3 001011110100001011110100
        v           v       2 Hz
   __-_----_-____-_----_-__
              ^           ^
```

異なる矩形波のF1:F2から合成されたF3は（PFM変調されて）一見複雑な波形となるが、
図にマークした位置の前後で周期的に同じ波形が現れることが解る。
ある時間範囲内で繰り返されるマーク数がそのまま周波数を意味するので、
先の公式が満たされていることも見て取れる。

AVR内部でこの信号合成を行うには`CCL`周辺機能が利用できる。

## CCLの利用

`CCL`周辺機能はルックアップテーブルを参照して、
最大3入力からひとつの論理合成出力を得る機能だ。

ここでは LUT0テーブルにて
`TCA0`の`WOA0`PWM信号と、
`TCB1`の`WOB1`PWM信号を論理合成する。

```c
CCL_TRUTH0 = CCL_TRUTH_1_bm | CCL_TRUTH_2_bm;
CCL_LUT0CTRLB = CCL_INSEL0_TCA0_gc | CCL_INSEL1_TCB1_gc; // <-- WOA0 XOR WOB1
CCL_LUT0CTRLA = CCL_ENABLE_bm;
CCL_CTRLA = CCL_ENABLE_bm;                               // --> LUT1OUT
```

LUT0は次のように構成した。
単なる2入力XOR演算である。\
OUTが1になる組み合わせのビットフラグを
`CCL_TRUTHn`レジスタに書く。

|WOA0|WOB1|OUT|CCL_TRUTHn|
|---:|---:|--:|----|
|  0 |  0 | 0 |    |
|  1 |  0 | 1 |CCL_TRUTH_1_bm|
|  0 |  1 | 1 |CCL_TRUTH_2_bm|
|  1 |  1 | 0 |    |

`INSEL[012]`に選べる入力信号は多岐に渡るが制約もあり、
例えば`TCAn`の`WO[345]`は選択できない。
このためふたつの周期長の異なる波形を
`TCAn`計時器分割動作だけで発生し、
合成させるには`EVSYS`を経由しなければならない。

> 直接選べない場合は一旦外部物理ポートに向けて出力してEVSYSで折り返す。
> この場合は代償として外部物理ポートをひとつ消費する。

### CCL_TRUTH_n_bm

ルックアップテーブルは`SEL[012]`の3bitマルチプレクサ入力が
どの状態になったら出力を`1`にするか決める。
指定シンボルの`n`に入る`0`〜`7`の数値は、3bitビット入力の論理和に等しい。
使用しない`SEL[012]`入力には常に`0`が入力されるものとして考える。

|SEL2|SEL1|SEL0|TRUTH|
|-|-|-|-|
|0|0|0|CCL_TRUTH_0_bm
|0|0|1|CCL_TRUTH_1_bm
|0|1|0|CCL_TRUTH_2_bm
|0|1|1|CCL_TRUTH_3_bm
|1|0|0|CCL_TRUTH_4_bm
|1|0|1|CCL_TRUTH_5_bm
|1|1|0|CCL_TRUTH_6_bm
|1|1|1|CCL_TRUTH_7_bm

## PWM周期の算出

さてふたつの計時器に与えるPWM周期長`PER`はどのように求めたら良いか。
先に上げた公式

$$ N = \lvert f_1 - f_2 \rvert $$

から f1-f2 の差が 0.5Hz になれば望む結果が得られることがわかる。
周波数の差は周期長の差であるから、これを 1であると定めたとき
両辺に半分ずつ割り振って

$$
\frac{CLK_{TCA}}{PER-0.5}=\frac{CLK_{TCA}}{PER+0.5} + N
$$

が満たされれば良い。
この式を`PER`について解くと

$$
PER=\sqrt{CLK_{TCA} \times \frac{1}{N}}
$$

であるからつまり

```plain
PER = sqrt( CLK_TCA * (1.0 / N) )
```

と書き下せるので

```plain
f1.PER <-- PER - 0.5
f2.PER <-- PER + 0.5
```

が得られる。実際の記述はこうだ。

```c
#include <math.h>
#define BLINK_HZ (0.5)

uint8_t periodic = sqrt((F_CPU / 1024.0) * (1.0 / BLINK_HZ)) - 0.5; // PER-0.5
uint8_t harfperi = periodic >> 1;                                   // CMP (Duty Cycle)
```

計算式に
`<math.h>`の
`sqrt()`関数を含むがその結果は定数値だから
コンパイラ最適化により事前計算定数と置き換えられるので
浮動小数点演算ライブラリが結合されるようなことはない。

### 検算

一応、前述の計算がどのような結果を導くか別のコードで検算してみよう。

```c
#include <math.h>
#define BLINK_HZ (0.5)                          /* 目標周波数差 N */

for (int i = 1; i <= 32; i++) {
  float fq = 1000000.0 * i;                     // F_CPU
  Serial.print((uint32_t)fq, DEC).write(' ');
  fq /= 1024.0;                                 // CLK_TCA
  int per = sqrt(fq * (1.0 / BLINK_HZ)) - 0.5;  // PER-0.5
  Serial.print(   (per)    , DEC).write(':');
  Serial.print(   (per+1)  , DEC).write(' ');
  Serial.println( (fq/(per)) - (fq/(per+1) );   // 実際に得られる周波数差
}
```

`BLINK_HZ`（N）を 1Hz、0.5Hz、0.25Hz と変化させ、
`F_CPU`も変えた結果を整理するとこのようになる。

```plain
            PER   N=1Hz       PER   N=0.5Hz     PER   N=0.25Hz
 1000000   30:31  1.050066   43:44  0.516153   62:63  0.250016
 2000000   43:44  1.032307   62:63  0.500032   87:88  0.255110
 3000000   53:54  1.023651   76:77  0.500629  107:108 0.253521
 4000000   62:63  1.000065   87:88  0.510220  124:125 0.252016
 5000000   69:70  1.010933   98:99  0.503277  139:140 0.250916
 6000000   76:77  1.001259  107:108 0.507042  152:153 0.251949
 7000000   82:83  1.004395  116:117 0.503681  164:165 0.252621
 8000000   87:88  1.020439  124:125 0.504032  176:177 0.250790
10000000   98:99  1.006554  139:140 0.501831  197:198 0.250362
12000000  107:108 1.014084  152:153 0.503899  216:216 0.250015
16000000  124:125 1.008064  176:177 0.501579  249:250 0.251003
20000000  139:140 1.003662  197:198 0.500725  279:280 0.250015
24000000  152:153 1.007797  216:217 0.500031  305:306 0.251122
28000000  164:165 1.010483  233:234 0.501518  330:331 0.250328
32000000  176:177 1.003159  249:250 0.502007  353:354 0.250076
```

`PER`は整数値で書くしかないので当然誤差は生じるが 5%ほどと解る。
そして`PER`は1〜255の範囲でなければならないから
`N=0.25Hz`（4秒周期）では`F_CPU=16Mhz`までしか対応できないことも明らかだ。
また低いCPU動作周波数では相対的に PWM周期長が短くなるので合成波形の粒度も下がり、
LED点滅にフリッカーを感じるだろうことも見て取れる。

> F_CPU=32768（ULP）設定でも動作するから、どう変化するか試してみると良い

## EVSYS

`LUT0_OUT`は`EVSYS`に（外部ポートへの出力を有効にしなくとも）
直接分配されているので、そのまま他の事象入力に流し込むことが出来る。
`LED_BUILTIN == PIN_PA7 == EVOUTA_ALT1`へ配給するには次のようにする。

```c
PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;       // PIN_PA7 <-- EVOUTA
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT0_gc;        // <-- LUT0_OUT
EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;     // --> EVOUTA
// pinModeMacro(PIN_PA7, OUTPUT); // 不要
```

- `PIN_PA7`を`pinMode`で出力に設定する必要はない。
`EVSYS_USER*`を設定すると該当出力先は強制的に`OUTPUT`方向となる。
- 逆に`EVSYS`は入力側の外部ポート設定を自動では`INPUT`許可とはしない。

> tinyAVR-0/1/2の 14pin/20pin品種では`LUT1_OUT == LED_BUILTIN`なのでこの項に該当しない。

## 信号出力補足

このサンプルでは
`WOAn`、`WOBn`、`LUTn_OUT`の各信号は
直接外部ポートに引き出されることなく MCU内部で完結している。
これらは`CCL`、`PORTMUX`、`EVSYS`への入力とする場合
`PORTx`を経由しないで直接扱えるからだ。

それぞれの途中出力を外部から
オシロスコープなどで
外部から観測できるようにするには
次のように追加の設定をする。

```c
/* WOA0 の外部ポート出力 */
TCA0_SPLIT_CTRLC = TCA_SPLIT_LCMP0_bm;
// pinModeMacro(PIN_PA0, OUTPUT); // 不要
```

```c
/* WOB1 の外部ポート出力 */
TCB1_CTRLB |= TCB_CCMPEN_bm;
// pinModeMacro(PIN_PA3, OUTPUT); // 不要
```

```c
/* LUT0_OUT のポート出力 */
/* 既定値は PIN_PA3 なので（WOB1と同じなので）PIN_PA6 に代替する */
PORTMUX_CCLROUTEA = PORTMUX_LUT0_bm;
CCL_CTRLA &= ~CCL_ENABLE_bm;
CCL_LUT0CTRLA = 0;
CCL_LUT0CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;
CCL_CTRLA |= CCL_ENABLE_bm;
// pinModeMacro(PIN_PA6, OUTPUT); // 不要
```

`CCL_LUT`の設定を後から書き換えるには、レジスタに変更保護がかかっているのでいちど
`CCL`と該当`CCL_LUTn`を停止状態に戻さなければならない。
新規設定時もこの制限があるので、書き込むレジスタの順番には順序がある。

> 一般に出口側から入口側に向かって順次設定してゆく。
この逆だと意図しない外部端子に意図しない信号が出力される時間が生じるからだ。

## tinyAVR-0/1 系統 8pin 品種例外

tinyAVR-0/1系統の特に 8pin品種では
前述の機能をすべてそのまま利用できるわけではない。
`PORTMUX`も`EVSYS`も、使い方や記述方法が異なる。

このため
`LED_BUILTIN == PIN_PA3`へ
PFM信号を出力するために
`EVOUT0 == PIN_PA2`を一旦経由し、
ポート割込を使って LEDを駆動するように書く。

```c
pinControlRegister(PIN_EVOUT0) = PORT_ISC_BOTHEDGES_gc;   // EVOUT0の両端検出割込を有効化

PORTMUX_CTRLA = PORTMUX_EVOUT0_bm;               // EVOUT0 を有効化（代替ピン切替の意味ではない）
EVSYS_ASYNCCH0 = EVSYS_ASYNCCH0_CCL_LUT0_gc;     // 非同期チャネル入力0番で LUT0_OUT受信
EVSYS_ASYNCUSER8 = EVSYS_ASYNCUSER0_ASYNCCH0_gc; // EVOUT0(非同期チャネル出力8番)に配給
```

> 割込を使うので他の実装よりCPU占有率は不利だ。\
> これは`LUT0_OUT == PA6`がLEDならば不要だが
`PIN_PA6`は普通`USART0_TXD`でも使われるため
ブートローダー経由書込時に問題を生じるだろう。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
