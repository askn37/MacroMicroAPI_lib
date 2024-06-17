# Import_BinFile.ino

このサンプルスケッチは以下について記述している；

- 外部バイナリファイルのインポート
  - 外部バイナリファイル内の範囲指定部分インポート

> この機能は `<api/macro_api.h>` の一部である。\
> `ENABLE_MACRO_API` マクロが未宣言では組み込まれない。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## 記述例

```c
#include <api/macro_api.h>
#include <avr/pgmspace.h>

/* FLASHページ境界に次の定数領域要素を配置する */
// PAGE_ALIGN(".progmem");

/* 定数領域のシンボル FooBin に foo.bin を配置し参照できるようにする */
IMPORT_BINFILE(".progmem", FooBin, __QUOTE(SKETCH_PATH) "/" "foo.bin");

/* C言語へのシンボル割付（型は任意） */
extern const uint8_t FooBin[], FooBin_end[] PROGMEM;

/* 量数 */
const size_t _sizeof_FooBin = FooBin_end - FooBin;

/* .data 領域への読込の場合（SRAM空間） */ /* 部分読込 */
IMPORT_BINFILE_PART(".data", FooBinDat, __QUOTE(SKETCH_PATH) "/" "foo.bin", 0x2c, 0x30);
extern const uint8_t FooBinDat[], FooBinDat_end[];
const size_t _sizeof_FooBinDat = (FooBinDat_end - FooBinDat);

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* PROGMEM空間の場合 */
  Serial.print(F("FooBin_ADDR=0x")).println(pgm_get_far_address(FooBin), HEX);
  Serial.print(F("sizeof=")).println(_sizeof_FooBin, DEC);
  Serial.printDump(P(FooBin), _sizeof_FooBin, true).println(F("[End]"));

  /* SRAM空間の場合 */
  Serial.print(F("FooBinDat_ADDR=0x")).println((uint16_t)&FooBinDat, HEX);
  Serial.print(F("sizeof=")).println(_sizeof_FooBinDat, DEC);
  Serial.printDump(FooBinDat, _sizeof_FooBinDat, true).println(F("[End]"));
}
```

## 解説

プリプロセッサ命令の `#include` はテキストファイルを該当位置に読み込むことしか出来ない。
かつ読まれた先の書式（CやC++）に左右されるため、あらゆる種類のオブジェクトをそこに配置できるわけでもない。
従って次のようなファイルを読み、オブジェクトを割り当てるのは容易なことではない。

- フォントイメージデータバイナリ
- デバイスアップデート用ファームウェアバイナリ
- センサーデバイス校正データバイナリ
- 構築済ハッシュ辞書データバイナリ
- メモリカード用フォーマット済初期化イメージバイナリ

`IMPORT_BINFILE`マクロはこの問題の解決策を支援する。
これはインラインアセンブリ命令で記述されており、
アセンブラの機能である `.incbin` 疑似命令を用いて外部データファイルを読み込み、
`.global` 疑似命令で 参照シンボルを割り付ける。

`
IMPORT_BINFILE("配置セクション", <割当シンボル>, "ファイルパス");
`

1. 配置セクションは普通 `".progmem"` か `".data"` を指定する。
`".data"` のほうが扱いは楽だが、同量のSRAMをワークとして消費するため AVRではあまり推奨できない。
`".progmem"` を選択した場合は `<avr/pgmspace.h>` ユーティリティを使わないと
読み出すことは出来ないが、フラッシュメモリ容量以外の制限はない。
2. <割当シンボル>は読み込まれたオブジェクトにアセンブリシンボルとしての参照名をつける。
3. ファイルパスには読み込むファイルのディスク上のパスを指定する。
普通これは絶対パスで書く。相対パスで指定すると何処が起点になるかわからない。
スケッチ記述フォルダのパスが `SKETCH_PATH` マクロに保持されているのでこれを使うとよい。
ただしこのマクロの中身はダブルクォートされていないので、クォート変換マクロを通じて記述する。

