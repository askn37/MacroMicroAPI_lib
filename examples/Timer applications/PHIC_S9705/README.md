# PHIC_S9705.ino

このサンプルスケッチは以下について記述している；

- TCB計数器による16bit幅計数捕獲と周波数シリアルプロッタ表示

## 対象AVR

- megaAVR世代
  - tinyAVR-2系統
- modernAVR世代
  - AVR Dx系統

> megaAVR-0、tinyAVR-0/1系統は対応しない。

## デジタルフォトICによる照度ダイレクト計測

[[浜松ホトニクス S9705]](https://www.hamamatsu.com/jp/ja/product/optical-sensors/photo-ic/illuminance-sensor/S9705.html)
は入射光強度に比例した発振周波数のデジタル矩形波を出力するCMOSフォトICだ。
`1lx=500Hz`の矩形波を±3%の公称誤差で出力でき、
__光強度を直接ルクス単位で直接計測__ できる特徴を持つ。
[秋月電子通商](https://akizukidenshi.com/catalog/g/gI-02443/)でも小売があり
アナログ測定回路を組んだり面倒な補正計算をしないで済むので
マイコンと組み合わせるには好適だ。
その性質から（レンズや鏡面パイプで指向性を高めて）精度の高い露出計を組み立てたり、
他のアナログフォトIC回路の校正や補正式の検算に使えるのも便利である。

> 秋月では2022年時点で処分特価中のため近々入手難になりそうではあるが。

AVR等との回路接続も単純で、付加部品もVDDにパスコンを加える程度で良い。

```plain
HAMAMATSU PHOTO IC S9705
     |
RESET:1 --> GND
  GND:2 --> GND
  VDD:3 <-- IOREF/VCC (2.7V-5.5V)
  OUT:4 --> PIN_PD2
            |
            modernAVR
```

IC1番端子の`RESET`は正論理入力なので注意しよう。
オープンにすると内蔵プルアップ抵抗で`HIGH`にされ機能停止状態になるので
使用中は`LOW`入力が必要だ。これは`GND`に落としておくか他のGPIO端子で制御する。

実測での出力周波数は概ね`2.5kHz`-`675kHz`範囲（個体差あり）だったので
`5lx`-`1350lx`を計測でき、普通の室内照度の範囲はカバーしている。
直射日光下ではすぐに測定上限を振り切るので、
屋外使用では NDフィルターを被せて1桁は感度を落としたい。

## TCB計数器での周波数測定

__tinyAVR-2__ 以降の新世代AVRでは
TCB計数器で外部端子入力の直接計数が可能なので、
計数捕獲モードを使えば簡単に周波数測定ができる。

`TCB0`計数器の設定は
`TCB0_EVCTRL`で事象捕獲入力を有効化し、
`TCB0_CTRLB`で動作モードを計数捕獲に変更し、
`TCB0_CTRLA`で事象入力計数を選びつつ`TCB0`計数器を活動状態にすればよい。

```c
/* TCB0を計数捕獲モードに設定 */
TCB0_EVCTRL = TCB_CAPTEI_bm;                      /* 事象捕獲有効 */
TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;                  /* 計数捕獲モード */
TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc; /* 事象から計数入力 */
```

`S9705`からの信号入力は`PIN_PD2`外部端子で受け入れるものとして、
これを`TCB0CAPT`事象計数入力に`EVSYS_CHANNEL3`で転送する。

```c
/* 外部端子入力有効化 */
pinModeMacro(PIN_PD2, INPUT);

/* S9705からの入力を TCB0で計数する */
EVSYS_CHANNEL3 = EVSYS_CHANNEL3_PORTD_PIN2_gc;
EVSYS_USERTCB0COUNT = EVSYS_USER_CHANNEL3_gc;
```

一方`TCB0`事象捕獲は、
`PIT`周期計時器からの`16Hz`周期イベントを
`EVSYS_CHANNEL2`で`TCB0CAPT`に転送することで発生させる。

```c
/* TCB0の計数を捕獲する */
/* DIV2048 == 16 Hz 周期 */
EVSYS_CHANNEL2 = EVSYS_CHANNEL2_RTC_PIT_DIV2048_gc;
EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;

/* PIT有効化 */
loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
RTC_PITCTRLA = RTC_PITEN_bm;
```

これは`TCB0`の計数範囲が 16bit幅なので
`675kHz`を測定可能にするには測定周期が`10.3Hz`以上でないと
計数結果が溢れてしまう。
つまり観測するのが`8Hz`周期では溢れるのでその上の`16Hz`周期を選択する。

$$ \frac{675000Hz}{2^{16}} = 10.2997...Hz $$

計数結果の取得は、事象計数捕獲が為されると
`TCB0_INTFLAGS`の`TCB_CAPT_bp`ビットがセットされるので、
これを（ここでは割込を使わずに）ポーリングで監視して、
捕獲結果を`TCB0_CCMP`から得る。
そして`TCB0_INTFLAGS`のビットをクリアして次の事象捕獲に備える。

```c
/* 計数捕獲 */
loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
uint16_t _CMP = TCB0_CCMP;
TCB0_INTFLAGS = TCB_CAPT_bm;
```

> `TCB0_INTCTRL`で割込を有効化しなくても
`TCB0_INTFLAGS`のビットが立つことに注意。
そしてこの割込ビットは`1`を書くことで`0`にクリアされる性質のものだ。

後はこの捕獲計数値を、実際に必要な単位に換算する。
`16Hz`周期で観測しているので単に16倍すれば、それはそのまま計測された周波数だ。
そして`1lx=500Hz`なので、これを`500Hz`で割れば求める照度（ルクス）が得られる。

```c
/* 捕獲値を kHzに換算 */
float _khz = (float)_CMP / (1000.0 / 16.0);

/* kHz を lxに換算 */
/* 1 lx = 500 Hz */
float _lx = _khz / 0.5;
```

> 観測16Hz周期での計測上限は`2^16*16`で`1048576`Hz未満となるから、
`S9705`の個体差（公称最大1000kHz）には十分対応できる。

以上をまとめて、Arduino IDE
のシリアルプロッタにプリントされるようにすると
次のようなスケッチ記述になる。

```c
void setup (void) {
  pinModeMacro(PIN_PD2, INPUT);
  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("\n\n\n\n"));
  EVSYS_CHANNEL2 = EVSYS_CHANNEL2_RTC_PIT_DIV2048_gc;
  EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;
  EVSYS_CHANNEL3 = EVSYS_CHANNEL3_PORTD_PIN2_gc;
  EVSYS_USERTCB0COUNT = EVSYS_USER_CHANNEL3_gc;
  TCB0_EVCTRL = TCB_CAPTEI_bm;
  TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
  TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc;
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITCTRLA = RTC_PITEN_bm;
}
void loop (void) {
  loop_until_bit_is_set(TCB0_INTFLAGS, TCB_CAPT_bp);
  uint16_t _CMP = TCB0_CCMP;
  TCB0_INTFLAGS = TCB_CAPT_bm;
  float _khz = (float)_CMP / (1000.0 / 16.0);
  float _lx = _khz / 0.5;
  Serial.print(F("LX=")).print(_lx).print(':').print(_lx);
  Serial.print(F(",kHz=")).print(_khz);
  Serial.print(F(",CNT=")).println(_CMP);
}
```

![ProtImage](https://askn37.github.io/img/LIB_PHIC_S9705.png)

### megaAVR-0 や tinyAVR-0/1 系統の場合

これらの系統では`TCB0`で事象入力を計数対象にできる機能がない。
代案としては次の方法がある。

1. `TCA0`に`EVSYS`経由で計数入力し`TCB0`は`TCA0`からのクロック入力を計数する。
（`TCA`計数器には計数捕獲機能がない）
ただしこの方法は2組の計数器を使ってしまう。
また __tinyAVR-0__ では`PIT`からの周期事象が
`EVSYS`を通らないので、捕獲動作は周期割込によるしかない。
2. `TCB0`の計数入力を`CLK_PER`とし`S9705`からの入力を事象捕獲に使う。
この場合は入力周波数の1パルス幅を`CLK_PER`時間精度で計測することになる。
どのAVRでも可能な方法だが別途積分しないとならないだろう。

> 2.の方法で得た計数値を直接プリントすると
`S9705`の高精度が災いして室内蛍光灯などの商用周波数による明滅も拾ってしまい
`50Hz/60Hz`の変動までも観測結果に乗ってしまうことが判る。
このノイズを均すには積分計算による観測値の平準化が必要だ。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
