# TaskChanger.ino

このサンプルスケッチは以下について記述している；

- 協調的マルチタスクの実装。

関連する時間関連機能については ```<TimeoutTimer.h>``` ライブラリを参照のこと。

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

> SRAM 128byte品種での動作は期待できない

## ノンプリエンプティブ・マルチタスク

```<TaskChanger.h>```ライブラリは次の機能を提供する。

```c
#include <TaskChanger.h>

volatile char task1_stack[64];
volatile char task2_stack[64];
volatile char task3_stack[64];

void setup (void) {
  TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);
  TaskChanger::attach_task_2nd(task2_stack, sizeof(task2_stack), &task2);
  TaskChanger::attach_task_3rd(task3_stack, sizeof(task3_stack), &task3);
}

void loop (void) { yield(); }
void task1 (void) { while(true) yield(); }
void task2 (void) { while(true) yield(); }
void task3 (void) { while(true) yield(); }
```

```TaskChanger``` は最大3個のタスクを ```loop()``` 関数の他に登録することができる。
最初は ```loop()``` から実行を開始するが ```yield()``` が呼ばれると
現在のタスク状態を保存し、次の登録タスクに実行権を移す。
これを順次繰り返して少なくとも4度目の ```yield()``` で最初の
```loop()``` タスクに実行権が（ラウンドロビン動作で）戻る。
```yield()``` が呼ばれない限り現在のタスクが実行権を握り続けるので、
この形態は __協調的マルチタスク__（ノンプリエンプティブ・マルチタスク）と呼ばれる。

```loop()``` は無条件に繰り返されるが、その他のタスクはそうではない。
それぞれのタスクは自身の中で必要なだけ処理をループを続けなければならない。
そうでなければタスクは終了し、登録エントリから破棄され、
改めて再登録されかつ呼び出されるまで再び実行権を得ることはない。

```<TaskChanger.h>```はインクルードされた時点で
```<TimeoutTimer.h>```ライブラリも有効化する。
そしてそちらの ```TimeoutTimer::delay_ticks()``` は
遅延待機中に ```yield()``` を呼ぶようになっているので、
次のような記述ができる。

```c
void loop (void) {
  Serial.print(F("0:")).println(TimeoutTimer::millis_left(), DEC);
  Serial.flush();
  delay(1000); /* タスク実行権委譲 */
}

void task1 (void) {
  while (true) {
    Serial.print(F("1:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000); /* タスク実行権委譲 */
  }
}

void task2 (void) {
  while (true) {
    Serial.print(F("2:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000); /* タスク実行権委譲 */
  }
}

void task3 (void) {
  while (true) {
    Serial.print(F("3:")).println(TimeoutTimer::millis_left(), DEC);
    Serial.flush();
    delay(1000); /* タスク実行権委譲 */
  }
}
```

ここからは次のような出力結果を得られるだろう。

```plain
14:18:30.878 -> 0:12
14:18:30.878 -> 1:20
14:18:30.878 -> 2:26
14:18:30.878 -> 3:33
14:18:31.895 -> 0:1020
14:18:31.895 -> 1:1028
14:18:31.895 -> 2:1037
14:18:31.895 -> 3:1044
14:18:32.927 -> 0:2028
14:18:32.927 -> 1:2037
14:18:32.927 -> 2:2044
14:18:32.927 -> 3:2053
14:18:33.917 -> 0:3036
14:18:33.917 -> 1:3044
14:18:33.917 -> 2:3053
14:18:33.917 -> 3:3061
```

それぞれのタスクの ```print``` 出力は 個々の ```delay()``` で指定した時間間隔を空ける。
それでいて個々の ```delay()``` は他のタスク実行を妨げないので、
4個のタスクが並列実行されたかのように動作する。

> 実際は全くの順次実行で、切替間隔が十分短いならば並列実行されたかのように見えるだけだ。

### プリエンプティブ・マルチタスク

