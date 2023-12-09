# SelfErase.ino

このサンプルスケッチは以下について記述している；

- フラッシュメモリ領域の自己消去

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統
  - AVR Ex系統

ここで論じる機能は __本SDKに付属するブートローダーの使用__ を前提としている。
他のSDKで用意されたブートローダを使用している、
あるいは使用しない場合は全く当てはまらないことに注意されたい。

## 自分自身を消去する

このスケッチプログラム例は、ビルドしてアップロードし、実行しても一見何も起こらない。
単にブートローダが、スケッチアップロードを催促するようLEDを明滅させるだけだ。
だが 128KiB品種などの大きなフラッシュメモリ量を持つ品種で試すと多少わかりやすいが、
幾ばくかの時間を消費して、ブートローダー以外のフラッシュメモリ空間を
スケッチ自身も含めて消去する動作をしている。

```c
#include <FlashNVM.h>

alignas(PROGMEM_PAGE_SIZE) const int _ro[] PROGMEM = {};

__attribute__ ((section (".init0")))
int main (void) {
  asm("CLR R1");
  pinMode(LED_BUILTIN, OUTPUT);
  uint32_t top = (uint32_t)_ro;
  for (;;) {
    top -= PROGMEM_PAGE_SIZE;
    FlashNVM::page_erase_PF(top);
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
}
```

最初の`_ro[]`領域宣言は`PROMGEM`中に、消去を開始する始点を定めている。
その中身は空で単にアドレスを確保しているに過ぎないが、
実はこれはプログラムメモリ中では次の`main`関数の開始位置に等しい。
そこで後の消去ループではこれが最後に消去されるページとなるように配慮している。

`main`関数には特殊な属性として`".init0"`セクションを指定している。
これは`RESET`割込ベクターの遷移先を示していて、
ブートローダー起動後の最初の遷移先を直接ここにするための指定だ。
これによって「C/C++言語」コンパイラが付加する諸々の初期化コードをスキップさせ、
`_ro[]`と`main`の物理メモリ位置が正しく一致するよう保証している。
ただし初期化コード実行はしないわけなので、本来それが担う最低限の初期化を
`asm`命令で補う必要はある。

あとは無限ループで自分自身が実行不可能になるまで粛々と
プログラムメモリ領域を`FlashNVM::page_erase_PF`関数で消去していくだけだ。
このループ処理は`main`関数先頭から64バイト以内に納まるよう書かれている。
フラッシュメモリ消去単位である`PROGMEM_PAGE_SIZE`の最小値が64バイトなので
`top`ポインタは増加方向でも減少方向でもどちらへ進めても構わない。
何れであってもアドレス指定がぐるりと一周して元に戻った時点で`main`関数位置を示す。
フラッシュメモリ消去後の`0xFFFF`を AVRは不正な命令語と解釈するので
プログラムカウンタはブートローダー先頭に再セットされ、
後はブートローダーの督促LED点滅が繰り返される動作に帰結する。

> ブートローダー領域は NVMコントローラから見て保護領域内にあるので消されない。\
\
> アドレス指定は32bit幅があるので一見 4GiB範囲を舐めるかのように見えるが
NVMコントローラのアドレス指定レジスタは
実際のフラッシュメモリ量を示せるだけのビット幅しか物理的に存在しておらず、
128KiB品種なら 17bit幅レジスタとしてラップアラウンドするので効率よく一巡する。

### 補足：tinyAVR への適用

tinyAVR系統のうちいくつかの品種では普通にビルドしたのでは正しく動作しない場合がある。これは目的の動作に不必要なベクターテーブルやライブラリスタートアップコードの埋め込みによって、肝心の動作ループ全体がページ境界に納まらなくなるためだ。

この場合は、ボードメニューの`Build API`->`Standard Library All Disable`を選択してビルドするとより小さく、確実に動作するバイナリが作成される。

これを可能にするには、前述のスケッチに以下の記述を追加する。

```diff
+ #define ENABLE_MACRO_API
+ #include <api/macro_api.h>
  #include <FlashNVM.h>

  alignas(PROGMEM_PAGE_SIZE) const int _ro[] PROGMEM = {};

+ __attribute__ ((used))
+ __attribute__ ((OS_main))
  __attribute__ ((section (".init0")))
  int main (void) {
```

冒頭は`Standard Library All Disable`によって無効化される`Macro_API`を改めて有効化し、`pinMode`や`digitalWrite`マクロを使用可能にする。そして除去されるスタートアップコードに代えて`main`が使われることを明らかとすべく、追加の属性を`main`に加えている。こうしてビルドされる出力コードは、概ね 60byte内外になる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
