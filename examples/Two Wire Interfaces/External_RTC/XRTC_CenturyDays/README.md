# XRTC_CenturyDays.ino

このサンプルスケッチは以下について記述している；

- 400年カレンダー時刻の実演（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`を
400年カレンダー（万年カレンダー）に用いた際のデモンストレーションだ。
グレゴリオ暦での各世紀の年末年始と、
閏日前後でどのようにカレンダー時刻が更新されるかを繰り返しプリントする。

## 1900年に閏日は存在しない

```plain
MJD: 15078  Wd: 3  Date: 19000228  Time: 235958  Wd: 3  Epoch: 2091076094
MJD: 15078  Wd: 3  Date: 19000228  Time: 235959  Wd: 3  Epoch: 2091076095
MJD: 15079  Wd: 4  Date: 19000301  Time: 000000  Wd: 4  Epoch: 2091076096
MJD: 15079  Wd: 4  Date: 19000301  Time: 000001  Wd: 4  Epoch: 2091076097
MJD: 15079  Wd: 4  Date: 19000301  Time: 000002  Wd: 4  Epoch: 2091076098

MJD: 15079  Wd: 4  Date: 19000301  Time: 235958  Wd: 4  Epoch: 2091162494
MJD: 15079  Wd: 4  Date: 19000301  Time: 235959  Wd: 4  Epoch: 2091162495
MJD: 15080  Wd: 5  Date: 19000302  Time: 000000  Wd: 5  Epoch: 2091162496
MJD: 15080  Wd: 5  Date: 19000302  Time: 000001  Wd: 5  Epoch: 2091162497
MJD: 15080  Wd: 5  Date: 19000302  Time: 000002  Wd: 5  Epoch: 2091162498
```

> 32bit符号なし表現型の`Epoch`は`1970-01-01 00:00:00`を`0`（元期）とする。
それ以前は意味を為さない。

## 20世紀から21世紀への繰り上がり

```plain
MJD: 51543  Wd: 5  Date: 19991231  Time: 235958  Wd: 5  Epoch: 946684798
MJD: 51543  Wd: 5  Date: 19991231  Time: 235959  Wd: 5  Epoch: 946684799
MJD: 51544  Wd: 6  Date: 20000101  Time: 000000  Wd: 6  Epoch: 946684800
MJD: 51544  Wd: 6  Date: 20000101  Time: 000001  Wd: 6  Epoch: 946684801
MJD: 51544  Wd: 6  Date: 20000101  Time: 000002  Wd: 6  Epoch: 946684802
```

## 2000年に閏日は存在する

```plain
MJD: 51602  Wd: 1  Date: 20000228  Time: 235958  Wd: 1  Epoch: 951782398
MJD: 51602  Wd: 1  Date: 20000228  Time: 235959  Wd: 1  Epoch: 951782399
MJD: 51603  Wd: 2  Date: 20000229  Time: 000000  Wd: 2  Epoch: 951782400
MJD: 51603  Wd: 2  Date: 20000229  Time: 000001  Wd: 2  Epoch: 951782401
MJD: 51603  Wd: 2  Date: 20000229  Time: 000002  Wd: 2  Epoch: 951782402

MJD: 51603  Wd: 2  Date: 20000229  Time: 235958  Wd: 2  Epoch: 951868798
MJD: 51603  Wd: 2  Date: 20000229  Time: 235959  Wd: 2  Epoch: 951868799
MJD: 51604  Wd: 3  Date: 20000301  Time: 000000  Wd: 3  Epoch: 951868800
MJD: 51604  Wd: 3  Date: 20000301  Time: 000001  Wd: 3  Epoch: 951868801
MJD: 51604  Wd: 3  Date: 20000301  Time: 000002  Wd: 3  Epoch: 951868802

