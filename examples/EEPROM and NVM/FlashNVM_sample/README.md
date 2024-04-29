# FlashNVM_sample.ino

このサンプルスケッチは以下について記述している；

- フラッシュメモリ領域の読み出し・自己書き換え

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
    - ATtiny202/402 はメモリ不足により除く
- modernAVR世代
  - AVR Dx系統
  - AVR Ex系統

ここで論じる機能は __本SDKに付属するブートローダーの使用__ を前提としている。
他のSDKで用意されたブートローダを使用している、
あるいは使用しない場合は全く当てはまらないことに注意されたい。

## \<FlashNVM.h\>

- [FlashNVM](https://github.com/askn37/askn37.github.io/wiki/FlashNVM) -- フラッシュメモリ自己書換支援ツール
[[place of folder]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/src)

これはスケッチプログラムを書き込んだ後の残りの、未使用フラッシュメモリ領域を
スケッチの中から書換可能な不揮発記憶領域として扱うことを支援するツールライブラリだ。
フラッシュメモリ領域は EEPROM 領域に比して数桁大きなまとまった容量を扱える一方、
書換可能限度回数は1桁以上低い。（一般に消去保証回数は 1000回程度）

この機能は以下の対応ブートローダー（FWV=3.71以降）が、スケッチプログラムよりも前方に存在していることを前提に動作する。

- 対応ブートローダー；
  - [[Bootloaders for modernAVR]](https://github.com/askn37/multix-zinnia-sdk-modernAVR/tree/main/bootloaders)
  - [[Bootloaders for megaAVR / tinyAVR]](https://github.com/askn37/multix-zinnia-sdk-megaAVR/tree/main/bootloaders)

> 他のSDK付属のブートローダーに例え同種の機能があっても、それには対応しないことに注意。

端的に言えばこれらのブートローダーがスケッチプログラムをフラッシュメモリ領域に書き込む能力を借り受けて使用するものだ。
当然のことながら、記憶されているスケッチプログラムそれ自体を
破壊したり消したりも容易に出来ることに注意されたい。

- この支援ツールの機能は *namespace* `FlashNVM`に属するメンバー関数として提供される。

## 用例

```c
#include <FlashNVM.h>

/* NVMEN領域に1ページ分のスペースを確保 */
const char nvm_store[PROGMEM_PAGE_SIZE] PGM_ALIGN NVMEM;

/* SRAM上のバッファ確保 */
char nvm_buffer[PROGMEM_PAGE_SIZE];

/* 機能サポート確認 */
if (! FlashNVM::spm_support_check()) {
  /* FlashNVM tools not supported */
  return;
}

/* <avr/pgmspace.h> の関数を使って */
/* NVMEM領域から SRAM にコピー */
memcpy_PF(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer));

/* nvm_storeバッファメモリを適当に書換えて。。。 */

/* ページ消去 */
if ( FlashNVM::page_erase_PF( pgm_get_far_address(nvm_store), sizeof(nvm_buffer) ) )
{ /* success */ }

/* ページ書込 */
if ( FlashNVM::page_update_PF( pgm_get_far_address(nvm_store), &nvm_buffer, sizeof(nvm_buffer) ) )
{ /* success */ }
```

基本的には以下の操作フローとなる；

1. プログラムメモリ領域に書き換え保存用のNVMページを確保する。
1. `FlashNVM::spm_support_check`で書き換え機能が使えるか確認する。
1. 確保したNVMページを SRAM上のデータバッファにコピーする。
1. （任意）データバッファの CRCを計算／照合して検査する。
1. データバッファを任意に書き換える。
1. （任意）データバッファの CRCを再計算して更新する。
1. （任意）書き換え先の NVMページを消去する。
1. データバッファを書き換え先の NVMページに書き込んで永続保存させる。

## 保存領域初期化

領域初期化は`NVMEM`か`ROMEM`属性で初期化子反映の有無を決め
`PGM_ALIGN`でページ境界に整列させて置かなければならない。

```c
const char nvm_store[PROGMEM_PAGE_SIZE] PGM_ALIGN NVMEM;
```

```c
const char nvm_store[] PGM_ALIGN ROMEM = "Hello World!";
```

`NVMEM`の場合、初期化子を書いても無視される。
`ROMEM`の場合、スケッチ書込の度に初期化子で指定した状態に最初期化されて元に戻る。
両者は用途に応じて使い分けるが何れにせよ`PGM_ALIGN`属性は必要だ。

原則として`PROGMEM_PAGE_SIZE`が示す1ページサイズ内にひとつの要素を置く。
複数の要素を1ページに詰め込みたい場合は構造体（セクター）を定義して使うのが良い。

`PAGE_ALIGN`マクロを使用すると境界パディングをせず密に詰め込めるが、
それら複数要素を一度に全部書き換えねばならないことと同義なので、
自己責任でのメモリ管理は煩雑になる。

```c
PAGE_ALIGN(".nvmem");
const char nvm_store0[4] NVMEM;
/* ここに隙間は空かず密に配置される */
const char nvm_store1[4] NVMEM;
```

## 書換機能確認

`FlashNVM::spm_support_check`は NVM自己書換機能を
ブートローダーが支援していれば真を返す。
そうでなければ以後の説明は用を為さないので、プログラム進行を止めるべきだ。

```c
/* 機能サポート確認 */
if (! FlashNVM::spm_support_check()) {
  /* FlashNVM tools not supported */
  return;
}
```

## 揮発メモリ領域への複写

NVM（不揮発メモリ）領域はページ粒度単位でしか書き換えることができない。
ただ単に読んで利用するだけなら必ずしも必要ではないが、
あとで書き戻すときのことを考えると（そして64KiB超空間のことも考慮すると）
ふつうはデータメモリ領域にページ内容を複写して
以後はそちらで読み書きすることになる。

```c
/* SRAM上のバッファ確保 */
char nvm_buffer[PROGMEM_PAGE_SIZE];
/* NVMEM領域から SRAM にコピー */
memcpy_PF(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer));
```

`NVMEM`領域を指す物理アドレスは、普通の`int`型ポインタに納まるとは限らない。
フラッシュメモリ量が 64KiBまでの品種は`int`型だが
128KiB品種の後半では 17bit幅の物理アドレスを割り当てられるからだ。

`pgm_get_far_address`マクロは指定要素のアドレスを
32bit変数に返却する。その拡張アドレスを認識できるプログラム領域操作関数には
普通`_PF`接尾子が付いている。
一方`_P`接尾子の操作関数は 64KiB以下空間用途に限られるが、
普通のポインタ、`void*`汎用ポインタ、`P()`マクロ等が使用できる。
これらはメモリ構造設計によって使い分けることになるが、
とりあえず`pgm_get_far_addressマクロ＋PF関数`を使っておけば間違いはない。

### 不揮発メモリ領域の消去

`NVMEM`領域へデータを書き戻す前にその領域はふつう、消去操作で初期化しなければならない。
消去操作されたページ内容は`0xFF`で埋め尽くされる。

```c
/* ページ消去 */
if ( FlashNVM::page_erase_PF( pgm_get_far_address(nvm_store), sizeof(nvm_buffer) ) )
{ /* success */ }
```

> この関数の返値の真偽は NVMコントローラの動作結果で、真にそれが消去できたかは示さない。

というのも`NVMEM`書込操作は、既にそこにある値との論理和を結果として残すからだ。
「書き換え保存」を意図しているなら、そこにある値が必ず`0xFF`になっていなければならない。
CRC検査値付きで保存するなら、消去操作は必須であろう。

```plain
例えば：旧値 0xFF AND 新値 0x5A → 結果 0x5A
例えば：旧値 0x6C AND 新値 0x5A → 結果 0x48
```

> 意図的にこの性質を使う応用もあるので、消去と書込は別操作になっている。\
> 更に、寿命に達したフラッシュメモリは消去しても`0xFF`にならず、
それに書き込んでも`0x00`にできないような現象が発生するようになる。

### 不揮発メモリ領域への書込

前述のように`NVMEM`書込操作は現データとの論理和操作だ。

```c
/* ページ書込 */
if ( FlashNVM::page_update_PF( pgm_get_far_address(nvm_store), &nvm_buffer, sizeof(nvm_buffer) ) )
{ /* success */ }
```

> この関数の返値の真偽は NVMコントローラの動作結果で、真にそれが書き込めたかは示さない。

書込後は望み通りの結果になっているか、SRAMデータとの間で比較検証するのが良い。
失敗した場合はやりなおすか、他のページにコピーを取るかなどする。

```c
/* 比較ベリファイ */
if ( 0 == memcmp_P(&nvm_buffer, pgm_get_far_address(nvm_store), sizeof(nvm_buffer)) )
{ /* success */ }
```

## NVM書換中の電源断対策

NVM書換操作は安定した電源供給が要求される。
動作中の電圧降下や変動は操作結果不定となる。
当然正しい保存データは失われるので、
以下の対策を考慮すべきだ。
これらは無論フラッシュメモリ寿命に達した際の対応にもなる。

- CRCなどの検査値を保持する
  - 該当データ内に含めても良いし、他のページにまとめて保持しても良い。一般的には EEPROM に CRC値を保持する。
- 複数の正副保存領域を保持する
  - 当然倍以上の NVM領域を消費するが実装は比較的簡単。
  - ただし BOOTROW / USERROW は全体がひとつの消去単位であるために、この手法は適さない。
- SRAMデータバッファを疑似不揮発メモリにする
  - 二重保存の副保存領域にSRAMを使う。
[[SRAMによる疑似不揮発メモリ実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/EEPROM%20and%20NVM/SRAM_persistent)
も参照のこと。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
