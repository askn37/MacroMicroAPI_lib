# Blink_01_Basic.ino

このサンプルスケッチは以下について記述している；

- 基本的な遅延ループで"Blink"実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1系統
- modernAVR世代
  - AVR Dx系統
  - tinyAVR-2系統
- reduceAVR
  - ATtiny10

## Blinkの要件

- 1秒経過毎に出力信号を正負反転（Duty比50%の 0.5Hz周波数信号出力）
- 出力信号で LEDを消灯・点灯
- 以上を継続動作

## スケッチ記述

```c
void setup (void) {
  pinModeMacro(LED_BUILTIN, OUTPUT);
}

void loop (void) {
  digitalWriteMacro(LED_BUILTIN, TOGGLE);
  delay_millis(1000);
}
```

## 解説

通常のArduino互換APIでの`delay()`機能は（ほとんどの場合）
計時器と割込よるマイクロ秒/ミリ秒計数で実装されている。
その実装については
[[TCB計時機で"Blink"実演]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Blink%20variations/Blink_02_delay)
を参照のこと。

ここではもっと原始的な遅延ループを使用する。
この実装は次のようなものだ。

```c
void delay_millis (uint32_t _ms) {
  __builtin_avr_delay_cycles((F_CPU * _ms) / 1000L);
}
```

`__builtin_avr_delay_cycles(uint32_t)`は
avr-gcc固有のAVR関数で、指定した数量のCPUサイクルを単に消費するコードを生成する。

これを用いて`F_CPU=24000000L`であるときの
前述の`loop()`関数は
次のような機械語にコンパイルされるだろう。

```asm
; F_CPU = 24000000L == 1000ms

loop:   sbi  0x02, 7    ; digitalWriteMacro(LED_BUILTIN, TOGGLE)
        ldi  r18, 0xFF  ; R25:R24:R18 <-- (24000000L / 5) - 1
        ldi  r24, 0x3D
        ldi  r25, 0x49
delay:  subi r18, 0x01  ; R25:R24:R18 <-- R25:R24:R18 -1
        sbci r24, 0x00
        sbci r25, 0x00
        brne delay
        rjmp break
break:  nop
        rjmp loop
```

`_ms=1000`（ミリ秒）はそのまま`F_CPU`が示す
`24000000`CPUサイクルと解釈される。
これをその後の`delay`ループ一周に要する5CPUサイクルで割った
`4800000`を求めるが、これを3個のレジスタに格納するには
3CPUサイクルを要するので
1を引いた`4799999 -> 0x493dff`が実際に行うループ数だ。
最後に余った2CPUサイクルを消費すべく`rjmp`と`nop`を
実行することで辻褄を合わせ`24000000`CPUサイクル丁度の遅延が作られている。
実際にはこれに`LED_BUILTIN`出力を反転する1CPUサイクルと、
全体ループを繰り返す`rjmp`があるので真の全体ループは
`24000003`CPUカウントとなる。

遅延ループは制御されているものの、CPUは他に何をするでもなく時間だけを消費する。
その代わり他にいかなるメカニズムも必要としない単純作業なので、
ATtiny10 のような機能制約の多い MCUでも簡単かつ問題なく使用できる。

## 制約

`__builtin_avr_delay_cycles(uint32_t)`は
コンパイル時に事前決定されている静的定数を引数に受け取ることを前提としている。
これに変数で値を渡すと期待した結果にはならないだろう。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
