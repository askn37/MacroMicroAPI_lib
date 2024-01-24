# Multix Zinnia Product SDK [Macro/Micro API libraries and samples]

__Multix Zinnia Product SDK__
の共通サブモジュール。

AVR-GCC / AVR-LIBC 専用。__Arduino互換API用ではない。__

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## パス配置

このモジュールは __Arduino IDE__ の
`boards.txt/platform.txt`において
以下のパスに配置される。

```plain
{runtime.platform.path}/cores/{build.core}        <-- MacroAPI_core {includes}
{runtime.platform.path}/libraries/{build.core}    <-- このサブモジュール
{runtime.platform.path}/libraries/M5_Modules      <-- M5_Modules
{runtime.platform.path}/variants/{build.variant}  <-- ボード固有設定 {includes}
```

## 収容ライブラリ

|名称|機能|
|-|-|
|FlashNVM.h|[フラッシュメモリ領域自己書換支援](https://github.com/askn37/askn37.github.io/wiki/FlashNVM)
|FontSets.h|[OLED用英数フォントイメージ](https://github.com/askn37/askn37.github.io/wiki/FontSets)
|GPS_RMS.h|[簡易NMEAパーサ](https://github.com/askn37/askn37.github.io/wiki/GPS_RMC)
|OLED_*.h|[OLEDパネル(I2C)制御支援](https://github.com/askn37/askn37.github.io/wiki/OLED_SSD1306)
|ReadUART.h|[リードバッファ付ハードウェアUSART](https://github.com/askn37/askn37.github.io/wiki/ReadUART)
|SoftwareUART.h|[SoftwareSerial代替](https://github.com/askn37/askn37.github.io/wiki/SoftwareUART)
|TaskChanger.h|[協調的マルチタスク支援](https://github.com/askn37/askn37.github.io/wiki/TaskChanger)
|TimeoutTimer.h|[RTCによるタイムアウト制御支援](https://github.com/askn37/askn37.github.io/wiki/TimeoutTimer)と delay/millis代替
|UrowNVM.h|[USERROW領域書き換え支援](https://github.com/askn37/askn37.github.io/wiki/UrowNVM)
|XRTC_*.h|[外部RTC制御支援](https://github.com/askn37/askn37.github.io/wiki/XRTC_PCF85063A)（NXP PCF85063A用）
|bcddatetime.h|[BCD年月日/時分秒表現とMJD変換支援](https://github.com/askn37/askn37.github.io/wiki/bcddatetime)

## 収容サンプルスケッチ

|群|種別|
|-|-|
|Blink variations|様々な Lチカ 実演
|Two Wire Interfaces|TWI / I2C操作実演
|EEPROM and NVM|不揮発メモリ操作実演
|Timer applications|計時器/計数器周辺機能の様々な応用
|UART and USART|UART/USART実演
|Miscellaneous|その他の様々な実演

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
