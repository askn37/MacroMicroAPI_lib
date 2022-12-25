# Special_AVR_DD14.ino

AVR_DD14/20品種での USART1_TXD 端子代替方法の実演

## 対象AVR

- modernAVR世代
  - AVR_DD系統 AVR_DD14/20

## AVR_DD14は割と御しがたい

AVR_DD14/20系統は一見、
ATtinyの後継機種に見えるにも関わらずソフト・ハード両面に互換性がほとんど無い、
独特なポジションにある。
現実的には AVR_DB系統の小ピン特化コアと見なすことができ、
複電圧対応端子機能も継承している。
コア機能だけを見れば相当リッチだ。
一方で複電圧電源端子があるがために実効端子数が減らされており、
特に14pin品種ではこれに由来する窮屈な制約が多いので
上位互換とも下位互換とも言えなくなってしまっている。

>UPDIとRESETを専用割付とした場合、汎用外部端子は9本しか残らない。

この MCU系統を選ぶ旨味は
低電圧でも内蔵発振器で 24Mhz駆動ができることと
（オーバークロックを許せば 32Mhz動作も可）
ハードウェアUSARTを2本持っていることだろう。
ところがRS485動作を有効化しようとすると、
2組同時に使えないことに気付かされる。
そもそも使える端子が少ないので、代替端子選択の幅も狭く
AVR_DD14では以下の4通りしか選べない。

|端子配置の組|TXD|RXD|XCK|XDIR|
|--|--|--|--|--|
|USART0_DEF |PA0|PA1|x|x|
|USART0_ALT3|PD4|PD5|__PD6__|__PD7__|
|USART1_DEF |x|PC1|PC2|PC3|
|USART1_ALT2|__PD6__|__PD7__|x|x|

素直にRS485を使うには`USART0_ALT3`を選ぶしかないが、
すると`PD6/PD7`が被っているため
`USART1`で選べる端子がなくなってしまう。

> 他にUSART 2本を持つ14pin外囲器品種には tinyAVR-2系統があって、
そちらにはこういう制約はなく RS485同期動作2本が同時利用できるから
普通は素直にそちらを選択したほうが得だ。
ただし低電圧時の動作クロック上限定格が大幅に制限される短所がある。

だいたい`USART1_DEF`で
TXD端子が使えないのがこの問題の元凶なのだが、
TXD出力信号自体がなくなっているわけでもないことは
`USART1_ALT2`で
TXDが普通に使えることからも明らかだ。
単に物理的に外部へ引き出されていないのである。

この事実は`CCL/LUT`のLUT入力選択表を確認することでも解る。

|LUT入力元|INSEL0|INSEL1|INSEL2|
|--|--|--|--|
|USARTn|USART0_TXD|USART1_TXD|USART1_TXD|
|SPI0|SPI0_MOSI|SPI0_MOSI|SPI0_SCK|

こちらでは代替端子選択の有無がないのでポート多重器を経由する前の、
直接USART周辺機能から出力信号が内部配線されているだろうことが推測できる。

そこで`CCL/LUT`と`PORTMUX`を経由して、
不足している`USART1->TXD`を他の未使用端子に配置することを検討する。
これが本稿の議題だ。

## 代替端子の選択

未使用端子ならどれでも
`USART1`の
代替TXD出力に使えるというわけでは、当然ない。
論理的に選べるのは次の5本だ。

- PC2 <-- EVOUTC DEF出力
- PC3 <-- LUT1_OUT DEF出力
- PD6 <-- LUT2_OUT ALT出力
- PD7 <-- EVOUTD ALT出力
- *PF7* <-- EVOUTF ALT出力

このうち`PF7`は既定で
UPDI端子であるから先に除外する。
残りの4本は何れも一長一短だ。

- `PC2`は`USART1`を同期動作とした場合に自分の`XCK`出力と衝突する。
- `PC3`は`USART1`をRS485動作とした場合に自分の`XDIR`出力と衝突する。
- `PD6`は`USART0`同期動作時の`XCK`でもある。
- `PD7`は`USART0`RS485動作時の`XDIR`でもある。

結論から言うと2本のUSART周辺機能を同時に「RS485+RS485」や「同期＋同期」動作にすることは
（UPDIを無効化して PF7を使わない限り）端子の空きがないので、できない。

`USART0`の設定により`USART1`が同時には使えない組み合わせを書き出すとこうなる。
`USART1`の`+?`端子に当てるべきTXD代用端子がなければ`x`だ。
__太字__ は復電圧対応端子である。

