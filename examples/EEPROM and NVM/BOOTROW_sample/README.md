# BOOTROW_sample.ino

このサンプルスケッチは以下について記述している；

- BOOTROW 領域の読み出しと書き換え

## 対象AVR

- modernAVR世代
  - AVR DU系統
  - AVR EB系統

ここで論じる機能は __本SDKに付属するブートローダーの使用__ を前提としている。他のSDKで用意されたブートローダを使用している、あるいはブートローダーを使用しない場合は全く当てはまらないことに注意されたい。

## \<FlashNVM.h\>

- [FlashNVM](https://github.com/askn37/askn37.github.io/wiki/FlashNVM) -- フラッシュメモリ自己書換支援ツール
[[place of folder]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/src)

このツールライブラリはフラッシュメモリ空間の書き換えを支援する。

__BOOTROW__ 領域は AVR_DU/EB で追加された新たな不揮発メモリで、__BOOTROW__ 領域 に類似するが以下の点で異なる。

- 外部からの UPDI制御と、`BOOTEND`（`FUSE_BOOTSIZE`）より下位の`BOOTCODE`内に実行コード位置（PC=プログラムカウンタ）がなければ、読むことも書くこともできない。
- 主にブートローダーの動作モード切り替えや、USBデバイス制御の初期値テーブル、プログラマブル波形生成器（シンセサイザー）のプリセット情報保存を主な目的として用意されている。

この __BOOTROW__ 読み書き機能は以下の対応ブートローダー（FWV=3.71以降）が、スケッチプログラムよりも前方に存在していることを前提に動作する。

- 対応ブートローダー；
  - [[Bootloaders for modernAVR]](https://github.com/askn37/multix-zinnia-sdk-modernAVR/tree/main/bootloaders)

> 他のSDK付属のブートローダーに例え同種の機能があっても、それには対応しないことに注意。

端的に言えばこれらのブートローダーがスケッチプログラムをフラッシュメモリ領域に書き込む能力を借り受けて使用するものだ。
当然のことながら、記憶されているスケッチプログラムそれ自体を
破壊したり消したりも容易に出来ることに注意されたい。

- この支援ツールの機能は *namespace* `FlashNVM`に属するメンバー関数として提供される。

## 用例

```c
#include <FlashNVM.h>

/* データ保存構造体 */
struct nvm_store_t {
  uint32_t count;
  char datetime[26];
  uint16_t magic;
} nvm_buffer;

/* BOOTROW から構造体に読み込む */
FlashNVM::bootrow_load(&nvm_buffer, sizeof(nvm_buffer));

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
  if ( FlashNVM::bootrow_clear()
    && FlashNVM::bootrow_save(&nvm_buffer, sizeof(nvm_buffer))
  )    Serial.println(F("[success]"));
  else Serial.println(F("*failed*"));
}
```

基本的には以下の操作フローとなる；

1. `FlashNVM::spm_support_check`で書き換え機能が使えるか確認する。
1. BOOTROW領域を SRAM上のデータバッファ（構造体など）にコピーする。
1. （任意）データバッファの CRCを計算／照合して検査する。
1. データバッファを任意に書き換える。
1. （任意）データバッファの CRCを再計算して更新する。
1. （任意）書き換え先の BOOTROW領域を消去する。
1. データバッファを書き換え先の BOOTROW領域に書き込んで永続保存させる。

> この操作フローは、USERROW領域書き換え（[UrowNVM](https://github.com/askn37/askn37.github.io/wiki/UrowNVM)）に準じている。

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
if ( FlashNVM::bootrow_clear() )
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
if ( FlashNVM::bootrow_save(&nvm_buffer, sizeof(nvm_buffer)) )
{ /* success */ }
```

> この関数の返値の真偽は NVMコントローラの動作結果で、真にそれが書き込めたかは示さない。

書込後は望み通りの結果になっているか、SRAMデータとの間で比較検証するのが良い。
失敗した場合はやりなおすか、他の対策を取るかなどする。

```c
/* 比較ベリファイ */
if ( FlashNVM::bootrow_verify(&nvm_buffer, sizeof(nvm_buffer)) )
{ /* success */ }
```

## NVM書換中の電源断対策

NVM書換操作は安定した電源供給が要求される。
動作中の電圧降下や変動は操作結果不定となる。
当然正しい保存データは失われるので、
以下の対策を考慮すべきだ。
これらは無論フラッシュメモリ寿命に達した際の対応にもなる。

- CRCなどの検査値を保持する
  - 該当データ内に含めても良いし、他のページにまとめて保持しても良い。
- 複数の正副保存領域を保持する
  - 当然倍以上の NVM領域を消費するが実装は比較的簡単。
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
