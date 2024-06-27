# MemSpace.ino

このサンプルスケッチは以下について記述している；

- ソースコード内での特殊メモリ初期化と HEXファイル生成の実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## 記述例

```cpp
/* .ino や .cpp ファイル */
#include <avr/io.h>
#include <api/memspace.h>

const char eeprom[EEPROM_SIZE] EEPROM = "0123456789ABCDEF";

const char usersig[USER_SIGNATURES_SIZE] USERSIG = "0123456789ABCDEF The_quick_brown_fox_jumps_over_the_lazy_dog.";

/* DU and EB series only */
const char bootsig[BOOTROW_SIZE] BOOTSIG = "The_quick_brown_fox_jumps_over_the_lazy_dog. 0123456789abcdef";
```

```c
/* FUSES 構造体は .c ファイルでしか初期化できない */
#include <avr/io.h>
#include <avr/fuse.h>

FUSES = {
  .WDTCFG   = FUSE_WDTCFG_DEFAULT
, .BODCFG   = FUSE_BODCFG_DEFAULT
, .OSCCFG   = FUSE_OSCCFG_DEFAULT
, .SYSCFG0  = FUSE_SYSCFG0_DEFAULT | FUSE_RSTPINCFG | FUSE_UPDIPINCFG
, .SYSCFG1  = FUSE_SYSCFG1_DEFAULT
, .CODESIZE = FUSE_CODESIZE_DEFAULT
, .BOOTSIZE = FUSE_BOOTSIZE_DEFAULT
, .PDICFG   = FUSE_PDICFG_DEFAULT   /* DU and EB series only */
};
```

## 解説

この実演スケッチは、具体的には何も実行しない。ビルド出力確認（`Ctrl+Alt+S`または`⌘+Alt+S`）を実行すると、特殊メモリ内容を含む初期化用ファイルが生成される様子を示すだけだ。

> メニューの "スケッチ例" リンクから直接呼ばれた場合、ビルド出力確認は機能しない。スケッチフォルダを別の場所に複製してから実行する必要がある。

`EEPROM`属性（あるいは`EEMEM`属性）を付加された`EEPROM`領域データは、".eep"（HEX形式）ファイルに保存される。

`USERSIG`属性を付加された`USERROW`領域データは、".urow"（HEX形式）ファイルに保存される。

`BOOTSIG`属性を付加された`BOOTROW`領域データは、".brow"（HEX形式）ファイルに保存される。この領域は AVR-DUおよび EBシリーズにのみ存在するため、他の MCUを指定している場合は無価値である。

## FUSES初期化構造体

`FUSES`初期化は、".fuse"（HEX形式）ファイルに保存される。これは`<avr/fuse.h>`をインクルードした ".c"ファイル内でしか正しく記述できない。（CとC++では構造体初期化記述方法に完全互換性がない）

SDKでは通常、`FUSE`操作はメニュー選択肢により行うが、`FUSES`初期化はメニューでは省略されているすべての要素を自由に定義することができる。これはコードの実行内容が特定の`FUSE`設定に依存していて不可分である場合、コード内でそれを管理できるという点で有用である。

その一方で、`FUSES`初期化内容は漏れなく完全でなければならず、不完全な初期値は時に回復不能で深刻なダメージを MCUに与える。正しい内容はそれぞれの製品データシートやヘッダファイルに記載されており、品番によっては流用できない場合も多い。この方法で出力した ".fuse"ファイルを実環境に適用する場合は、慎重に吟味されたい。

> `SYSCFG0`項目には UPDI書換許可属性が、`BOOTSIZE`（あるいは`BOOTEND`）属性にはブートローダーサイズが含まれる。これらを破壊した場合の原状回復は困難になる。

> AVR-DU/EBで追加された`PDICFG`項目については特に、決して忘れることなく`FUSE_PDICFG_DEFAULT`初期値を指定しなければならない。これの意識的変更は __対象チップへの NVM書換を完全かつ永久に禁止__ する。つまり完全 ROM化に至らしめる。総じてこれは最終製品量産段階でのみ行われるべき操作だ。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
