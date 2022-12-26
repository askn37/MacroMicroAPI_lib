# XRTC_ConsoleAdjust.ino

このサンプルスケッチは以下について記述している；

- シリアルモニターからのUARTによる時刻設定の実演（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`にシリアルモニターから時刻設定する例を示している。
スケッチアップロードして起動すると次のようなプリントアウトがなされ、
文字列入力待ちとなる。

```plain
<startup>
F_CPU=24000000
[prefix: ':' time, '/' date, '=' unix-epoch, '+|-' shift seconds]
Date: 20221225  Week: 0  Time: 183731  Epoch: 1671961051
```

- Date : 年月日（YYYYMMDD）
- Week : 曜日（`0:日`〜`6:土`）
- Time : 時分秒（hhmmss）
- Epoch : UNIX Epoch（UTC）

`Epoch`と現在時刻との時差計算の初期値は9時間としている。
これはスケッチ冒頭の`TZ_OFFSET`マクロで変更可能。

## Enter入力のみ

現在時刻を再表示する。

```plain
[Enter]
Date: 20221225  Week: 0  Time: 183732  Epoch: 1671961052
```

## 時分秒設定

文字列先頭に`:`が付されていると、続く6桁の10進数を時分秒として設定する。
`000000`〜`235959`が有効。

```plain
:235959[Enter]
```

## 年月日設定

文字列先頭に`/`が付されていると、続く8桁の10進数を年月日として設定する。
`19000101`〜`22991231`が有効。

```plain
/20221225[Enter]
```

> 1900年元旦から2299年大晦日までの400年間の任意の日付が設定可能。\
> `22991231`日曜日の翌日は`19000101`月曜日に巻き戻る。

## Epoch設定

文字列先頭に`=`が付されていると、続く10進数を`UNIX Epoch`として設定する。
`0`〜`4294967295`が有効。`UTC`->`地方時`は自動で変換される。

```plain
=1671000000[Enter]
```

## 秒調整

文字列先頭に`+`または`-`が付されていると、続く10進数を`UNIX Epoch`に加減算する。
ただし結果は`UNIX Epoch`の設定可能範囲を超えない。
（範囲外の年と曜日は再調整される）

```plain
+10[Enter]
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
