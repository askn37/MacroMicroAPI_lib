# NativeC.ino

このサンプルスケッチは以下について記述している；

- 標準C言語ライブラリ及びスタートアップコードを含まない純粋C言語記述。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Dx系統

## 前提

このスケッチは、以下のメニューオプションを選択した場合について説明している。

```c
Build API -> Standard Library All Disable
```

これは通常のC/C++言語動作に必要な前提を取り除くので、
通常の記述に従ったスケッチコード等は全くコンパイルできなくなるだろう。
これを必要とするのはごく限られた状況だけだ。

- 通常の割込ベクタテーブルを必要としない、あるいはその不要スペースをも削減したい最小コードの実現。
- ブートローダーやそれに準じる特別なプログラム。

しかしながら Arduino IDE では`make`コマンドによる自動化は支援されないため、大規模な記述は難しいだろう。

> C++のうちクラス関係は規定のスタートアップ初期化がないため使用できない。ほぼC言語記述専用となる。

## スケッチ.ino は使わない

Arduino IDE にプログラムを認識させるためには `.ino` ファイル（実体は`.cpp`）が必要だが、
これは使用しなくともよく中身は空としておけばよい。
代わりに任意の `.c` または `.S` ファイルの何処かに
スタートアップコードを記述し、それを動作開始点として認識させなければならない。

## スタートアップコード記述

動作開始点は一般に`main`関数として書くが、
名称は必ずしも`main`である必要はない。
単にプログラムコードの先頭にあるかどうかでその意味が決まる。
AVR-LIBC 環境でのこれはコードの配置位置が
`".initN"`セクションに属しているかどうかに依存する。

```c
__attribute__((used,naked,section(".init9")))
int main (void) {

  /* STUB */

  for (;;);
}
```

セクション名には`".init0"`から`".init9"`があり、この順で配置される。
割込ベクタを配置する場合はそれを飛び越すように（アセンブラで）書かねばならない。
割込を全く使わないのであれば、このように直接プログラム関数から始めても良い。
そしてこの関数から`return`で抜け出すようなことがあってはならない。
故に記述の最後は必ず無限ループになるだろう。

> 普通は`".init9"`としてよい。割込ベクタも書く場合は併せて`".init0"`からアセンブラで記述する。\
> `used`修飾子がないとこのmain関数は __何処からも参照されていないが故に削除__ されてしまうだろう。\
> また`main`のみは特別に`return`がなくとも`int`型としなければならない。他の名前の場合は`void`型とする。

## ゼロレジスタ

スタートアップ記述の第一は、ゼロレジスタの初期化である。
これがなければ以後のCで書かれた演算記述の前提条件が崩れ、正常な動作が損なわれる。
この初期化コードはアセンブラで直接書かれなければならないため、この1行が必要だ。
ただしこれ以後に C言語に依存する記述がないのであれば、省略しても支障ない場合もある。

```c
__asm__ __volatile__ ("CLR __zero_reg__");
```

> `__zero_reg__` マクロシンボルは普通 `r1` レジスタに解釈される。

## スタックポインタ

スタートアップ記述の第二は、スタックポインタの初期化である。
ただし（__tinyAVR-0__ 系統以降の）__modernAVR__ ではリセット時に自動でスタックポインタが
SRAM領域の末尾を示すようにセットされるため、必ずしも必要な記述ではない。
あえて記述するなら次のようになる。当然アセンブラ記述である。

```c
#include <avr/io.h>
__asm__ __volatile__ ( R"#ASM#(
  LDI     R16, lo8(%0)        ; SP初期化（下位）
  OUT     __SP_L__, R16       ;
  LDI     R16, hi8(%0)        ; SP初期化（上位）
  OUT     __SP_H__, R16       ;
)#ASM#" :: "n" (RAMEND));     // SP初期値定数 <avr/io.h>で定義
```

スタックポインタの初期化定数 `RAMEND` は `<avr/io.h>` を読むことで知ることができる。

> RAMENDから敢えて減じて固定アドレスヒープ領域を確保することもできる。\
> このアイデアは`malloc`代替の一種として使われる。
> 普通のC/C++でそれをする場合は`".init3"`に書くことが多い。

## CPUクロック設定

CPUクロックの初期状態は `F_CPU` マクロ定数と一致しているとは限らない。
このマクロは`delay`マクロなどが参照するため、それらを使うのであれば
データシートを参照して正しい値に再定義するか、
あるいは初期化コードを書いて CPUクロックの方を合致させるようにする。
この初期化コードは、このSDKでは次のヘッダファイルと定義済関数を使えば簡単に書ける。

```c
#include <variant.h>
_CLKCTRL_SETUP();
```

## 便利マクロ

SDKに付属の Macro API の一部は、この純粋C言語形態でも使用できる。

- `pinModeMacro` `digitalWriteMacro` `digitalReadMacro` etc.
- `delay_millis` `delay_maicros` etc.

> delay系関数の使用は F_CPU の正しい事前定義、ひいてはCPUクロック設定作業が必須。

従って Lチカは次のように書き下せる。

```c
#include <api/macro_digital.h> /* connotation <io/avr.h> <variant.h> */
#include <api/delay_busywait.h> /* using F_CPU */
__attribute__((used,naked,section(".init9")))
int main (void) {
  __asm__ __volatile__ ("CLR __zero_reg__");
  _CLKCTRL_SETUP();
  pinModeMacro(LED_BUILTIN, OUTPUT);
  while (1) { /* true と false を使うには <stdbool.h> */
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay_millis(1000);
  }
}
```

AVR-LIBC付属ライブラリ中のいくつかは純粋C言語形態でも利用できる場合がある。

- 機能する例：`<avr/io.h>` `<util/atomic.h>`（avrとutil以下の多くは使える可能性がある）
- 機能しない例：`<stdio.h>` `<stdlib.h>`
`<avr/interrupt.h>`（割込ベクタ配置は要アセンブラ補助）

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
