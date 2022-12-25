# HardwareUART_Echo.ino

このサンプルスケッチは以下について記述している；

- 単純な組込ハードウェア実装UARTの実演。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 用例

以下は組込ハードウェア実装UARTを用いて
入力された文字列を送信元に送り返すだけの、単純なエコーバック実演例だ。

```c
#include <peripheral.h>
void setup (void) {
  Serial.begin(CONSOLE_BAUD);
}
void loop (void) {
  size_t length;
  char buff[INTERNAL_SRAM_SIZE / 4];
  length = Serial.readBytes(&buff, sizeof(buff), '\n');
  if (length) Serial.write(&buff, length);
}
```

__Macro/MicoroAPI__ が有効な場合`スケッチ.ino`は既定で
`<peripheral.h>`を読み込んでいるため、最初のヘッダ導入は必要ない。

ここでは`Serial.readBytes`メソッドを用いて`\n`で終端された行単位で
バッファメモリに入力内容を保持してそれをプリントしているが、
バッファメモリを使わずに文字単位でエコーバックするには以下のようにする。
Arduino互換APIの例ではこちらのほうが普遍的だろう。

```c
void loop (void) {
	if ( Serial.available() ) Serial.write( Serial.read() );
}
```

ただし __MicroAPI__ で提供される組込UART制御は「半二重通信」であることに注意を要する。
上記の記述は完全な「全二重通信」でなければ正しく成り立たない。
文字送信中に次の文字入力が入ってきた場合、それは失われる可能性が高いからだ。

「半二重通信」はトランシーバーのそれと同じく、通信両端で定めたハンドシェイク規約に支配される。
それは`\n`や`\0`等の送信終端を示すターミネーターであることもあれば
通信ペイロード先頭の固定長ヘッダによる、終端位置を表す文字数であるかも知れない。
いずれにせよ「何が起きたら自分が送信してよい順番なのか」を定義する必要があるわけだ。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
