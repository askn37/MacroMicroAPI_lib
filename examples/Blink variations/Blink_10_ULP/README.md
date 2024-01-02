# Blink_10_ULP.ino

このサンプルスケッチは以下について記述している；

- 超低消費電力内蔵発振器`OSC32K`を使った CPU最低動作速度の実現
- `TCA0`計時器
- `EVSYS`事象システム
- `PORTMUX`ポート多重器
- 独自`main()`関数

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## 主動作周波数を 512Hz まで落とす

`modenAVR`/`megaAVR`/`tinyAVR`の各系統は
メインとなる高速発振器`OSCHF`の他に
32.768kHz超低消費電力発振器`OSC32K`を内蔵している。
これに対しても`OSCHF`同様に前置分周器を接続でき、
これが 64分周比を最低値として設定できるので
`32768/64==512(Hz)`を CPUに
主クロック`CLK_PER`として供給することができる。
`tinyAVR-0`以降の AVRでは、これが内蔵発振器だけで実現可能な最低動作速度となる。

> これ以前の品種ではWDT用128kHz内蔵発振器を256分周した500Hzが最低設定可能動作速度だった。

この動作速度ともなると毎秒僅か512命令（0.000512 MIPS）しか実行できないため、出来ることはごく限られる。
除算を含む演算は秒単位の時間がかかるし、割込ハンドラの利用も実用的なスループットを維持できない。
しかし設定するだけでCPUと無関係に動作し続ける周辺機能を使うだけなら、これで十分な場合もある。

そこでこのスケッチでは`TCA0`計時器を用いて、CPU能力を使わずに自律的にLEDを交互点滅させる。

### CLKCTRLの設定

```c
_PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKSEL_OSC32K_gc);
_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm);
```

1. OSC32K（OSCULP）を選択
1. 前置分周比を1/64とし、分周許可ビットと共に書く

- これらのレジスタはIO書込保護されているため書込許諾マクロ`_PROTECTED_WRITE()`を使用する

## TCA0計時器

`TCA0`計時器の動作は`TCA0_SPLIT`分割動作を選択する。
これはもっとも簡単な計時器応用で
最大256カウント周期で動作することができ、
その範囲内でひとつの比較一致を設定でき、
計数溢れと比較一致時に`WOn`イベントを起こすことができる。
これが分割動作では 6組同時に使うことができ、
`WO[0-5]`信号はそれぞれに1対1で対応する外部ピンを
`LOW/HIGH`にデジタル駆動することができる。

- 計数方向は下降、出力は比較一致時LOW->HIGH、BOTTOM時にHIGH->LOW

主クロック`CLK_PER`を計数機が受け取る際に独自の分周比が設定できるので
これに 4分周比を選ぶと
計時器の動作クロック`CLK_TCA`は`512/4=128Hz`にできる。
これが最大256カウントであるから`128/256=0.5Hz`
つまり2秒で計数機は一周する。

そこでこの半分を比較一致値に選ぶと、
`WO[0-5]`信号は1秒毎に`LOW/HIGH`を交互トグルすることが出来る。
これをLEDに出力すれば目的の`"Blink"`が達成できる。

### TCA0の設定

```c
TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm;
TCA0_SPLIT_HPER = UINT8_MAX;
TCA0_SPLIT_CTRLB = TCA_SPLIT_HCMP0EN_bm;
TCA0_SPLIT_HCMP0 = UINT8_MAX / 2 - 1;
TCA0_SPLIT_CTRLA = TCA_SPLIT_CLKSEL_DIV4_gc | TCA_SPLIT_ENABLE_bm;
```

1. `TCA0`計時器に分割動作を指示する
1. `WO3`に対応するTOP値設定レジスタ`[HL]PER`に最大値を書く（`HPER`は`WO[456]`で共用）
1. `WO3`に対応する比較許可ビットを書く
1. `WO3`用の比較レジスタに最大値の半分を書く
1. `CLK_TCA`分周比を1/4とし、計数動作許可ビットと共に書く

## LED_BUILTIN

マクロ`LED_BUILTIN`はオンボードLEDの接続外部ポート番号を指している。
これが`TCA0`の出力信号`WO[0-5]`と一致しているなら何もすることはないが
そうでない場合は次の選択を計画せねばならない。