```c
__QUOTE(SKETCH_PATH) "/" "foo.bin"
```

> Arduino IDEは、スケッチフォルダ内の `.c .cpp .h .S .ino` ファイル以外を
ビルド作業領域に取り込まない。そのためこれら以外のファイルは絶対パスがないと読ませることが出来ない。

こうして読み込まれたオブジェクトを C/C++言語から認識させるには
`extern` 命令で型定義と属性を付加する。
先に指定した `<割当シンボル>` がオブジェクトの先頭番地、
`<割当シンボル>_end` がオブジェクト終了番地（の次）を指している。
型はアドレスとして見えればどんなものであっても構わない。
普通は char 型か uint8_t 型配列となるようにする。
オブジェクトの大きさは `<割当シンボル>_end - <割当シンボル>` で知ることができる。
（sizeof では参照できない）

```c
/* C言語へのシンボル割付（型は任意） */
extern const uint8_t FooBin[], FooBin_end[] PROGMEM;

/* 量数 */
const size_t _sizeof_FooBin = FooBin_end - FooBin;
```

`".data"` 領域に読み込んだ場合は、このシンボルと型宣言を通して普通に配列として読むことができる。
`".progmem"` 領域に読み込んだ場合は `<avr/pgmspace.h>` ユーティリティを使用して読むことができる。

`IMPORT_BINFILE_PART` マクロは前述の派生で、
ファイル先頭からの位置と、そこからの読込量を指定できるように拡張したものだ。

```c
IMPORT_BINFILE_PART("配置セクション", <割当シンボル>, "ファイルパス", ファイル先頭から読み飛ばすオフセットバイト量, そこから読み込むバイト量);
```

AVR-GCC では配列の最大サイズ/最大要素数が 32767（INT_MAX）に制限されている制約がある。
つまり 32KiB を超えるサイズのオブジェクトを作ろうとしても作れない。
この制約を避けるには 32767 byte 以下の複数のオブジェクトに分割して扱うしかないので
部分読込マクロを使う必要がある。

## その他の関連マクロ

### PAGE_ALIGN(".progmem")

このマクロは後続する指定セクションの要素の配置を、コンパイル対象のフラッシュメモリ粒度に一致するページ境界に合わせる。
フラッシュメモリ粒度は消去/書換が可能な最小単位で、AVR では 64、128、256、512 の何れかであり MCU依存である。
これは自己フラッシュ書換機能を使用して事後書換をするような場合に必要となる。

### pgm_get_far_address(var)

指定した要素の MCU上での物理メモリ絶対番地を返す。
プログラム領域は 0x000000 から 0x7FFFFF の 23bit（バイト）アドレス、
SRAM領域は 0x800000 以上にオフセットされた 24bit（バイト）アドレスで返る。
この定義はリンカエディタでのアドレス管理に一致する。
これは（アセンブラでの）SPM/LPM/ELPM命令で使われる拡張アドレスレジスタ設定にも使える。

> このマクロは `<avr/pgmspace.h>` で定義されているもので、
接尾子`_FP`を持つプログラム領域用拡張ユーティリティ関数の引数用に準備されている。

### F("...")

指定の文字列をプログラム領域に配置し、その番地と属性を返すマクロ。
`print` `printf` `write` 関数等の対応関数/メソッドで使用できる。

### P(var)

64KiB範囲内用の プログラム領域指定マクロ。
これは `F("文字列")`定義マクロが指定できる場所で、
直接文字列以外の任意要素を間接指定するために使うことができる。

> 128KiBフラッシュメモリ品種の、後半領域を指定できないことに注意。
リンクエディタは普通この問題を避けるために `".progmem"` セクションを
出力ファイルの先頭付近に集め `".text"` セクションより前方に配置する。
それでもアクセスできない領域に配置されてしまったデータは
`pgm_get_far_address` マクロに対応した関数（_PF群）でしか扱えない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
