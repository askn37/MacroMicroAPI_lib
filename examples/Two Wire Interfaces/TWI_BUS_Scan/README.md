# TWI_BUS_Scan.ino

このサンプルスケッチは以下について記述している；

- TWI/I2Cバス捜索の実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 用例

この例は、組込TWI周辺機能を使ってそのI2Cバス上に存在する従装置の制御アドレスを調べ上げる実演だ。

```c
#include <peripheral.h>

Wire.initiate(TWI_SM);
for (uint8_t _addr = 8; _addr < 0x78; _addr++) {
  if (Wire.scan(_addr)) {
    /* client address found ! */
  }
}
Wire.end();
```

I2Cバス規約に則り、捜索する7bitアドレス範囲は`0x08`から`0x77`としている。
その他のアドレス指定`0x00-0x07`と`0x78-0x7F`は
予約値や制御用や 10bit従装置アドレス指定用であって、
I2C用7bit従装置アドレスとしては使わるものではない。

`Wire.scan`メソッドは指定したアドレスの従装置が`ACK`ビットを返すかどうかだけを調べて真か偽を返す。
データ送受信は発生しない。しかし休止状態にあるSMBus規約装置を活動状態にする可能性はある。

> SMBus規約はI2Cバスと概ね互換性があるのでI2C主装置から通信可能である。
主な違いは最大速度が 100kbps ないし 200kbpsであることと、
PECと呼ばれるCRC検査符号を持つデータペイロードが定められていることだ。
一方でI2Cバスは（設計依存ではあるが）最低速度 0bps が許される。

## I2Cアドレス表現の混乱

周辺機器のデータシートを見ると、時折 8bitでの従装置アドレス表現が散見されて紛らわしいことがある。
これはI2Cバスデータ通信では`MSB先行左詰め`でビット列が並ぶことからの誤解や思い違いが原因だろう。
コード上でもアドレス表現は8bit左詰めに変換して書かれるので、それをそのまま転記したのかも知れない。

> USARTやSPIでは普通`LSB先行右詰め`の「逆順」に、ビットを表す信号がデータバス上に並ぶ。

はじめから 7bit表現であるなら曖昧なところは全く無いのだが、
記載者が 8bit表現で表している場合は注意深く読まねばならない。
「書込アドレス」と「読込アドレス」を併記している場合はまだ親切な方だ。
「書込アドレス」は必ず偶数「読込アドレス」は「書込アドレス+1」で必ず隣接しているし、
そうでなければ異常である。

```plain
Write Address : 0xA4
Read  Address : 0xA5

--> 7bit address : 0x52
```

説明が不足していてどちらだかわからない場合は、バス信号シーケンス図を探して読み解く。
アドレス記載部を見つけて左から 7bitを数えれば、それがI2C従装置アドレスだ。
それすら添付されていない場合は実際に動いているプログラムのソースコードを探したり、
「I2Cバス捜索」で実際の従装置応答を確認するしかない。

....

このサンプルスケッチは、まさしくそういう調査用途に書かれた。
実用上の要請から、見つかったアドレスに対する周辺機器の用途や型番も判る範囲で
プリントアウトするようにしている。
もっともこの対応表を完全になるまで拡充することは不可能なので、
よく見かけるものについてだけだ。
従装置アドレス変更機能を持つ品種については、考慮のしようもない。

> 例えば`0x70-0x77`はI2CHUBチップ`PCA9548A`の設定変更可能範囲だが、
この範囲には他のよくある環境センサーIC類`BME280`や`QMP6988`等がずらりと並ぶ。\
> またNXP社の`RTC`チップ類は互換性のない別型番であっても一意に同一の
`0x51`で応答するから、アドレスが解ったからといって正しく制御できるとは全く限らない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
