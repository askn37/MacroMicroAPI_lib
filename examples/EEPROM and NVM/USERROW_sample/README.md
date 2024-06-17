# USERROW_sample.ino

このサンプルスケッチは以下について記述している；

- USERROW 領域の書き換え

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## \<UrowNVM.h\>

- [UrowNVM](https://github.com/askn37/askn37.github.io/wiki/UrowNVM) -- USERROW領域書き換え支援ツール

[[place of folder]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/src)

__USERROW__ 領域は、データ空間内にある（コード実行はできない）特別な 不揮発フラッシュメモリ保存領域だ。
この領域は応用コードのシリアル番号や工場出荷時調整値の保存を意図して用意されており、通常は外部装置を使用して静的に書き込み、応用コード中からは読み出し専用メモリとして使用するが、応用コード中からは通常のフラッシュメモリと同様の手順で書き換えることもできる。
このツールライブラリは USERROW の書き換えを簡便な方法で支援する。

- この支援ツールの機能は *namespace* `UrowNVM`に属するメンバー関数として提供される。
- 応用コードの実行位置（PC=プログラムカウンタ）が`CODEEND`以降の`APPDATA`に属している場合は、書き換えることはできない。
- 公的な消去可能回数保証は 1000回程度である。

## 用例

```c
#include <UrowNVM.h>

/* データ保存構造体 */
struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
} nvm_buffer;

/* USERROW から構造体に読み込む */
UrowNVM::userrow_load(&nvm_buffer, sizeof(nvm_buffer));

/* データが不正なら初期値を投入する */
if (nvm_buffer.magic != 0xABCD) {
  Serial.println(F("*reinit*"));
  nvm_buffer.count = ~0;
  nvm_buffer.magic = 0xABCD;
}

/* 各種メモリ操作 */
nvm_buffer.count++;
strcpy(nvm_buffer.datetime, _date_time_strings);

/* USERROW に保存する */
ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  if ( UrowNVM::userrow_clear()
    && UrowNVM::userrow_save(&nvm_buffer, sizeof(nvm_buffer))
  )    Serial.println(F("[success]"));
  else Serial.println(F("*failed*"));
}
```

基本的には以下の操作フローとなる；

1. USERROW領域を SRAM上のデータバッファ（構造体など）にコピーする。
1. （任意）データバッファの CRCを計算／照合して検査する。
1. データバッファを任意に書き換える。
1. （任意）データバッファの CRCを再計算して更新する。
1. （任意）書き換え先の USERROW領域を消去する。
1. データバッファを書き換え先の USERROW領域に書き込んで永続保存させる。

## 揮発メモリ領域への複写

NVM（不揮発メモリ）領域はページ粒度単位でしか書き換えることができない。ただ単に読んで利用するだけなら必ずしも必要ではないが、あとで書き戻すときのことを考えるとふつうはデータメモリ領域にページ内容を複写して以後はそちらで読み書きすることになる。

```c
/* SRAM上のバッファ確保 */
char nvm_buffer[PROGMEM_PAGE_SIZE];
/* NVMEM領域から SRAM にコピー */
UrowNVM::userrow_load(&nvm_buffer, sizeof(nvm_buffer));
```

### 不揮発メモリ領域の消去

NVM領域へデータを書き戻す前にその領域はふつう、消去操作で初期化しなければならない。
消去操作されたページ内容は`0xFF`で埋め尽くされる。

```c
/* ページ消去 */
if ( UrowNVM::userrow_clear() )
{ /* success */ }
```

> この関数の返値の真偽は NVMコントローラの動作結果で、真にそれが消去できたかは示さない。

というのも NVM書込操作は、既にそこにある値との論理和を結果として残すからだ。
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

前述のように NVM書込操作は現データとの論理和操作だ。

```c
/* ページ書込 */
if ( UrowNVM::userrow_save(&nvm_buffer, sizeof(nvm_buffer)) )
{ /* success */ }
```

> この関数の返値の真偽は NVMコントローラの動作結果で、真にそれが書き込めたかは示さない。

書込後は望み通りの結果になっているか、SRAMデータとの間で比較検証するのが良い。
失敗した場合はやりなおすか、他の対策を取るかなどする。

```c
/* 比較ベリファイ */
if ( UrowNVM::userrow_verify(&nvm_buffer, sizeof(nvm_buffer)) )
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

## megaAVR と tinyAVR 系統について

megaAVR と tinyAVR 系統 の USERROW 領域の実体は EEPROM 方式なので、1byte 粒度で消去と書き込みができる。
つまり `<avr/eeprom.h>` EEPROM 操作ライブラリでも書き換えができる。
しかし他の系統は FLASH 方式であって EEPROM のようには扱えないないため、本ツールライブラリでは共通インタフェースだけを提供している。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
