# TimeoutTimer.ino

このサンプルスケッチは以下について記述している；

- TIMEOUT_BLOCK 機能とその派生タイマーの実装。
  - delay_timer
  - millis_timer
  - sleep_cpu_timer
  - interval_check_timer

__協調的マルチタスク__ 応用については `<TaskChanger.h>`  ライブラリを参照のこと。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## TIMEOUT_BLOCK

`<TimeoutTimer.h>`ライブラリは次の機能を主として提供する。

```c
#include <TimeoutTimer.h>

TIMEOUT_BLOCK(1000) {
  while (true) {
    /* 無限ループ等 */
  }
}
```

`TIMEOUT_BLOCK` は指定ミリ秒を経過してなおブロック内のコードが実行中であるなら、
それを強制中断してブロックの外に処理を移す。
ブロック内で状態変数を管理し、自身でタイムアウト脱出を判定する必要はない。
従って単純な無限ループでも経過時間が過ぎれば処理を中断させることができ、
記述を単純化できる。

これを用いたLチカ スケッチは次のように書き下せる。

```c
#include <TimeoutTimer.h>
void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
}
void loop (void) {
  TIMEOUT_BLOCK(1000) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    while (true);
  }
}
```

### 動作原理

`<TimeoutTimer.h>`ライブラリはインクルードされた時点で
次の周辺機能リソースを専有し、有効化する。

- RTC計時器 -- `CLK_RTC=1KHz` 約976us精度に設定（既定では）
- RTC_CNT_vect 割込ベクタ

`TIMEOUT_BLOCK` が開始されると、次のように動作する。

1. 現在の実行状態を保存し、指定時間後に発火する時間比較割込を有効化。
1. 時間比較割込発火前にブロックを抜けるか、時間比較割込が発火すると現在の実行状態を放棄し、開始前の実行状態を復元し、ブロックの次から実行を継続。

> 実行状態の保存と復元には`<setjmp.h>`標準C言語ライブラリが使われている。

### 制約

1\. `TIMEOUT_BLOCK` 内での変数書換はブロック外へ伝播されずに破棄される可能性が高い。
これを避けるには変数に `volatile` 宣言を付加しておく。

```c
volatile bool state = false;
TIMEOUT_BLOCK(1000) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = true;
  }
}
if (state) {} // volatile 宣言がない場合、真にならない可能性がある
```

> 単純な変数は実メモリが割り付けられず、単なるレジスタ変数に最適化されうる。
ブロック内のレジスタ状態はブロックを抜ける時に破棄されるので変更内容も忘却される。

なお volatile 変数の書換えはクリティカル・セクションとして
`ATOMIC_BLOCK` 保護されるべきだ。
さもないとメモリ変更コード半ばで `TIMEOUT_BLOCK` を抜けてしまう可能性があり、不正な値を返しうる。

2\. タイムアウトはミリ秒で指定するが、計時器精度に伴う誤差がある。
丸めの方向によって最大時間にも解釈されるから最小 __2__ は指定しなければならない。
一方で指定可能な最大値は __63998__ である。

> 16bit幅ミリ秒指定の最大有効値は __TIMEOUT_MILLIS_MAX マクロ定数で参照できる。

3\. `TIMEOUT_BLOCK` 内で全体割込を禁止すると、その間のタイムアウト割込は抑制される。
無限ループ内で割込禁止にすると抜け出せなくなるので注意。
また割込禁止のままブロックを抜けると以後の動作も保証できない。
従って`sei` `cli`を直接使うのではなく、
割込禁止対象は `ATOMIC_BLOCK` で囲むようにすべきだ。

```c
TIMEOUT_BLOCK(1000) {
  while (true) {
    uint16_t timeleft = TimeoutTimer::time_left();
    /* #include <util/atomic.h> */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      Serial.println(timeleft, DEC);
    }
  }
}
```

4\. TIMEOUT_BLOCK を再入すると、内側ブロック開始時に外側ブロックの計時は一時停止する。

```c
/* 10秒後に脱出させたいが実際にはそうならない */
TIMEOUT_BLOCK(10000) {
  while (true) {
    TIMEOUT_BLOCK(1000) {
      while (true) {
        /* ここにとどまるあいだ外側の計時は一時停止する */
      }
    }
    /* ここを通過する間は外側が計時される */
  }
}
```

この例では外側ブロックのタイムアウト発火時期を正確には予測できなくなる。

一方 `ticks_left` の計数値は中断ぜずに進むので、
多重ループが必要なら `interval_check_timer` を使い
ポーリングで記述するのが良い。

