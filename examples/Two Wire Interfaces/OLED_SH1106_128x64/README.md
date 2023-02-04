# OLED_SH1106_128x64.ino

このサンプルスケッチは以下について記述している；

- OLEDディスプレイ（SH1106 128x64 pixel）キャラクタコンソール実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 解説

このスケッチは`SH1106`を使用し、
0.96インチ の 128x64 ピクセル OLED を搭載したキャリアモジュールの動作実演だ。
`<OLED_SSD1106_128x64.h>`はこれを横向きの 16キャラクタx4行 英数コンソールとして使用できる機能を提供する。

- 基本動作は
[[OLEDディスプレイ（U119）キャラクタコンソール実演]](https://github.com/askn37/M5_Modules_lib/tree/main/examples/M5_OLED_Console)
と同じである。
正し`SH1106`のGDDRAM搭載量は`SH1107`の約半分なので同じではない。
- `<OLED_SSD1106_128x64.h>`は 128x64 ピクセルディスプレイ用に調整されてはいない。

> `SH1106`は 132x64ピクセルぶんの GDDRAM を持つが、
12x64ピクセルパネルの場合は両端2ピクセルが実在せず表示できない。

配線接続は次のようにする。
IOREF選択電圧は3V3選択とする。

```plain
[Career]   [modernAVR]
    SCL <-- PIN_PA3
    SDA <-> PIN_PA2
    VDD <-- 3V3
    GND --> GND
```

## 初期化

`TWI/I2C`バスの初期化では、速度指定は普通`TWI_SM`とするが
他にこのバスを使うセンサーがないのであれば`TWI_FM` `TWI_FMP`でもよく
キャラクタ表示速度を高速化できる。
バスのAVR側プルアップ抵抗指定はモジュール側のを使うので`false`とする。

`OLED`クラスインスタンスには`Wire`インスタンスと、使用するフォントセットを指定する。
__MicroAPI__ が標準で用意している対応フォントセットは`moderndos_8x16_vh`なので、
これを指定する。表示可能文字コードは`0x20`から`0x7F`の範囲だ。

> `moderndos_8x16_vh`と`moderndos_8x16`は同じグリフだがビットマップ配列方向が異なる。
> `SH1106`では`_vh`接尾詞付のグリフを使用する。

`OLED`の初期化はまず`clear`メソッドを呼ぶことで始まる。
このメソッドは自身のインスタンスを返すので、
メソッドチェーンが可能だ。

```c
#include <OLED_SH1106_128x64.h>
OLED_SH1106_Class OLED = {Wire, moderndos_8x16_vh};

/* setup */
Wire.initiate(TWI_SM, false);
OLED
  .clear()
// .setFlip(true)
// .setRevesible(true)
// .setExtTableInROM(moderndos_8x16_vh) /* 0x80-0xFF External character bank */
// .setExtTableInRAM(extra_table)    /* 0x10-0x1F Volatility External character area */
;
```

`setFlip(bool)`はディスプレイの使用方向を決める。
`false`は左上原点、`true`は右下原点になる。
90度回転機能はない。

`setRevesible(bool)`はピクセルの白黒を決める。
`false`は黒地に白（点灯）、`true`は白地に黒（消灯）で表示する。

`setExtTableInROM`は`0x80`から`0xFF`の文字コードに割り付ける外字フォントセットを指定する。使わなければ表示内容は不定だ。

`setExtTableInRAM`は`0x10`から`0x1F`の文字コードに割り付ける
16キャラクタぶん 256バイトの 外字表示用RAMバッファアドレスを指定する。
これはコンソールのちょうど一行分にあたり、
漢字フォント表示やカーソル合成表示に使うことが出来る。

標準フォントセットのキャラクタビットマップをこのRAMバッファに転写したり
論理合成`OLED_(SET|OR|AND|XOR)`したりするには`setExtFont`メソッドが使える。

```c
for (int i = 0; i < 16; i++) {
  OLED
    .setExtFont(&extra_table[i << 4], '@' + i, OLED_SET) /* Load ROM Font */
    .setExtFont(&extra_table[i << 4], '\x0', OLED_XOR)   /* Exor Mask */
  ;
}
```

> `moderndos_8x16_vh`の場合
`"_^~-="`等がグリフ合成に使える。
`0x7F`には `"°"`（degree）記号のグリフが入っているので
`"Å"`を合成したり、`度°分'秒"`桁区切りが表示できる。

## スクロール表示

単に`OLED`へプリントしていけば、それはスクロールコンソール表示になる。
`\r`で仮想カーソル位置が行頭に復帰、
`\n`で新規改行、
16文字毎に行頭折返しで自動的にスクロールが進む。

```c
OLED.println(F("Hello World!"));
```

## 絶対位置表示

`clear(false)`メソッドで画面消去すると、以後は絶対位置表示モードになる。
仮想カーソルが画面表示外に移動しても自動スクロールはされない。
仮想カーソル位置は左上原点として`setPosition(X,Y)`で自由に設定できる。

```c
OLED.clear(false);
OLED.setPosition(0, 0).print(F("1st line"));
OLED.setPosition(0, 1).print(F("2nd line"));
```

4行分の画面があるので、仮想カーソルの`Y`は`3`まで指定可能だ。
上下方向の表示開始位置は`setScroll(Z)`メソッドで変更することができる。
`Z`には`0`から`63`が指定でき、ピクセル単位で調整できる。
これを使うと1ピクセル単位のスムーズスクロールができる。（画面上下はリング状に繋がっている）

```c
OLED.setPosition(0, 0).print(F("top view"));
OLED.setPosition(0, 3).print(F("end view"));

OLED.setScroll(0);  /* "top view" is top */
OLED.setScroll(48); /* "end view" is top */
```

特定の行だけをクリアするには`newLine(Y)`メソッドを使用する。

```c
OLDE.newLine(1);
```

絶対位置表示を止めてスクロール表示に戻すには`clear(true)`メソッドを実行する。

## その他

`drawTestPattern`メソッドは画面全体をテストキャラクタで埋め尽くす。

```c
OLED.clear(false);
OLED.drawTestPattern();
for (int i = 0; i < 64; i++) {
  OLED.setScroll(i);
  delay_millis(100);
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
