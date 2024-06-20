# SerialUSB_Echo.ino

このサンプルスケッチは以下について記述している；

- SerialUSBの基本的な実演

## 対象AVR

- modernAVR世代
  - AVR-DU系統専用

## 用例

以下は SerialUSB クラスを用いて、USBホスト側から入力された文字列を USBデバイス側から送り返すだけの、単純なエコーバック実演例を示す。

```cpp
#include "SerialUSB.h"

#undef Serial
#define Serial SerialUSB

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);

  /* USB-CDCでキャラクタデバイス型通信ポートを開く */
  Serial.begin(/* BAUD引数は使用されないので省略可能 */);
  while (!Serial);  /* USBホストと通信可能になるまで待機する */
  Serial.println(F("<SerialUSB startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU, DEC);
  Serial.print(F("_AVR_IOXXX_H_=")).println(_AVR_IOXXX_H_);
}

void loop (void) {
  while (Serial.available() > 0) {
    int _c = Serial.read();
    Serial.write(_c);
    /* 改行を検出するたびにLED状態をトグルする */
    if (_c == '\n') digitalWrite(LED_BUILTIN, TOGGLE);
  }
}
```

## 説明

`<SerialUSB.h>`は、USB 2.0 Full-Speed (12Mbps) 対応の USB-CDC 通信を導入できるクラスモジュールだ。
`read()/write()` 関数型インタフェースによるキャラクタデバイス USBシリアル通信を提供する。
これらの関数はブロッキング動作（ハードウェアフロー）であるため、`read()`はホスト側が文字を送るまで、`write()`はホスト側が文字を受け取るまで、主プロセスの進行は抑止される。
これを嫌ってノンブロック動作とするには、それぞれ`available()`で読み込み可能か、`availableForWrite()`で書き込み可能かを事前に調べるのが良い。

基本的には通常の UART `Serial` クラスと使い方は変わらないので `print()/println()` メソッドも通常通り使用できる。

上記の単純な実演サンプルでは、コードサイズ 2960 byte、メモリ消費 260 byte である。

双方向キャラクタデバイス通信でのスループットは、実効1Mbps 程度まで可能である。

- 単方向であれば 2Mbps 程度は発揮可能（OS側の処理性能にも依存）

## 対応OS と 制約

__Microchip AVR-DU__ シリーズにのみ、インストールして実行／利用することができる。他のチップには対応せず動作しない。

Windows／macosとも、通常の OS標準組込ドライバで認識される。追加の`.inf`ファイル組込等は必要ない。
使用される USBベンダーID／プロダクトID、および供給者識別符号は __AVR-DU__ シリーズ限定であるため Microchip社発行の、USB-CDC 通信用の汎用標識を使用している。

- USB-CDC VID: `0x04D8` + PID:`0x0B15`
- 供給者識別符号: `Microchip Technology Inc.`

Windowsの場合、自動選択される `USBser.sys` ドライバの制約により、通信切断時のファイルハンドル自動再接続は行われない。
該当ファイルハンドルをクローズするまで、COMポートの再使用はできない。

`read()/write()` 関数はブロッキングを伴うため、割込禁止状態下と、割込ルーチン内では使用できない。

__AVR-DU__ シリーズ（の`F_CPU`設定）は、最低 12MHz 以上でなければならない。

> ただし __AVR64DU32__ の初期ロットシリコン（Rev. A3）は 20MHz以下でないと正常動作が保証されないので要注意

USB周辺機能が SUSPEND 状態でなければ、CPUを スタンバイ停止状態にすることはできない。
スタンバイ停止状態から USB周辺機能を RESUME 割込で復帰することは可能。
パワーダウン停止状態からの自立復帰はできない。

- SUSPEND／RESUME は通信断を伴うので、標準`USBser.sys`ドライバでは ファイルハンドル再オープンが必要。
- 電源投入のまま USBポートを抜くと通信復帰までの間ミリ間隔で SUSPEND／RESUME 割込を繰り返し発生するため、ハードウェア／ソフトウェアともに対応が必要。現実には VBUS 電圧検出機能を備えていなければ、実用的な省電力化はできない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