1. 物理的に`WO[0-5]`出力ピンと`LED_BUILTIN`とをジャンパワイヤで配線する
1. 割込を使って`LED_BUILTIN`を駆動する：超低速駆動での割込実装は極力避けたい
1. `LED_BUILTIN`に使うピンが論理出力機能`EVOUTx`対応なら論理設定で仮想配線する

### PORTMUXの使い方

`PORTMUX`は周辺機器に割り当てらてた信号と外部ピンの関係を
代替ピンに振り替えることのできる機能だ。
これによって機能が重複する周辺機能信号を別のポートにずらして
同時使用できるようにしたりできる。

例えば`WO[0-5]`は普通外部ピン`PIN_PA[0-5]`に割り付けられているが、
これを`PIN_PD[0-5]`に振り替えることができる。
一部のピンだけを任意に移動できるとは限らず
ポートセット全体の一括変更であることには注意。

> `PORTMUX`の詳細は
[[modernAVR 周辺機能比較一覧]](https://github.com/askn37/askn37.github.io/wiki/Peripheral)
に詳しい。

### EVSYSの使い方

`EVSYS`の設定はまず、捉えるべき事象イベントを
複数あるチャネルの何れかに割り当てる。
事象によっては選べるチャネルに制限がある。
例えば`PORTD`からの入力信号は`CHANNNEL0`で捕捉できない。
また事象によっては同期と非同期の区別がある。
非同期信号は`CLK_PER`が停止していても捉えることができるので
CPUが休止状態であってもイベントを駆動できる。

ついで捕捉した事象をどの周辺機能のどのイベントに送るかを指定する。
ひとつの事象を複数の周辺機器に同時に分配しても良い。
イベントを捉える周辺機能が非同期に活動できるのならば
CPUは休止状態であっても構わない。

### EVSYSとPORTMUXの設定例

```c
/* LED_BUILTIN==PIN_PA7 かつ PIN_PA7==PIN_EVOUTA_ALT_1 の場合 */
pinModeMacro(LED_BUILTIN, OUTPUT);
pinModeMacro(PIN_PA3, OUTPUT);
pinControlRegister(LED_BUILTIN) |= PORT_INVEN_bm;
PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTA_ALT1_gc;
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN3_gc;
EVSYS_USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;
```

1. `LED_BUILTIN==PIN_PA7`をデジタル出力方向ピンとする
1. `WO3==PIN_PA3`をデジタル出力方向ピンとする
1. （任意）`PIN_PA7`の論理を反転：`PIN_PA3`にもLEDがあれば交互点滅する様子が目視できる
1. `EVOUTA`出力経路に`PIN_PA7`を指示する（既定値は`PIN_PA2`）
1. 事象チャネル0入力で`PIN_PA3==PORTA_PIN3`を信号入力する
1. 事象チャネル0出力を`EVOUTA`入力に接続して信号を分配する

## CPU休止動作

以上の`TCA0`設定が行われると、
それは`CLK_PER`が供給される限り
CPU状態と無関係に継続的に動作を続け、
与えられた周期と（比較一致レジスタで定めた）デューティー比を守って
`WO[0-5]`信号を正しく駆動し続けるだろう。
もはやCPUはそれに関与する必要がないので、
CPUを不活性化して消費電力を削減することができる。

```c
set_sleep_mode(SLEEP_MODE_IDLE);
sleep_enable();
sleep_cpu();
```

1. 休止制御器にアイドル動作を指示する
1. 休止制御器を有効にする
1. CPUを休止状態にする

- `power_idle`マクロはこの3命令を1関数で呼び出せる。

## main関数

通常のスケッチにおいては、既定の`main`関数が
`F_CPU`マクロに応じて`CLK_PER`が生成されるよう、CPUを初期設定する。
今回のこのスケッチでは`OSC32K`利用を自前で設定するので、
既定の`main`関数は使われないようにする。
これには単に、自前の`main`を書くようにすれば良い。
そうすることで既定の`main`は上書きされる。

```c
int main (void) {
  /* CLKCTRL */
  /* PORTMUX */
  /* TCACTRL */
  /* SLEEPCTRL */
}
```

> `main`関数は`int`型を戻す宣言でなければならないが
例外的に`return`命令を省略でき、コンパイルエラーも警告も生じない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