|TXD代替選択表|USART0→|非同期|同期|RS485非同期|RS485同期|
|-|-|-|-|-|-|
|USART1動作↓|専有端子↓→|PA0/1|PD4/5/6|PD4/5/7|PD4/5/6/7|
|二線非同期 DEF |PC1+?|__PC2 PC3__ PD6 PD7|__PC2 PC3__ PD7|__PC2 PC3__ PD6|__PC2 PC3__|
|二線非同期 ALT2|PD6/7|PD6|x|x|x|
|単線非同期 ALT2|PD6|PD6|x|PD6|x|
|二線同期 DEF   |PC1/2+?|__PC3__ PD6 PD7|__PC3__ PD7|__PC3__ PD6|__PC3__|
|単線同期       |不可|x|x|x|x|
|RS485二線非同期|PC1/3+?|PD6 PD7|__PC2__ PD7|__PC2__ PD6|__PC2__|
|RS485二線同期  |PC1/2/3+?|PD6 PD7|PD7|PD6|x|
|RS485単線非同期|不可   |x|x|x|x|
|RS485単線同期  |不可   |x|x|x|x|

- `USART0`は単線/二線非同期動作なら`PA0/PA1`（DEF）選択を優先、
それ以外では XCKか XDIRが必要なので`PD4-PD7`（ALT3）選択となる。
- `USART0`が単線動作の場合、未使用になるのはPA1かPD5であり、他に影響がないため省く。
- `USART1`が単線動作できるのは`PD6`（ALT2）選択時のみであり、
単線同期は XCKがないため対応不可、
RS485動作も XDIRがないため対応不可となる。

なお`PC2/PC3`を TXD代替端子に選ぶ明らかな利点は、
`PC1:RXD`入力ともども複電圧対応ポート群にあることだ。
このためMCU本体を2.5V動作にしつつ`USART1`の対向先を5V論理にすることも、
その逆もできる。

RS485差動回路を付属させることを考えると、これは複電圧動作である理由はない。
復電圧動作であってほしいのは対向先と配線先を直結する場合なので、
`XDIR`が同電位である必然性はないからだ。
`XCK`は同電位でなければならないが、通信方向は主従装置関係が決まれば単方向なので
ダイオード1個によるレベルシフトでカバーすることが出来るだろう。

## 設定方法

以下は`USART0` `USART1`とも二線式非同期全二重に使うとし、
`USART1`用TXD代替端子設定について述べる。

### PC3を選択

`PC3`を使う場合、
単に`LUT1`を設定すればよい。

```c
/* PC3 に USART1_TXD を転送する */
CCL_TRUTH1 = CCL_TRUTH_2_bm;                    // INSEL1が真ならOUTも真
CCL_LUT1CTRLB = CCL_INSEL1_USART1_gc;           // INSEL1にUSART1_TXDを入力
CCL_LUT1CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;   // LUT1動作と LUT1_OUT出力を有効化
CCL_CTRLA = CCL_ENABLE_bm;                      // CCLを有効化
pinModeMacro(PIN_PC3, OUTPUT);                  // PC3を出力方向
/* 以後USART1_DEF の設定... */
```

> 設定の順序に注意。LUT->CCL->PORTの順で行う。以下同。

### PD6を選択

`PD6`を使う場合、
これは`LUT2_OUT`の代替出力端子なので
ポート多重化器の設定も行う。

```c
/* PD6 に USART1_TXD を転送する */
PORTMUX_CCLROUTEA = PORTMUX_LUT2_ALT1_gc;       // LU2_OUTを代替端子に変更
CCL_TRUTH2 = CCL_TRUTH_2_bm;                    // INSEL1が真ならOUTも真
CCL_LUT2CTRLB = CCL_INSEL1_USART1_gc;           // INSEL1にUSART1_TXDを入力
CCL_LUT2CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;   // LUT2動作と LUT2_OUT出力を有効化
CCL_CTRLA = CCL_ENABLE_bm;                      // CCLを有効化
pinModeMacro(PIN_PD6, OUTPUT);                  // PD6を出力方向
/* 以後USART1_DEF の設定... */
```

- PORTMUXによる代替端子割当はそれを使う以前に設定しなければならない。以下同。

### PC2を選択

`PC2`を使う場合、
出力は`EVOUTC`から行うので
`CCL`は`LUT0/1/2`のどれであってもよい。
`LUTn_OUT`の直接出力は使わないことに注意。

