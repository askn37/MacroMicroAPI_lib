# XRTC_12HourMode.ino

このサンプルスケッチは以下について記述している；

- 12時間表示モード時刻の実演（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`を「12時間表示モード」にした場合のデモンストレーションだ。
「12時間表示」では`00`時は存在せず午前午後とも`12`時と表現される。
そして「24時間表示」での`20`時に位置するビットが`午前`／`午後`を示すようになる。
この結果パックドBCD表現内容の連続性は失われる。

```plain
<startup>
F_CPU=24000000
[Demonstration Wraparound 12-Hours]
Date: 20000101  Week: 6  Time: pm 115958 (24h:235958)
Date: 20000101  Week: 6  Time: pm 115959 (24h:235959)
Date: 20000102  Week: 0  Time: am 120000 (24h:000000)
Date: 20000102  Week: 0  Time: am 120001 (24h:000001)
---
Date: 20000102  Week: 0  Time: am 125958 (24h:005958)
Date: 20000102  Week: 0  Time: am 125959 (24h:005959)
Date: 20000102  Week: 0  Time: am 010000 (24h:010000)
Date: 20000102  Week: 0  Time: am 010001 (24h:010001)
---
Date: 20000102  Week: 0  Time: am 115958 (24h:115958)
Date: 20000102  Week: 0  Time: am 115959 (24h:115959)
Date: 20000102  Week: 0  Time: pm 120000 (24h:120000)
Date: 20000102  Week: 0  Time: pm 120001 (24h:120001)
---
Date: 20000102  Week: 0  Time: pm 125958 (24h:125958)
Date: 20000102  Week: 0  Time: pm 125959 (24h:125959)
Date: 20000102  Week: 0  Time: pm 010000 (24h:130000)
Date: 20000102  Week: 0  Time: pm 010001 (24h:130001)
---
Date: 20000102  Week: 0  Time: pm 115958 (24h:235958)
Date: 20000102  Week: 0  Time: pm 115959 (24h:235959)
Date: 20000103  Week: 1  Time: am 120000 (24h:000000)
Date: 20000103  Week: 1  Time: am 120001 (24h:000001)
---
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