```yield()``` はタイマー割込ベクタの中から呼ぶこともできる。
するとこれはタイマー間隔で強制的にタスク実行権を横取り（プリエンプション）するようになるので
__プリエンプティブ・マルチタスク__ にすることができる。

```c
ISR(RTC_PIT_vect) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  TaskChanger::yield();
}

/* 64Hz 間隔でプリエンプション */
loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
RTC_PITINTCTRL = RTC_PI_bm;
RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC512_gc;
```

ただし、これは現在の不可分処理が中断されたくない場合でも実行権が他へ移りうるため、
用途によっては好ましくない結果になる。実装可否は慎重に吟味しなければならない。

> プリエンプション先のタスクで普通に動かせるのは、Lチカ程度だと考えても良い。

### 制約

1\. ```TimeoutTimer``` の ```TIMEOUT_BLOCK```
および ```sleep_cpu_ticks``` / ```sleep_cpu_timer```
実行中に ```yield()``` が呼ばれてもタスク切替はされない。
これらの内部処理は不可分である。

> 具体的には ```RTC_INTCTRL``` レジスタの ```RTC_CMP_bp``` ビットが 1 ならば
```yield()``` はタスクを切り替えない仕掛けになっている。

2\. 各タスクには登録時に個別の専用ローカルスタックメモリを必ず割り当てなければならない。
これは該当タスク実行中に不足するようなことが絶対にあってはならない。
スタックメモリ不足は __スタック・オーバーフロー__ を発生せしめ、CPU動作を不正な状態に招く。

3\. ```yield()``` はタスク切換えに少なくとも 140 CPUサイクルを消費する。
これは CPUが 1MHz 動作の場合は 140usの、20MHz動作なら 7us の遅延に相当する。
この間は他の割込実行は待たされる。
遅延が許されないタイムクリティカルな処理があるなら、
その間はタスク切換が発生しないように配慮しなければならない。

> ```TIMEOUT_BLOCK``` はタスク切換を抑止するので、プリエンプティブ実行時にもこれを援用できる。\
> 全体割込禁止中に明示的に ```yield()``` した場合、その割込可否は他のタスクに伝播しない。
それぞれのタスクは独自の割込可否状態を持つ。
元のタスクに戻ってきたら以前の割込可否に戻る。

4\. 異なるタスク間の変数共有は ```volatile``` 宣言を伴っていなければ保証されない。
かつその書換はクリティカル・セクションでなければならない。
プリエンプティブ・マルチタスクの場合は ```ATOMIC_BLOCK``` での保護が必須である。

> 各タスクはレジスタセットや全体割込状態を共有せず、それぞれが独自に保持している。\
> 一方、volatile の多用はコードの肥大、速度の低下を招く。その必要性は吟味すること。

5\. 再入不能コードの実行中はプリエンプティブ・マルチタスクを使用できない。
その理由や制限については AVR-LIBC FAQ が詳しいが、
再入不能コードは非ローカル変数や IOレジスタを状態保存に使っている。
割込による強制プリエンプションはこれを破壊しうるので期待した結果になりえない。
協調的マルチタスクでは比較的この問題に遭遇しにくいが、それでも例えば
割込＋リングバッファを用いたUART/I2C/SPI通信の遅延処理中にタスク切換がなされると
不具合を起こす可能性がある。それ自体が別の独立したタスクでもあるからだ。
これに対しては ```yield()``` 実行前は常に ```flush()``` を実行するのが定石だろう。

