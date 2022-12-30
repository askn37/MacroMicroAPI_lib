# Blink_PINALL.ino

このサンプルスケッチは以下について記述している；

- すべての外部端子の論理を交互に切り替える
  - 正し現在の UART_RxD ポート は除く

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 動作試験用

このスケッチは対象基板の組立工程後の不良検査用でそれ以上の意味は特にない。
各ポートの切替は 10ms の間隔を空けている。

単純にすべての外部端子を対象とするだけなら難しいことはないのだが
ブートローダー経由アップロードを想定した場合、
該当 UARTの RxD端子を出力方向に切り替えると
__対向する UART装置とのあいだで電気的短絡__ が生じてしまう危険がある。
従って該当端子には触れないようにしなければならない。

Macro/Micro API では、`PIN_USART0_RXD` といったマクロで
周辺機能割付端子を直接知ることができる。

一方 IDEメニューで選択されたカレントシリアルポートは、
`Serial` マクロで知ることができるが、
これは USART操作クラスのインスタンスを指している。
だがプリコンパイル処理段階ではアドレス情報が確定せず、
リンカエディタ処理段階で実アドレスが確定する類のものだから`#if`で判断できない。

そのような理由でこのサンプルスケッチは少し面倒な判定処理を行っている。

1. メニューの Variant指定により該当MCUが持つ全ての
USART周辺機能インスタンスは `Serial**` マクロとして用意されている。
このマクロがなければ物理的にその USART周辺機能は存在しない。
存在しないマクロ定義については`#if`判定で弾く。
なお `Serial0A` については必ず存在するはずだから確認省略。
2. 存在する `Serial**` マクロについて `Serial` マクロと比較する。
単純な`&`オペレータ参照ではインスタンスアドレスを直接取得できないため
（`<avr/pgmspace.h\` で定義されている）`pgm_get_far_address` マクロで
リンカエディタ処理アドレスを取得して比較処理する。
3. 比較に成功したら `PIN_USART*_RXD` マクロから割付端子情報を獲得する。

このフローの欠点は、実際には使用しない USART周辺機能クラスの実体とインスタンスが
参照されるがゆえに、やむなく実行プログラムにリンクされてしまうことだ。
つまり何処からも使用されないデッドコードが必ず生じてしまう。
もっともこれは __動作確認用__ のテストコードであるがゆえに、些末な問題として片付けてよいだろう。

## 割付端子情報

Macro/Micro API では、`PIN_***` といったマクロが
Variant指定に応じて全物理端子について定義されている。
これは Arduino API でのそれのような連続する通番ではなく飛び番になっており、
その 8bitフィールド定義に意味が持たされている。

|Bit|説明|
|-|-|
|7,6,5|0〜6 == PORTA〜PORTG
|4|予約(1)
|3|予約(0)
|2,1,0|0〜7 == PIN_0bp〜PIN_7bp

これは \<avr/io.h\> で定義されるIOレジスタやマクロと次のような関係にある。

```c
// 例えば；
// PIN_PC3 == 83  ==   0x53
// PORTA          == 0x0400
// PORTC          == 0x0440
// PORTC_OUTSET   == 0x0445
// PORTC_PIN3CTRL == 0x0453 == _SFR_MEM_ADDR(PORTA) + PIN_PC3
// VPORTA         == 0x0000
// VPORTC         == 0x0008
// PIN3_bp        ==      3
// PIN3_bm        ==   0x08 == (1 << PIN3_bp) == _BV(PIN3_bp)

PORT_t *port      = (PORT_t*)(_SFR_MEM_ADDR(PORTA) + (PIN_PC3 & 0xE0)); // -> PORTC
register8_t *ctrl = (register8_t*)(_SFR_MEM_ADDR(PORTA) + PIN_PC3);     // -> PORTC_PIN3CTRL
VPORT_t *vport    = (VPORT_t*)((PIN_PC3 & 0xE0) >> 3);                  // -> VPORTC
uint8_t bp        = PIN_PC3 & 7;                                        // -> PIN3_bp
uint8_t bm        = _BV(bp);                                            // -> PIN3_bm

port->OUTSET = bm;     // -> PORTC_OUTSET = PIN3_bm;
vport->OUT |= bm;      // -> VPORTC_OUT |= PIN3_bm;
*ctrl = PORT_INVEN_bm; // -> PORTC_PIN3CTRL = PORT_INVEN_bm;

/* 以上の全コンパイル結果 */
/*
    88 e0         ldi r24, 0x08   ; <- bm
    80 93 45 04   sts 0x0445, r24 ; port->OUTSET <- bm
    4b 9a         sbi 0x09, 3     ; vport->OUT Set Bit Immediate
    80 e8         ldi r24, 0x80   ; <- PORT_INVEN_bm
    80 93 53 04   sts 0x0453, r24 ; *ctrl <- PORT_INVEN_bm
*/
```

つまるところ `PIN_***` は `PORTA` レジスタ番地から
該当する `PIN*CTRL` レジスタ番地までのオフセットに一致する 10進数値とした定義だ。
従って `PIN_***` からは操作に必要な IOレジスタを特定することも、
ビット操作位置を確定することもオフセット計算をするだけでできるようになっている。

そして計算過程がすべて定数式であるならばコンパイル最適化効果によって途中経過が省かれ、
最小限のIOレジスタ操作だけが結果的に出力される。

> Arduino互換APIの実装では普通、ピン番号から IOレジスタを特定するのにいくつものテーブル参照を行っている。
その実装は移植性と汎用性に長けるものの、GPIO操作が遅かったり余分なメモリ領域が消費される原因でもある。
__Macro/MicroAPI__ では固定された AVRアーキテクチャ依存設計かつ他アーキテクチャへの移植性は放棄しているため、
ハードウェア制御に最適化された出力コードが得られやすい実装を採用している。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
