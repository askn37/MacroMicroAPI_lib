# Blink_09_TaskChanger.ino

このサンプルスケッチは以下について記述している；

- TaskChangerで"Blink"実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

これは
[TaskChanger](https://github.com/askn37/askn37.github.io/wiki/TaskChanger)
を用いた "Blink" の実演だ。
協調的マルチタスクの最も単純な記述（雛形）でもある。

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
#include <TaskChanger.h>

volatile char blink_stack[64];
void yield (void) { TaskChanger::yield(); }

void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
  // Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  TaskChanger::attach_task_1st(blink_stack, sizeof(blink_stack), &blink);
  // TaskChanger::attach_task_2nd(blink_stack, sizeof(blink_stack), &blink);
  // TaskChanger::attach_task_3rd(blink_stack, sizeof(blink_stack), &blink);
}

void blink (void) {
  while (true) {
    digitalWriteMacro(LED_BUILTIN, TOGGLE);
    delay(1000);
  }
}

void loop (void) {
  yield();
}
```

## 解説

`<TaskChanger.h>`は`loop()`以外に3個までのタスク関数を実行できるが、
このレベルまでスケッチを単純化するとコンパイラの最適化が影響して不具合が生じる。

通常、未登録のタスクは何ら害を及ぼさないが、
この例では`attach_task_1st()`を
`attach_task_2nd()`
あるいは`attach_task_3rd()`
に書き換えると正しく動作しなくなって、無限にリセット動作を繰り返す。

この症状は`Serial`初期化行のコメントアウトを外すと再現しなくなることが確認できる。
症状が発生するのは、周辺機能を扱う有効な実行コードがひとつも記述されていない時に限られるようだ。
そういうことは普段起こり得ないが、`<MacroAPI.h>`だけを使って GPIOしか触らない場合に顕在化することがあり得る。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