> [AVR-LIBC FAQ](https://cega.jp/avr-libc-jp/FAQ.html#faq_reentrant)

## 詳細

### void TaskChanger::attach_task_1st (volatile char \__local_stack[], size_t \__local_stack_size, void (*\__start_task)())

### void TaskChanger::attach_task_2nd (volatile char \__local_stack[], size_t \__local_stack_size, void (*\__start_task)())

### void TaskChanger::attach_task_3rd (volatile char \__local_stack[], size_t \__local_stack_size, void (*\__start_task)())

第1〜第3タスクのそれぞれにローカルスタックメモリ先頭番地と、その量と、タスク実行関数（void型）を関連付ける。
ローカルスタックメモリの確保には ```volatile``` 宣言を必要とする。
必要量はタスク実行内容に依存するが、完全に満足できる量でなければならない。

```c
volatile char task1_stack[64]; /* global */

TaskChanger::attach_task_1st(task1_stack, sizeof(task1_stack), &task1);
```

> 普通は少なくとも 64byte 以上だろう。```remaining_stack_XXX``` を参照のこと。

タスクは登録しただけでは実行されない。
タスク実行権の委譲は ```yield()``` が行う。

各タスク関数は ```return``` で終了するとそこで破棄され、
各エントリは未登録状態に戻る。
その後のローカルスタックメモリは再利用しても良い。

各タスクは自身の中でこれらを再実行した場合は次のような挙動となる。

- 実行中の自分自身のスタックを書き換えることは出来ず、無視される。
- 他のタスク設定変更はそれを上書きし、元のタスクには復帰せず、継続しない。次からは新たなタスクとなる。

### void TaskChanger::detach_task_1st (void)

### void TaskChanger::detach_task_2nd (void)

### void TaskChanger::detach_task_3rd (void)

現在実行中ではない該当タスクをエントリから取り除き、次回実行権も取り消す。
実行中の自分自身への指定は無視される。

```c
TaskChanger::detach_task_1st();
```

### bool TaskChanger::joined_task_1st (void)

### bool TaskChanger::joined_task_2nd (void)

### bool TaskChanger::joined_task_3rd (void)

対応するタスクが終了しているならば、真を返す。
該当エントリは未使用なので ```attach_task_XXX``` を再利用できる。

```c
if ( TaskChanger::joined_task_1st() ) {
  /* 1stタスクは終了済 */
}
```

> タスク関数は void 型であり、引数を渡すことも返すことも出来ない。
タスク間の変数受け渡しは volatile 宣言されたグローバル変数を用いることでのみ行える。

### size_t TaskChanger::remaining_stack_1st (volatile char \__local_stack[])

### size_t TaskChanger::remaining_stack_2nd (volatile char \__local_stack[])

### size_t TaskChanger::remaining_stack_3rd (volatile char \__local_stack[])

引数にローカルスタックメモリ先頭番地を与えると、
個々のタスクの現在のスタック残量（byte）を計算して返す。

参照されるのは ```yield()``` が呼ばれた時に保存されたスタックポインタなので、
結果はタスク切換に必要な最低量からの余裕値である。
ローカルスタックメモリ初期量はこれを目安に判断するとよいが、
あまりタイトに切り詰めると割込等による観測範囲外のスタック消費で溢れる場合もある。
故に 20〜30byte か それ以上の余裕を持たせるべきである。

```c
size_t remaining_stack_free_size = TaskChanger::remaining_stack_1st(task1_stack);
```

> スタックが不足（オーバーフロー）しても
セグメンテーションフォルトのような例外は発生しない。
実行環境は黙して破壊される。

### void TaskChanger::yield (void)

### void yield (void)

現在のタスクを中断し、全レジスタを保存し、次のタスクに実行権を移す。

名前空間のないほうはグローバルな ```yield```仮関数（weak属性登録）の書換であり
```delay()``` 関数等の内部から呼ばれる想定である。
他のAPI との併用で ```yield()``` 関数の実体が不明瞭な場合は
名前空間付きで使用するのがよい。

```c
TaskChanger::yield(); /* タスク実行権委譲 */
```

### #define NOTUSED_OVERWRITE_YIELD

このマクロを事前に定義すると ```yield()```関数のグローバル空間への割付を抑止する。
```yield()```関数は名前空間付きでのみ呼ぶことができるようになる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
