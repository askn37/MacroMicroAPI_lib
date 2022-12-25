# SoftwareUART_Echo.ino

このサンプルスケッチは以下について記述している；

- 単純なソフトウェア実装UARTの実演。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 用例

以下はソフトウェア実装UARTを用いて
入力された文字列を送信元に送り返すだけの、単純なエコーバック実演例だ。

```c
#include <SoftwareUART.h>
SoftwareUART_Class SoftSerial = {PIN_USART0_TXD, PIN_USART0_RXD};
void setup (void) {
  SoftSerial.begin(CONSOLE_BAUD);
}
void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 4];
  length = SoftSerial.readBytes(&buff, sizeof(buff), '\n');
  if (length) SoftSerial.write(&buff, length);
}
```

ここでは`Serial.readBytes`メソッドを用いて`\n`で終端された行単位で
バッファメモリに入力内容を保持してそれをプリントしているが、
バッファメモリを使わずに文字単位でエコーバックするには以下のようにしたいだろう。
Arduino互換APIの例ではこちらのほうが普遍的だ。

```c
void loop (void) {
	if ( Serial.available() ) Serial.write( Serial.read() );
}
```

だがこれは`<SoftwareUART.h>`ではビルドできない。`read`が実装されていないからだ。

`<SoftwareUART.h>`によるUART制御は「半二重通信」専用であることに注意を要する。
上記の記述は完全な「全二重通信」でなければ正しく成り立たない。
文字送信中に次の文字入力が入ってきた場合、それは確実に失われるからだ。
そもそも組込ハードウェアUARTの支援もどころか、
計時器周辺機能の補助も一切受けないので普通の`read`を実装する余地がない。
`Serial.readBytes`メソッドだけが唯一、UART受信する手段になっている。

> 普通のソフトウェアUART実装はタイミング管理に計時器周辺機能の使用は必須だ。
一方で`<SoftwareUART.h>`は計数器資源も割込ハンドラも一切使わない
非常に原始的な実装になっている。動作原理はそんなものが発明もされてなかった、
ほぼ70年以上前の水準だ。

「半二重通信」はトランシーバーのそれと同じく、通信両端で定めたハンドシェイク規約に支配される。
それは`\n`や`\0`等の送信終端を示すターミネーターであることもあれば
通信ペイロード先頭の固定長ヘッダによる、終端位置を表す文字数であるかも知れない。
いずれにせよ「何が起きたら自分が送信してよい順番なのか」を定義する必要があるわけだ。

`<SoftwareUART.h>`を使わなければならない状況は、
組込ハードウェアUARTの数がどうしても足りない時だろう。
そこでどの通信機器に対してハードウェアUARTか、ソフトウェアUARTかを割り当てる選択が生じる。
`<SoftwareUART.h>`の実装は`read`を持たないので
「固定長ヘッダ＋可変長ペイロード」を持つ対向機器に対しては使用できない。
「可変長ペイロード＋終端文字」を持つハンドシェイク規約にのみ対応しているので
もっぱら「GPS NMEA」のような元々ヒューマンリーダブルになっている用途向きである。

これらは CPUに都合の良いデータ形式に解釈／変換しなければならないオーバーヘッドが大きいので
要求される通信速度が（CPU処理能力に対して）非常に遅い。
ハードウェアUARTを割り当ててもその周辺機器資源が遊んでいる時間がとても長いので、勿体ないのだ。
そういう状況に`<SoftwareUART.h>`は適合するよう配慮されている。

> そもそもの開発動機が
「Grove端子のUART出力GPSを、I2C機能用Grove端子で通信可能にする」
ことだったので、最初からそういう想定なのである。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
