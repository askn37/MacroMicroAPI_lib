# Servo_TCB2.ino

このサンプルスケッチは以下について記述している；

- サーボモーターをシリアルモニターからPWM制御する実演（FS0403用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
- modernAVR世代
  - AVR Dx系統

> tinyAVR-0/1/2系統と AVR_DD14/20系統は対応しない。

## サーボモーターのPWM制御

ここでは
[[FEETECH FS0403]](https://akizukidenshi.com/catalog/g/gM-14805/)
での制御例を取り上げる。
このサーボモーターの場合の基本スペックは；

- 電源電圧 DC4.8V-6.0V
  - ストール電流 500mA（4.8V）600mA（6.0V）
- 制御端子 JS型コネクター ピッチ2.54mm 3端子
  - 信号入力 橙
  - 電源入力 赤
  - 接地入力 茶
  - 電源線と信号線には保護ダイオード（フリーホイールダイオード）を並列接続するのが望ましい
- 制御方式 FUTABA互換
  - 右回転60度 900us 正パルス
  - 中央0度 1500us 正パルス
  - 左回転60度 2100us 正パルス

この制御信号を`TCB2`計時器を使用して生成する。

### PWM初期設定

`TCB2`を使うのは既定の`WO`信号出力端子が`PIN_PC0`だからだ。
`TCB0`は`PIN_PA2`、`TCB1`は`PIN_PA3`が既定端子なので
`TWI0`（I2C）制御端子と被っていて使いにくいことによる。

この`TCB2`を`CLK_PER`に対する分周比2の「単発動作モード」に設定し、
`WO`信号出力も有効にする。

```c
/* TCB計時器 : TCB2 WO -> PIN_PC0 */
TCB2_CCMP = POSITION_NEUTRAL;
TCB2_CTRLB = TCB_CCMPEN_bm | TCB_CNTMODE_SINGLE_gc;
TCB2_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;
loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);
```

比較一致レジスタ`TCB2_CCMP`の初期値`POSITION_NEUTRAL`マクロは、
中央0度位置である`1500us`を指す値だ。
同時に最小値`POSITION_MIN`と最大値`POSITION_MAX`は次のように定義する。
分周比2であるので`(F_CPU / 2MHz)`が1マイクロ秒に一致する制御粒度となる。

```c
/* PWM Servo neutral position = 1500 us */
#define POSITION_MIN     ( 900L * (F_CPU / 2000000L)) /*  30 deg min right */
#define POSITION_NEUTRAL (1500L * (F_CPU / 2000000L)) /*  90 deg neutral   */
#define POSITION_MAX     (2100L * (F_CPU / 2000000L)) /* 150 deg max left  */
```

> 分周比1でも`F_CPU=24MHz`駆動までなら問題ないが、
もしそれ以上だと桁溢れしてしまうので分周比2としている。

「単発動作モード」では計数値`TCB2_CNT`が計数を開始すると`WO`出力端子が`HIGH`となり、
計数値が比較一致レジスタ`TCB2_CCMP`に一致すると
`WO`出力端子を`LOW`に駆動して計数を停止する。
計数中は`TCB2_STATUS`の`TCB_RUN_bp`ビットが立つので、
その間は`TCB2_CCMP`を再設定してはならない。
`TCB2`有効化直後に`loop_until_bit_is_clear`でビットクリアを待っているのは
初回パルス生成完了待ちのためだ。

## シリアルモニターからの角度入力

サーボ位置はシリアルモニターからUARTで受け取るものとする。
__MicroAPI__ の`<api/HarfUART.h>`にはストリーム入力機能がないので、
ここでは AVR-LIBC標準の`<sidlib.h>`で用意されている
`strtol`を使って数値文字列入力を`long`型変数に変換している。

```c
#include <stdlib.h>

long _angle;
size_t _length;
char _buff[5];
_length = Serial.readBytes(&_buff, sizeof(_buff), '\n');
if (_length) {
  _angle = strtol((const char*)&_buff, NULL, 10);
  if (-60 <= _angle && _angle <= 60) {
    /* STUB */
  }
}
```

`Serial.readBytes`は`\n`（エンターキー）入力を終端として
シリアルモニターから文字列を受け取り、それを`strtol`の第1引数に渡す。
第2引数は使わないので`NULL`とし、第3引数に10進数を意味する基数`10`を指示する。
変換に失敗した場合の返値は`0`だが、これはそのまま中央位置を示すものとして弾かない。
そうしておいて取得結果`_angle`が正しい角度範囲にあることを確認したら、
この例の条件式は真となって`STUB`が実行されることになる。

> `_buff`量を超えて文字入力された場合`strol`は範囲外まで読みに行くことに注意。
その場合意図しない巨大な数値が返ってきたりするが、
この例では取得結果の範囲を狭めているので影響はない。\
\
> 任意の複雑な入力解析には`<stdio.h>`の`sscanf`が利用できるが、
こちらは`prtntf`同様に1.9KiB以上の出力HEX/bin量増加となる。
小容量品種では到底無視できないので`printf`と併用するのでなければ採用をよく検討することだ。

## PWMパルス単発出力

この`STUB`に書かれるのは以下の内容だ。

```c
TCB2_CCMP = map_long(_angle, -60, 60, POSITION_MIN, POSITION_MAX);
TCB2_CNT = 0;
loop_until_bit_is_clear(TCB2_STATUS, TCB_RUN_bp);
```

`map_long`で入力角度の下限／上限を実際に必要な
最小値`POSITION_MIN`／最大値`POSITION_MAX`に射影して、
比較一致レジスタ`TCB2_CCMP`に書く。
そして`TCB2_CNT`に`0`を書くと新規の計数進行が始まり、
計数中は`WO`出力端子が`HIGH`に駆動される。
この計数が終わって`LOW`に戻るのを
`loop_until_bit_is_clear`で待つという流れだ。

## 補足

FUTABA型のサーボ制御の場合、制御信号入力がなければ
（信号線が`LOW`であるあいだは）モーターを駆動せず現在位置を保つ。
このためPWM信号を連続したパルスとして送リ続ける必要はないので、
ここでは単発パルスを送信する実演となっている。
一方継続的なPWM信号を要求する型のサーボの場合、
`TCB`計時器の使用は適していない。
「PWM出力モード」を選ぶと 8bit幅計数しかできず、かつ分周比が
`1`か`2`しか選べないため、十分な時間長のパルス幅を生成できないからだ。
その場合は`F_CPU`を落とすか、
`EVSYS`経由で別に作った`CLKSEL_EVENT`を送りこむか
（これは __megaAVR-0__ ではできない）
あるいは素直に16bit幅かつ分周比選択肢の多彩な`TCA`計時器の採用を選択する。

なお複数サーボ同時制御の場合`TCB`の`WO`直接出力先はひとつだけなので、
これを別の外部端子制御に変える必要がある。
`loop_until_bit_is_clear`時間待ちの前後に
任意の`degitalWrite`を置くような実装が簡単だろう。

> `TCA`計時器は大多数の品種で1組しか持たないため、どの用途に選ぶかは悩みどころだ。
一方で`TCA`だと同時に3組（精度が荒くて良ければ6組）のサーボを制御できる。
かたや同時制御では瞬間的大電力消費による制御不良問題も纏わり付いてくる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
