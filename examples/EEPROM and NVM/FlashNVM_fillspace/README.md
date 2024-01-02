# FlashNVM_fillspace.ino

このサンプルスケッチは以下について記述している；

- フラッシュメモリ領域の読み出し・自己書き換え

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

## 最大フラッシュメモリ量確保試験

このスケッチは
[[フラッシュメモリ領域を読み書きする実演その1]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/EEPROM%20and%20NVM/FlashNVM_sample)
の変種で、単に`ROMEM`初期領域をフラッシュメモリ領域全域に拡大しただけのものだ。
つまり 16KiB品種なら 16KiB弱の、128KiB品種なら 128KiB弱の、
スケッチアップロード可能な最大サイズの出力 HEX/binファイルが得られるように書かれている。
スケッチプログラムの動作よりもブートローダーやプログラマライターの
性能評価のほうが主たる目的だ。

## AVR-GCCの制約

このスケッチプログラムは少し奇妙な記述をしている。
例えばNVM領域確保用の配列を宣言している部分がある。

```c
const struct nvm_store_t nvm_store0[NVM_STORE0] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store1[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store2[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store3[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
const struct nvm_store_t nvm_store4[NVM_STORE1] PGM_ALIGN NVM = { {0, __DATE__ " " __TIME__, 0xABCD} };
```

これは「C/C++言語」ではひとつの配列に付き32767バイトまでしか宣言できないので、
ひとつの大きなまとまった配列として宣言することができず、
やむなく複数の領域に分割せざるを得ないがためだ。
端的には`size_t`型が`int`に等価であることに由来する。
「8bit AVR」が「8bit CPU」であることを根拠に、
「C/C++言語」設計の時点でバイアスが掛かっているのである。

おなじ`int`型制約は関数ポインタや`void*`汎用ポインタにも課せられている。
これらの方はより事態が深刻で、AVRが 8MiBのメモリ空間を持っているにも関わらず
C/C++言語ポインタ指定は 64KiBしか素直に指し示せない枷になっている。
真にリニアなメモリ空間にアクセスするには、
そういう制約がないアセンブリ言語の補助が必要不可欠だ。

もうひとつ、次のような記述がある。
これは単に前述の確保領域の先頭アドレスをプリントしているに過ぎないが、
これらを試しにコメントアウトしてビルドして見てほしい。
生成されるスケッチサイズが激減することを確認できるはずだ。

```c
Serial.print(F(" nvm_store0=0x")).println(pgm_get_far_address(nvm_store0), HEX);
Serial.print(F(" nvm_store1=0x")).println(pgm_get_far_address(nvm_store1), HEX);
Serial.print(F(" nvm_store2=0x")).println(pgm_get_far_address(nvm_store2), HEX);
Serial.print(F(" nvm_store3=0x")).println(pgm_get_far_address(nvm_store3), HEX);
Serial.print(F(" nvm_store4=0x")).println(pgm_get_far_address(nvm_store4), HEX);
```

これは、確保領域の宣言を他で参照していないがためにデッドコードとなり、
コンパイル最適化の過程で不要と判定されて削除されてしまうことによる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