```c
/* 10秒経過後から真となる計数比較 */
uint32_t _check = TimeoutTimer::ticks_left() + timeout_millis_to_ticks(10000);
while ( ! TimeoutTimer::interval_check_ticks(_check) ) {
  TIMEOUT_BLOCK(1000) {
    while (true) {
      /* STUB */
    }
  }
}
```

> TIMEOUT_BLOCK を実現する割込ベクタがひとつしかないことに起因する。

## その他の付加機能

### PIT周期割込との併用

RTC計時器とその割込は使用するが PIT周期計時器とその割込は使用しないため、
利用者は自由に活用して構わない。
両者は独立して機能する。

```c
#include <avr/io.h>

loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
RTC_PITINTCTRL = RTC_PI_bm;
RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;

ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
}
```

> RTC_CTRLA 等の PIT 周辺機能制御以外のレジスタには触らないこと。

### void TimeoutTimer::abort (void)

現在の TIMEOUT_BLOCK を中断し、ブロック外に抜ける。

```c
TIMEOUT_BLOCK(1000) {
  /* STUB */

  TimeoutTimer::abort(); /* 途中で中断 */

  /* STUB */
}
```

脱出できる `TIMEOUT_BLOCK` は1段だけであり、複数段を一回で抜けることは出来ない。

> TIMEOUT_BLOCK を再入している場合、外側のタイムアウト計時が中断点から再開する。\
> TIMEOUT_BLOCK の外でこれを実行すると予測できない結果を招く。多くはリセットが掛かる。

### uint16_t TimeoutTimer::time\_left (void)

`TIMEOUT_BLOCK` 内で、タイムアウトまでの残数を取得できる。単位は 計時器精度。（約976us）

取得値をミリ秒に換算するには `timeout_ticks_to_millis`マクロを、
マイクロ秒に換算するには `timeout_ticks_to_micros`マクロを利用できる。

```c
TIMEOUT_BLOCK(1000) {
  while (true) {
    uint16_t timeleft = TimeoutTimer::time_left();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      Serial.println(timeleft, DEC);
    }
  }
}
```

> 取得時間は目安であって運用時の CPU動作クロックや周辺環境（気温等）によっても変動する。

### uin32_t TimeoutTimer::ticks\_left (void)

計時器の継続計数を取得する。
単位は 計時器精度（約976us）で 32bit幅の有効範囲を持つ。
従って 4194304秒（約48.5日）でこの計数は一巡する。
元期は MCUリセット直後からである。

取得値をミリ秒に換算するには `timeout_ticks_to_millis`マクロを、
マイクロ秒に換算するには `timeout_ticks_to_micros`マクロを利用できる。

```c
uint32_t tc, ms, us;
tc = TimeoutTimer::ticks_left();
ms = timeout_ticks_to_millis(tc); // TimeoutTimer::millis_left() 取得結果に同じ
us = timeout_ticks_to_micros(tc); // TimeoutTimer::micros_left() 取得結果に同じ
```

### uint32_t TimeoutTimer::millis\_left (void)

計時器の継続計数をミリ秒換算で取得する。
計時開始から 4194304000 カウント経過（約48.5日）でゼロに丸められる。
いわゆる `millis` 関数の代替。

> 32bit幅いっぱいで溢れる前に丸めが発生することに注意。
ある時間差を求めたい場合は ticks 計数値を直接加減算し、比較判定の換算は最後にすべきである。

### uint32_t TimeoutTimer::micros\_left (void)

計時器の継続計数をマイクロ秒換算で取得する。
これはおおむね 268秒相当で丸められる。
いわゆる `maicros` 関数の代替。

> 便宜上用意されているが時間粒度が荒くかつ計数溢れと丸めが早いため目安にしかならないことに注意。

### void TimeoutTimer::delay\_ticks (uint16\_t \_ticks)

指定計数のあいだ処理進行を遅滞させる。いわゆる `delay` 関数の代替。
ミリ秒から指定計数値を得るには `timeout_millis_to_ticks` マクロが使用できる。
直接ミリ秒を指定可能な `delay_timer` マクロも用意されている。

指定可能な最大値は __63998__ （ms）である。（16bit幅）

```c
TimeoutTimer::delay_ticks( timeout_millis_to_ticks(1000) );
// または短縮マクロで
delay_timer(1000);
// または（他のAPIを同時に組み込んでいなければ）
delay(1000);
```

> ライブラリをインクルードした時点で従前の Macro API `delay` は、このマクロに交換される。\
> `delayMaicroseconds`を代替する機能はない。

なお遅延待機中は（TIMEOUT_BLOCK の外であれば）`yield` 関数が継続して実行されるので、
これを用いると __協調的マルチタスク__ を実現できる。
詳細は `<TaskChanger.h>` ライブラリを参照のこと。

