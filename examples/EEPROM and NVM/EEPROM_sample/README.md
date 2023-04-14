# EEPROM_sample.ino

このサンプルスケッチは以下について記述している；

- EEPROM領域の初期化・読み出し・書き換え

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統
  - AVR Ex系統 (後述)

## EEPROM領域へのアクセス

`EEPROM`領域の空間は、AVRでは普通のデータ領域の一部に割り付けられている。
しかしそれは SRAMではないので、読むことはできても書き換えるには特別な手順を要する。
AVR-LIBCでは`<avr/eeprom.h>`にそれらのユーティリティ関数が纏められている。
`EEPROM`領域にアクセスするには
`<avr/eeprom.h>`をインクルードし、
要素には`EEMEM`属性を付加する。

### EEPROM関係定義

`<avr/io.h>`をインクルードすると、以下の定数が使えるようになる。

|宣言名|説明|備考
|-|-|-|
|EEPROM_START|EEPROM開始番地|データ番地 0x1400
|MAPPED_PROGMEM_START|同
|EEPROM_SIZE|EEPROM総量|型番依存
|EEPROM_PAGE_SIZE|EEPROMページ粒度|型番依存
|E2PAGESIZE|同
|EEPROM_END|EEPROM終了番地|EEPROM_START + EEPROM_SIZE - 1
|E2END|同

この中で重要なのは`EEPROM_SIZE`だろう。これは型番によって異なる。
__modernAVR__ / __megaAVR__ / __tinyAVR__ の場合これは
`(64|128|256|512)`の何れかだ。
この総量を超えて`EEPROM`領域を書き換えることはできない。

以下に全型番のEEPROM総量のリストを挙げる。

|総量|型番
|-|-|
|64|ATtiny202/204 ATtiny212/214
|128|ATtiny402/404/406/804/806/807 ATtiny412/414/416/417/814/816/817 ATtiny424/426/427/824/826/827
|256|ATtiny1604/1606/1607 ATtiny1614/1616/1617 ATtiny1624/1626/1627 ATmega808/809/1608/1609
|256|ATtiny3216/3217 ATtiny3224/3226/3227 ATmega1608/1609/3208/3209/4808/4809
|256|AVR_DDnn
|512|AVR_DAnn AVR_DBnn
|512|AVR_EAnn

こられら全ての型番で`EEPROM_START`は`0x1400`を返す。
この値は当然、それ以前の旧世代AVRとは全く異なる。

`EEPROM_PAGE_SIZE`は`EEPROM`を「ブロック消去」する際の最小単位だが、
これは`<avr/eeprom.h>`ユーティリティを使う限り関係しないので、
ここでは特に言及しない。

## EEPROMの静的初期化

```c
#include <avr/eeprom.h>

char _date_time[] EEMEM = __DATE__ " " __TIME__;

uint32_t _reset_count EEMEM = 0;
```

`EEMEM`属性であることを除いて、その宣言は普通のデータ領域要素と変わらない。
型は任意に指定できる。だが実用上は任意の構造体を宣言して割り付けることになるだろう。

`EEMEM`要素には初期化子を与え、静的初期値を与えることが出来る。
しかしこれは実際には普通、スケッチ書込時には反映されない。
スケッチ書込と同時に`EEPROM`へ初期化子を反映させるには
以下の条件を満たしていなければならない。

- `EEPROM`書換に対応した書込機を使用しているか、
`EEPROM`書換に対応したブートローダーが既に導入されている。
- IDEの`ツール`サブメニューで以下の選択をしている。
  - `FUSE EEPROM`->`"Erase" and "Replace"`

> 例えば`DxCore`付属のブートローダーは`EEPROM`書換に対応していない。

これは`EEPROM`領域がスケッチ等の応用コードが任意に書き換え、
特有の情報を保存しておくことを前提にしていることによる。
スケッチ書込の度に毎回再初期化されるのでは都合の悪いことが普通だからだ。

## EEPROM領域の読み出し

`EEPROM`の読み出しは、
__modernAVR__ / __megaAVR__ / __tinyAVR__ 系統においては
普通のデータ領域と同じに直接読み出せる。
しかしながら旧世代からの慣例により`<avr/eeprom.h>`が用意する
アクセス関数を使用して、SRAMデータ領域にコピーして取り扱うほうが望ましい。
そのほうが後で`EEPROM`を書き換えるのに叙述が首尾一貫するからだ。

