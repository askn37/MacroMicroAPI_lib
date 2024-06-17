# ReadUART_Echo.ino

このサンプルスケッチは以下について記述している；

- リードバッファ付き組込ハードウェア実装UARTの実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## 用例

以下はリードバッファ付き組込ハードウェア実装UARTを用いて入力された文字列を送信元に送り返すだけの、単純なエコーバック実演例を示す。

```c
#include <ReadUART.h>

Settings_SerialR0A(ReadUART_BUFFSIZE);

#ifdef Serial
#undef Serial
#define Serial SerialR0A
#endif

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));
}

void loop (void) {
  while (Serial.available()) {
    Serial.write( Serial.read() );
  }
}
```

`<ReadUART.h>`は任意量のリードバッファを付加したシリアルポートインタフェースを提供する。
基本機能は`<api/HarfUART.h>`と同等で、`Serial.write`機能については変わりがない。
つまりライト方向は 1バイトの組み込みバッファのみを持ち、割り込み禁止中でも`Serial.write`で書くことができる。
一方でリード方向は受信割り込みを使用するため、割り込み禁止中の`Serial.read`と`Serial.readBytes`は機能しない。

`<ReadUART.h>`は複数の組み込みUSARTを同時並行使用する際の、受信取りこぼしを緩和する。
一方で割り込み頻度と負荷は CPUクロック速度に強く支配されるため、最大受信可能速度は`<api/HarfUART.h>`より低く制限される。

### define Settings_SerialR\[n\]\[x](S)

指定のシリアルインタフェースを、指定のバッファサイズで使用可能にする。
例えば、使用するインスタンス名が`SerialR0A`ならば`Settings_SerialR0A(S)`を記述する。
使用可能なインスタンス名は選択中の AVR品種に依存しており、`<peripheral.h>`で定義される`_PORTMUX_USART[n][x]`と対応している。
異なる`[n]`の同時使用は可能だが、`[x]`は同一の`[n]`内で排他使用だ。
同一`[n][x]`を持つ`<api/HarfUART.h>`の`Serial[n][x]`とも排他使用となる。

初期化されたインスタンス`SerialR0A`等は同時に`extern`でエクスポートされる。
従ってソースコード全体で「宣言」できるのはただ一箇所でなければならない。
翻訳単位外のソースファイルから使用するには`extern`でインポートする。

```c
#include <ReadUART.h>
extern ReadUART_Class SerialR0A;
```

通常、`Serial[n]`マクロは`<api/HarfUART.h>`を使うよう事前定義されているため、`<ReadUART.h>`を指定するにはマクロの再定義が必要になる。

```c
#ifdef Serial
#undef Serial
#define Serial SerialR0A
#endif
```

バッファサイズ`(S)`は 1以上の任意量で設定できる。
省略はできない。
2の冪乗である必要もない。
推奨値は`ReadUART_BUFFSIZE`マクロ定数で定義されている。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