```c
/* PC2 に LUT2 から USART1_TXD を転送する */
CCL_TRUTH2 = CCL_TRUTH_2_bm;                    // INSEL1が真ならOUTも真
CCL_LUT2CTRLB = CCL_INSEL1_USART1_gc;           // INSEL1にUSART1_TXDを入力
CCL_LUT2CTRLA = CCL_ENABLE_bm;                  // LUT2動作を有効化
CCL_CTRLA = CCL_ENABLE_bm;                      // CCLを有効化
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT2_gc;    // EVSYSに LUT2_OUTを配給
EVSYS_USEREVOUTC = EVSYS_USER_CHANNEL0_gc;      // EVSYSから EVOUTCに配給
pinModeMacro(PIN_PC2, OUTPUT);                  // EVOUTC==PC2を出力方向
/* 以後USART1_DEF の設定... */
```

- EVSYS設定はそれを使う前に行えばよいが、PORTMUX変更より前に行うべきではない。

### PD7を選択

`PD7`を使う場合、
出力は`EVOUTD`の代替端子から行うので
`CCL`は`LUT0/1/2`のどれであってもよい。
`LUTn_OUT`の直接出力は使わないことに注意。

```c
/* PD7 に LUT2 から USART1_TXD を転送する */
PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTD_ALT1_gc;   // EVOUTDを代替端子に変更
CCL_TRUTH2 = CCL_TRUTH_2_bm;                    // INSEL1が真ならOUTも真
CCL_LUT2CTRLB = CCL_INSEL1_USART1_gc;           // INSEL1にUSART1_TXDを入力
CCL_LUT2CTRLA = CCL_ENABLE_bm;                  // LUT2動作を有効化
CCL_CTRLA = CCL_ENABLE_bm;                      // CCLを有効化
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT2_gc;    // EVSYSに LUT2_OUTを配給
EVSYS_USEREVOUTD = EVSYS_USER_CHANNEL0_gc;      // EVSYSから EVOUTDに配給
pinModeMacro(PIN_PD7, OUTPUT);                  // EVOUTD==PD7を出力方向
/* 以後USART1_DEF の設定... */
```

### PF7を選択

`PF7`を使う場合、
これは`PD7`を選ぶ場合と同じ設定の流れだが、
加えて`FUSE`を書き換えて`UPDI`を無効化しなければならない。
当然その状態から再度`UPDI`制御を取り戻すには`HV書込制御`が要求される。

```c
/* 事前にFUSE書換が必要 */
/* PF7 に LUT2 から USART1_TXD を転送する */
PORTMUX_EVSYSROUTEA = PORTMUX_EVOUTF_ALT1_gc;   // EVOUTFを代替端子に変更
CCL_TRUTH2 = CCL_TRUTH_2_bm;                    // INSEL1が真ならOUTも真
CCL_LUT2CTRLB = CCL_INSEL1_USART1_gc;           // INSEL1にUSART1_TXDを入力
CCL_LUT2CTRLA = CCL_ENABLE_bm;                  // LUT2動作を有効化
CCL_CTRLA = CCL_ENABLE_bm;                      // CCLを有効化
EVSYS_CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT2_gc;    // EVSYSに LUT2_OUTを配給
EVSYS_USEREVOUTF = EVSYS_USER_CHANNEL0_gc;      // EVSYSから EVOUTFに配給
pinModeMacro(PIN_PF7, OUTPUT);                  // EVOUTF==PF7を出力方向
/* 以後USART1_DEF の設定... */
```

## サンプルスケッチ

`USART1`の`Serial1A`代替選択に`PC1:RXD` `PC3:TXD`端子を設定して、
エコーバックする例を示す。無論複電圧でも動作する。

> `PC2:XCK`が残っているので同期動作も設定可。

```c
/* for AVR_DD14 */
void setup (void) {
  Serial1A.begin(CONSOLE_BAUD);  // Enable PC1:RXD PC0:not_implemented
  CCL_TRUTH1 = CCL_TRUTH_2_bm;
  CCL_LUT1CTRLB = CCL_INSEL1_USART1_gc;
  CCL_LUT1CTRLA = CCL_ENABLE_bm | CCL_OUTEN_bm;
  CCL_CTRLA = CCL_ENABLE_bm;
  pinModeMacro(PIN_PC3, OUTPUT); // Enable PC3:TXD
}
void loop (void) {
  if (Serial1A.available()) Serial1A.write(Serial.read());
}
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
