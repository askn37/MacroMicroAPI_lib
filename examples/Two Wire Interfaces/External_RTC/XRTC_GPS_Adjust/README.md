# XRTC_GPS_Adjust.ino

このサンプルスケッチは以下について記述している；

- UART式GPSからの時刻設定の実演（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 解説

この実演は`PCF85063A`の時刻情報を、UART出力の GPSに合わせるデモンストレーションだ。
GPSモジュールと __"Zinnia Duino"__ は次のように配線する。

```plain
[GPS]    [Duino]
  TXD --> PIN_PD2
  RXD <-- PIN_PD1
  VCC <-- VCC (3.7V〜5.5V)
  GND --> GND
```

GPSの NMEA出力は`<SoftwareUART.h>`で読み取り、
`<GPS_RMC.h>`で`$**RMC`行を解析し、UTC時刻（と現在位置）を取得する。
UTC時刻から地方時刻を得るには`TZ_OFFSET`で時差を指定しておく。

> GPSモジュールは
[[M5Stack: Mini GPS/BDS Unit (AT6558) SKU:U032]](https://shop.m5stack.com/collections/m5-sensor/products/mini-gps-bds-unit)
の使用を想定している。
他の製品を使う場合は通信速度や設定をそれに合わせて調整する。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