MJD: 51604  Wd: 3  Date: 20000301  Time: 235958  Wd: 3  Epoch: 951955198
MJD: 51604  Wd: 3  Date: 20000301  Time: 235959  Wd: 3  Epoch: 951955199
MJD: 51605  Wd: 4  Date: 20000302  Time: 000000  Wd: 4  Epoch: 951955200
MJD: 51605  Wd: 4  Date: 20000302  Time: 000001  Wd: 4  Epoch: 951955201
MJD: 51605  Wd: 4  Date: 20000302  Time: 000002  Wd: 4  Epoch: 951955202
```

## 21世紀から22世紀への繰り上がり

```plain
MJD: 88068  Wd: 4  Date: 20991231  Time: 235958  Wd: 4  Epoch: 4102444798
MJD: 88068  Wd: 4  Date: 20991231  Time: 235959  Wd: 4  Epoch: 4102444799
MJD: 88069  Wd: 5  Date: 21000101  Time: 000000  Wd: 5  Epoch: 4102444800
MJD: 88069  Wd: 5  Date: 21000101  Time: 000001  Wd: 5  Epoch: 4102444801
MJD: 88069  Wd: 5  Date: 21000101  Time: 000002  Wd: 5  Epoch: 4102444802
```

## 2100年に閏日は存在しない

```plain
MJD: 88127  Wd: 0  Date: 21000228  Time: 235958  Wd: 0  Epoch: 4107542398
MJD: 88127  Wd: 0  Date: 21000228  Time: 235959  Wd: 0  Epoch: 4107542399
MJD: 88128  Wd: 1  Date: 21000301  Time: 000000  Wd: 1  Epoch: 4107542400
MJD: 88128  Wd: 1  Date: 21000301  Time: 000001  Wd: 1  Epoch: 4107542401
MJD: 88128  Wd: 1  Date: 21000301  Time: 000002  Wd: 1  Epoch: 4107542402

MJD: 88128  Wd: 1  Date: 21000301  Time: 235958  Wd: 1  Epoch: 4107628798
MJD: 88128  Wd: 1  Date: 21000301  Time: 235959  Wd: 1  Epoch: 4107628799
MJD: 88129  Wd: 2  Date: 21000302  Time: 000000  Wd: 2  Epoch: 4107628800
MJD: 88129  Wd: 2  Date: 21000302  Time: 000001  Wd: 2  Epoch: 4107628801
MJD: 88129  Wd: 2  Date: 21000302  Time: 000002  Wd: 2  Epoch: 4107628802
```

> 32bit符号なし表現型の`Epoch`は`2106-02-07 06:28:15`を超えると`0`に逸脱する。
それ以後は意味を為さない。

## 22世紀から23世紀への繰り上がり

```plain
MJD: 124592  Wd: 2  Date: 21991231  Time: 235958  Wd: 2  Epoch: 2963151102
MJD: 124592  Wd: 2  Date: 21991231  Time: 235959  Wd: 2  Epoch: 2963151103
MJD: 124593  Wd: 3  Date: 22000101  Time: 000000  Wd: 3  Epoch: 2963151104
MJD: 124593  Wd: 3  Date: 22000101  Time: 000001  Wd: 3  Epoch: 2963151105
MJD: 124593  Wd: 3  Date: 22000101  Time: 000002  Wd: 3  Epoch: 2963151106
```

## 2200年に閏日は存在しない

```plain
MJD: 124651  Wd: 5  Date: 22000228  Time: 235958  Wd: 5  Epoch: 2968248702
MJD: 124651  Wd: 5  Date: 22000228  Time: 235959  Wd: 5  Epoch: 2968248703
MJD: 124652  Wd: 6  Date: 22000301  Time: 000000  Wd: 6  Epoch: 2968248704
MJD: 124652  Wd: 6  Date: 22000301  Time: 000001  Wd: 6  Epoch: 2968248705
MJD: 124652  Wd: 6  Date: 22000301  Time: 000002  Wd: 6  Epoch: 2968248706

MJD: 124652  Wd: 6  Date: 22000301  Time: 235958  Wd: 6  Epoch: 2968335102
MJD: 124652  Wd: 6  Date: 22000301  Time: 235959  Wd: 6  Epoch: 2968335103
MJD: 124653  Wd: 0  Date: 22000302  Time: 000000  Wd: 0  Epoch: 2968335104
MJD: 124653  Wd: 0  Date: 22000302  Time: 000001  Wd: 0  Epoch: 2968335105
MJD: 124653  Wd: 0  Date: 22000302  Time: 000002  Wd: 0  Epoch: 2968335106
```

## 23世紀から20世紀への巻き戻り

```plain
MJD: 161116  Wd: 0  Date: 22991231  Time: 235958  Wd: 0  Epoch: 1823857406
MJD: 161116  Wd: 0  Date: 22991231  Time: 235959  Wd: 0  Epoch: 1823857407
MJD: 15020  Wd: 1  Date: 19000101  Time: 000000  Wd: 1  Epoch: 2085978496
MJD: 15020  Wd: 1  Date: 19000101  Time: 000001  Wd: 1  Epoch: 2085978497
MJD: 15020  Wd: 1  Date: 19000101  Time: 000002  Wd: 1  Epoch: 2085978498
```

> 時分秒と曜日が連続していることに注意。その他は連続性なく逸脱する。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
