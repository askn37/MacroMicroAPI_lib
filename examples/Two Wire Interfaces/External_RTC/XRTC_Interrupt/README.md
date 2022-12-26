# XRTC_Interrupt.ino

このサンプルスケッチは以下について記述している；

- アラーム割込によるCPU休止解除の実演（PCF85063A用）

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> NXP PCF85063Aは __"Zinnia Duino"__ 系列にオンボードで搭載されている。

## 要件

この実演の初期設定は次のようなものだ。
何れも`<XRTC_PCF85063A.h>`によって実現される機能である。

- 30秒毎の周期割込（HarfMinuteInterrupt）
- 7秒毎の周期割込（PeriodTimer）
- 17秒毎のアラーム割込（activeAlarm）

これらが同時に稼働して「パワーダウン」休止状態の CPUを活性化させる。

## isPowerDown

`isPowerDown`メソッドは`PCF85063A`（以後`XRTC`）が電源断を経験し、
その時刻保持情報が信頼できなくなっている時に真を返す。
これが偽を返すのならば新規の時刻設定は、校正を意図するのでなければ必要ない。
時刻参照が不要で単に周期タイマーを使用するだけなら、無視しても構わない。

```c
if (XRTC.isPowerDown()) {
  XRTC.adjust(_bcddatetime);
}
```

## 時刻設定

`adjust`メソッドは`bcddatetime_t`型か`time_t`型で新たな時刻情報を受け取り、設定する。
秒未満の精度での時計合わせを希望するなら`stop`、`start`メソッドと組み合わせる。

```c
XRTC.stop();           /* 計時機能停止 */
XRTC.adjust(new_time); /* 時刻情報設定 */
/* GPS 1PPS信号などとの 適当なタイミング合わせ */
XRTC.start();          /* この 約0.5秒後に new_time + 1 秒から計時開始*/
```

## 周期タイマー

周期タイマー機能には、ふたつの固定周期組込機能と、
ひとつのユーザー任意時間設定タイマーがある。
固定周期のものは時刻情報の秒の桁が`00`か`30`の時に割込を引き起こすが、
任意時間のものは活性化した時点の時刻情報を元期とする。

```c
XRTC.setMinuteInterruptEnable(false).activeTimer();    /* 組込60秒周期（無効） */
XRTC.setHarfMinuteInterruptEnable(true).activeTimer(); /* 組込30秒周期（有効） */
XRTC.startPeriodTimer(20);                             /* 任意時間周期 1-15300秒 */
```

> 組込30秒周期は 組込60秒 の効果を隠蔽するので、両方同時に有効化する理由はない。

周期タイマーは初回割込起動時に`isTimer`が真を返し
`clearTimerFlag`でクリアされるまでそれを維持する。
割込信号は単発ストローブなので`INT`端子はすぐ開放される。

## 単発タイマー設定

任意時間周期タイマーは、任意時間単発タイマーとして使うこともできる。

```c
XRTC.startCountdownTimer(15);
```

これの場合`isTimer`が真になると`clearTimerFlag`を指令するまで
`INT`割込は開放されない。
これはアラーム同様の`LOW`レベル型の割り込みである。

```c
if (XRTC.isTimer()) {
  XRTC.clearTimerFlag();
}
```

## アラーム設定

`setAlarmSettings`メソッドはアラーム割込時刻を設定する。
`bcdtime_t`型は`日時分秒`を含むパックドBCD表現時刻、
`XRTC_ALARM_SETTINGS`型構造体はどの時刻桁比較を有効化するかの
情報をアラーム機能に指示する。
それぞれの桁比較は、その桁がインクリメントされて指定値と合致した時が真だ。
故に日比較（月に一度）と曜日比較（週に一度）の両方を同時に真とすると、
数年に一度しか合致する機会がない場合もあることに注意されたい。
時と分（と秒の）比較を同時に真とすると、1日に一度特定の時刻に真となる。

```c
XRTC.setAlarmSettings(
/* 日時分秒指定 */
  alarm_time,
  (XRTC_ALARM_SETTINGS){
/* 曜日指定,  秒,   分,   時,     日,   曜日 */
        0, true, true, true, false, false
  }
).activeAlarm(true);
```

> 当然のことながらアラーム機能は時刻設定が正しいことを前提とする。
`isPowerDown`が真でかつレジスタ内容が不正な場合、意図した動作にならない。

