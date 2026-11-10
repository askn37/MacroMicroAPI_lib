# USB_Descriptor_Build.ino

この応用記述は以下について記述している；

- USB記述子テーブルセットの生成と確認
- いくつかの USB記述子テーブルセット 構成例

## 対象AVR

- modernAVR世代
  - AVR-DU系統専用

## 説明

この応用記述には、AVR-DU 系統の拡張NVM（不揮発メモリ `EEPROM`、`USERROW`、`BOOTROW`）に配置することも可能な、いくつかの USB記述子テーブル構成のサンプルが付属している。

- src/Descriptor_CDC_EDBG.cpp
- src/Descriptor_CDC.cpp
- src/Descriptor_EDBG.cpp

それらの構成テーブルは `<USB_Utils.h>` で定義される `get_descriptor_copy`（データメモリ空間用）または `get_descriptor_copy_P`（プログラムメモリ空間用）関数の引数にすることができ、USB の "GET_DESCRIPTOR" 要求 に応じた データ塊を SRAM上（USB周辺機能のエンドポイントバッファ）に展開する。

サンプルを希望の NVM に配置するには、各サンプル中の `USB_DESC_MEM` マクロに、適切な選択値を定義する。それらを MCUボード へ書き込むには IDE の Tools メニューを適宜変更すれば良い。

`_wValues` 配列には、テストしたい "GET_DESCRIPTOR" 要求 の `wValue` 値を列挙する。上位バイトが要求種別、下位バイトが指標番号だ。代表的な 記述子要求は以下に挙げる。指定の記述子を取得できないなら、`get_descriptor_copy*` 関数は 0値を返す。（`General`群は複数の補助機能記述子を内包する）

- `0x0100` - Device Descriptor #0
- `0x0200` - (General) Configuration Descriptor #0
- `0x0600` - Device Qualifier Descriptor #0
- `0x0700` - (General) Other Speed Configuration Descriptor #0
- `0x2200` - HID Class-Specific Report Descriptor #0
- `0x0300` - String Descriptor with wLANGID list
- `0x03xx` - String Descriptor #xx

現在のところ、この構成テーブルには以下の制約がある。（メモリ消費を切り詰めた設計の弊害）

- チェックサム等の確認指標はない。間違いがあれば、装置は単に沈黙する。
- 最後の登録記述子は `0x0300` 記述子でなければならない。記述子の捜索は、線形探索で行われる。
- 個別の記述子長は 254バイト以下とする。`General`群に対しては 255バイト以下とする。
- 登録可能な文字列は 英数ASCII形式のみ。故に USB言語IDは `0x0409` を既定値とする。（本来の UTF16-LE 形式は記述できない）
- 登録する文字列は全て結合して 253文字以下でなければならない。文字列はその中から開始位置と文字数（最大126文字）を指定し、UTF16-LE に変換して出力する。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