### void TimeoutTimer::sleep\_cpu\_ticks (uint32\_t \_ticks)

指定計数のあいだ CPUを休止状態に置く。
指定計数経過か、他の割込によって休止状態は解除される。
ミリ秒で直接指定するには `sleep_cpu_timer` マクロが使用できる。

```c
set_sleep_mode(SLEEP_MODE_STANDBY);
sleep_enable();

TimeoutTimer::sleep_cpu_ticks( timeout_millis_to_ticks(10000) );
// または
sleep_cpu_timer(10000);
```

この関数は `TIMEOUT_BLOCK` の中で使うことが出来ない。その機能を停止する。

> 休止中は計時が停止する。休止中の実経過時間を取得することはできない。\
> 休止を終了させた条件を知ることは出来ない。これは他の割込ベクタで調べる必要がある。\
> 休止時間は概ねの目安であって精度の保証はない。休止中の正確な経過時間を得るには外部RTCを使用のこと。

### bool TimeoutTimer::interval\_check\_ticks (uint32\_t &\_ticks, uint16\_t \_interval = 0)

指定計数が経過するたびに真を返し、保持カウンタ変数を更新する。
時間間隔をミリ秒で直接指定するには `interval_check_timer` マクロが使用できる。

指定可能な最大値は __63998__ （ms）である。

```c
uint32_t _check1 = TimeoutTimer::ticks_left();
uint32_t _check2 = _check1;
while (true) {
  // if ( TimeoutTimer::interval_check_ticks( _check1, timeout_millis_to_ticks(30) ) )
  if ( interval_check_timer( _check1, 30 ) ) digitalWriteMacro(LED_BUILTIN, TOGGLE);
  if ( interval_check_timer( _check2, 31 ) ) digitalWriteMacro(LED_BUILTIN, TOGGLE);
}
```

殆どの周期的逐次処理はこの機能で記述できるだろう。

> 実際の確認周期より短い計数を指定すると、常に真を返す。（内部計数が溢れるまで）\
> 保持変数の初期値が現在計時値より小さい場合、それに追いつくまで繰り返し真を返す。

マクロでない方の `interval_check_ticks` の第2引数は
省略したなら 0 指定であると解釈される。
これは計数値が溢れるまで（符号付32bit変数比較なので最大約24日）いちど真になった状態を変化させない。
従って単発の時間経過を知るには、次のように書き下すことができる。

```c
/* 10秒経過後から真となる計数比較 */
uint32_t _check = TimeoutTimer::ticks_left() + timeout_millis_to_ticks(10000);
while (!TimeoutTimer::interval_check_ticks(_check)) {
  TIMEOUT_BLOCK(1000) {
    while (true) {
      /* STUB */
    }
  }
}
```

### void TimeoutTimer::begin (uint32\_t \_ticks = 0)

### uint32_t TimeoutTimer::end (void)

RTC計時器を開始または停止する。
停止時には現在の計数値を返すので、
計時再開時にこれを渡して以前の状態から継続することができる。
これらは `sleep_cpu_ticks` 内で使用されている。

```c
/* RTC計時器停止 */
uint32_t _save_ticks = TimeoutTimer::end();

/* RTC周辺機能を使用する他の処理... */

/* RTC計時器再開 */
TimeoutTimer::begin( _save_ticks );
```

> 停止中は TIMEOUT_BLOCK 等を使用できない。その中でも使えない。\
> RTC_CNT_vect 割込ベクタは開放されず再定義も出来ないことには注意。

### #define \_\_TIMEOUT\_CLKFREQ

RTC計時器の時間粒度を指定するマクロ定数。
既定値は 1024 （1024Hz）。

`<TimeoutTimer.h>`ライブラリをインクルードする前に
これを再定義すると計測時間粒度を変更できる。
指定可能なのは 512、1024、2048 （単位は Hz）の三種。
通常は既定値のままでよいだろう。

- 512Hzにすると約1953us粒度に精度が下がるが、8388608秒（約97日）までの計時が可能になる。
- 2048Hzにすると約488us粒度に精度が上がるが、2097152秒（約24日）までしか計時ができない。
  - delay_timer や interval_check_timer の指定可能値も __31266__（ms）まで低下する。

> 16bit幅ミリ秒指定の最大有効値は __TIMEOUT_MILLIS_MAX マクロ定数で参照できる。

### #define NOTUSED_INITIALIZE_TIMEOUTTIMER

このマクロ定数を事前に定義すると暗黙の `Timeout::begin` 実行開始が抑止される。
`setup` 内で明示的に `Timeout::begin` が呼ばれるまで
計時動作は開始されず、他の機能も動作しない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