## アラーム停止

一度発生したアラームは`clearAlarmFlag`で停止させないと
`INT`割込が`LOW`レベルを維持したままになる。
設定したアラームを発生前に取り消すには`activeAlarm(false)`メソッドを使う。

```c
if (XRTC.isAlarm()) {
  XRTC.clearAlarmFlag();
}
```

## アラーム相対時刻設定

アラーム機能を周期タイマーのように使う場合、
現在時刻からの相対的なアラーム時刻を計算しなければならない。
分単位（60秒）だけ、時単位（60分）だけ、日単位（24時間）だけなら
前述のフラグ指定だけしてあとは放置すればよいが、
そうでなければ一旦`UNIX Epoch`に変換して相対秒数を加減算するのがよい。

```c
time_t epoch = XRTC.getEpochNow();
bcddatetime_t _alarm = epochToBcdDateTime(epoch + 33); /* 33秒後 */
XRTC.setAlarmSettings(_alarm.time, (XRTC_ALARM_SETTINGS){
  0, true, true, true, false, false
}).activeAlarm(true);
```

## 時刻設定の与え方

この実演スケッチプログラムでは、そのビルド時刻を初期設定値とする。
ビルド時刻は特殊組込マクロ`__TIME__`と`__DATE__`から次のように与える。

```c
char timstr[] = __TIME__ " " __DATE__;
      /* 例えば "08:34:56 Nov 18 2022" のように展開される */
```

`buildtime`関数はこの文字列を`bcddatetime_t`型に変換するコード例だ。

> スケッチアップロードではその待ち時間があるので、だいたい5〜6秒遅れの時刻が設定される。

## CPU休止状態での割込

__"Zinnia Duino"__ では`PCF85063A`の`INT`出力端子は
ジャンパー端子経由で`PIN_PF1`に接続される。
これはふつうユーザースイッチ`USW`と共用かつオープンドレインなので、
CPUから認識させるにはプルアップ抵抗を有効化しなければならない。
また「パワーダウン／スタンバイ」休止状態では「両端点割込」だけが
休止解除を行えるので、次の設定が定型的に必要となる。

```c
pinControlRegister(PIN_PF1) = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
```

> `USW`割込と`XRTC`割込はふつう区別しない。
ユーザーの代わりにアラームやタイマーが休止解除ボタンを押すものと思えば良い。
`XRTC`割込を区別するには`clearAlarmFlag`する前に
`isAlarm`が真か否かを調べるなどする。

実際の周期割込／アラーム割込は「下降端点」で通知されるが
この設定では「上昇端点」も余分な割込を発生させるので、
これを回避するには割込フラグ解除を休止直前まで延期するのが良い。
割込パルスは`1/64`〜`1/4096`秒しか持続しないから、
ほとんどの場合はこれで多重割込発生を抑制できる。

```c
EMPTY_INTERRUPT(PORTF_PORT_vect);  /* 何もしない割込ハンドラ */

PORTF_INTFLAGS = PIN1_bm; /* 休止直前に割込通知了解 */
sleep_cpu();
```

## ユーザーボタンとの併用

アラーム割込および単発タイマー割込は、クリアされるまで`INT`端子を`LOW`に保持する。
よって休止前に必ず`clearAlarmFlag`や`clearTimerFlag`で`INT`端子を開放すること。
`LOW`が維持されているあいだは`USW`割込も当然発生しないので
ユーザーボタン認識を行うならその前に解除することになる。

> `USW`割込は、ユーザーボタンを押したときと離したときの2回割込を発生させる。
これが害となる場合はユーザーボタンが離されるまで
次の休止状態に入らないようにしなければならない。
休止解除後の処理時間が十分長ければ
その間にボタンは離されていると見込むこともできるので、
ほとんどの運用はそれで対処できるだろう。
それができないほど短い処理しかしない場合は
「アイドル」休止を選んで割込条件を`PORT_ISC_FALLING_gc`に変えるほうが良いだろう。

割込条件は`PORT_ISC_BOTHEDGES_gc`に変えて
`PORT_ISC_LEVEL_gc`を割込条件にしても
「パワーダウン／スタンバイ」休止状態を解除できるが、
`INT`割込が開放されるまで割込発生が繰り返し継続する。
こちらの場合はユーザーボタンを押している間は結果的に休止状態をすぐ抜け出す。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