```c
/* 構造体にブロック読み出し */
struct_t _block;
eeprom_read_block(&_block, &_eeprom_block, sizeof(struct_t));

/* dword 読み出し */
uint32_t _dword = eeprom_read_dword(&_eeprom_dword);
/* 以下同様 */
uint16_t _word = eeprom_read_word(&_eeprom_word);
uint8_t _byte = eeprom_read_byte(&_eeprom_byte);
float _float = eeprom_read_float(&_eeprom_float);
```

## EEPROM領域への書き出し

`EEPROM`への書込関数には`update`系列と`write`系列があるが、
普通は`update`系列を使用する。
これは現在の`EEPROM`内容と比較して、変化するバイト部分だけを実際に書き換える。
そのぶん実行速度は低下するが、そうすることによって無条件書込を行う`write`系列よりも
`EEPROM`寿命の損耗を抑えることが期待できるからだ。

```c
/* 構造体をブロック更新 : strcpy() とは引数順序が異なることに注意 */
eeprom_update_block(&_block, &_eeprom_block, sizeof(struct_t));

/* dword 更新 */
eeprom_update_dword(&_eeprom_dword, _dword);
/* 以下同様 */
eeprom_update_word(&_eeprom_word, _word);
eeprom_update_byte(&_eeprom_byte, _byte);
eeprom_update_float(&_eeprom_float, _float);
```

## USERROW

新世代AVRは`EEPROM`の他に`USERROW`あるいは`USER_SIGNATURES`と呼ばれる
別の拡張`EEPROM`領域を持っている。
これは`EEPROM`と同じに扱うことが出来るが、
スケッチ書込での初期化や、スケッチ内での書き換えは想定されていない。
その用途は製品シリアル番号や、工場出荷初期設定の保持だ。
書き換えや初期化には原則として`USERROW`に対応した書込器を使用する。

```c
/* USERROWアドレス割り付け例 : USER_SIGNATURES_SIZE は使用可能な最大サイズ */
/* 短縮属性表現は用意されていない */
uint8_t _userrow[USER_SIGNATURES_SIZE] __attribute__((section(".user_signatures")));
```

`tinyAVR`および`megaAVR`系統での`USERROW`の物理特性は`EEPROM`なので、
`eeprom_update_*`でスケッチ内から自己書換することは可能だ。
しかし`AVR_Dx`および`AVR_Ex`系統での`USERROW`の物理特性は`Flash`なので、
既定では自己書換の手段は用意されていない。
物理特性が異なるので当然`NVMCTRL`の制御方法も異なる。

> かつ`AVR_Dx`および`AVR_Ex`とでは`NVMCTRL`のバージョンが異なり、制御方法も異なる。

## サンプルスケッチ

```c
#include <avr/eeprom.h>
char _date_time[] EEMEM = __DATE__ " " __TIME__;
uint32_t _reset_count EEMEM = 0;
void setup (void) {
  Serial.begin(CONSOLE_BAUD);
  if (*((char*)&_date_time + EEPROM_START) == -1) {
    Serial.println(F("<noinit>"));
  }
  uint32_t _count = eeprom_read_dword(&_reset_count);
  Serial.print(F("COUNT=")).println(_count++, DEC);
  eeprom_update_dword(&_reset_count, _count);
  Serial.flush();
}
```

`EEPROM`に適当な初期化子と、インクリメントカウンタを永続保持させる例を示す。
`EEPROM`領域は工場出荷時のまま未使用であれば（あるいは消去済であれば）
`0xFF`で埋め尽くされているので、その場合は`"<noinit>"`をプリントする。
その後は再起動回数カウンタを毎回インクリメントして更新保存している。

> `EEPROM`領域を消去して工場出荷状態に戻すには、
IDEの`ツール`サブメニューで`FUSE EEPROM`->`Save guard "Erase"`を選び、
書込機でブートローダーを書き込むか、適当なスケッチを書き込む。\
> あるいは`EEPROM_SIZE`全量を`0xFF`で埋め尽くしたブロックを
`eeprom_write_block`で書き込むスケッチを記述して実行する。

## AVR EA系統での挙動

`AVR EA`系統のAVR-LIBCデバイスファイルパックは、2023-03-22 時点では`<avr/eeprom.h>`に問題があって、EEPROMの書換が動作していない。従ってこの応用記述も正常動作しない。具体的には`NVMCTRL`に発行する指令が`AVR EA`の値になっていない。
（megaAVR-0世代の値になっている）

> http://packs.download.atmel.com/Atmel.AVR-Ex_DFP.2.3.163.atpack (2023-03-22)


## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
