# SerialUSB_Echo_and_Event.ino

このサンプルスケッチは以下について記述している；

- SerialUSBの基本的な実演：イベント表示付き

## 対象AVR

- modernAVR世代
  - AVR-DU系統専用

## 説明

この実演サンプルは、[SerialUSBの基本的な実演](../SerialUSB_Echo/README.md) にイベント状態表示と、UARTデバッグ表示を加えたものだ。

USBコンソール側は、ArduinoIDEのシリアルモニタで開いている時に（左下のドロップリストで）速度変更をしてみると、イベント通知の効果が視覚化される。
そしてUSBホストとデバイス間においては、その速度指定とは無関係に常時最高速度が得られていることも容易に理解できるだろう。

デバッグUARTポート出力を使用するには`DEBUG=1`あるいは`DEBUG=2`設定を与えてビルドしなければならない。速度設定`CONSOLE_BAUD`は 400kbps〜500kbps（`400000L`〜`500000L`）の設定を推奨する。

## イベントコールバック関数

各種の発生イベントは、割込ルーチン中からコールバック関数を介して、ユーザーコードにリアルタイムで伝達される。
`USB::CDC`名前空間に属するそれぞれの規定のコールバック関数には __weak__ 属性が付されているので、ユーザーコードで任意に上書き実体登録ができる。
受け渡し変数には __volatile__ 属性を付加すべきだ。また割込中での呼び出しであるため、実行コードは必要最小限としなけれればならない。

以下に再登録可能なコールバック関数のリストを示す。これらのプロトタイプ宣言は`<USB/CDC.h>`で見ることができる。

- void cb_bus_event_sof (void);
- void cb_bus_event_suspend (void);
- void cb_bus_event_resume (void);
- void cb_bus_event_reset (void);
- void cb_bus_event_stalled (void);
- void cb_bus_event_underflow (void);
- void cb_bus_event_overflow (void);
- void cb_bus_event_start (void);
- void cb_bus_event_stop (void);
- bool cb_bus_check (void);
- void cb_cdc_set_lineencoding (LineEncoding_t* _lineencoding);
- void cb_cdc_set_linestate (LineState_t* _linestate);
- void cb_cdc_set_sendbreak (uint16_t _sendbreak);
- void cb_cdc_clear_sendbreak (void);

これらの再定義に`USB::CDC`名前空間内に記述する。

```cpp
volatile uint16_t _ss = 0;
namespace USB::CDC {
  void cb_cdc_set_sendbreak (uint16_t _sendbreak) { _ss = _sendbreak; }
}
```

### void cb_bus_event_sof (void)

SOFトークン受信通知。この割込が有効であれば、少なくとも 1ms に1回以上通知される。
バルク転送中は 1ms に10回以上発生する場合もある。

ただし`USB::CDC/CDC.h`実装においてはそれが必要な場合しか SOF割込を有効にしないため、これが期待通り動作することはない。

### void cb_bus_event_suspend (void)

SUSPEND遷移通知。これを受信したら、USBデバイスは可及的速やかに低電力状態へ移行すべきである。
バスパワー装置に対しては 2mA 以下の電流しか供給されなくなる可能性がある。

> VBUS状態取得`cb_bus_check()`を別途実装しないのであれば、利用価値はない。

### void cb_bus_event_resume (void)

RESUME遷移通知。USBデバイスは通常電力状態への復帰を試みることができる。
これに先立って`USB::CDC`実装は USB通信の再開を開始する。

> VBUS状態取得`cb_bus_check()`を別途実装しないのであれば、利用価値はない。

- `USB::CDC`実装においては USB通信線リセット検出と区別されず、現在の USBアドレスは破棄され、初期化エニュメレーションの開始準備を行う。

### void cb_bus_event_reset (void)

USB通信線リセット検出通知。
これに先立って`USB::CDC`実装は現在割り当てられている USBアドレスを破棄し、初期化エニュメレーションの開始準備を行う。

### void cb_bus_event_stalled (void)

USB通信線失速状態検出通知。`USB::CDC`実装（およびその機能拡張）が`STALLED`状態をホストに通達すると、それが解消されるまで継続して通知される可能性がある。

### void cb_bus_event_underflow (void)

バッファアンダーフロー発生通知。入力エンドポイントにて`NAK`状態が継続すると生じる。

### void cb_bus_event_overflow (void)

バッファオーバーフロー発生通知。出力エンドポイントにて`NAK`状態が継続すると生じる。

### void cb_bus_event_start (void)

通信開始状態通知。アプリケーションが USB-CDC通信路を使用可能な状態になると生じる。

### void cb_bus_event_stop (void)

通信停止状態通知。アプリケーションが USB-CDC通信路を使用不可の状態になると生じる。

### bool cb_bus_check (void)

VBUS状態取得要求。VBUS電圧が規定範囲内なら true を返却すべきである。
これが false であれば、`read`は常に -1、`write` は常に 0を返して、失敗するだろう。
既定では VBUS状態確認機能を持たないので、常に true を返す weak 関数で代替されている。

### void cb_cdc_set_lineencoding (LineEncoding_t* _lineencoding)

LineEncoding変更要求。ホストから送られた `LineEncoding_t` 構造体へのポインタが渡される。
この構造体のメンバーは以下の通り。

```cpp
void cb_cdc_set_lineencoding (LineEncoding_t* _lineencoding) {
  uint32_t baudrate   = _lineencoding->dwDTERate;   // 9600UL、115200UL など
  uint8_t  formatt    = _lineencoding->bCharFormat; // StopBits_1、StopBits_2
  uint8_t  parity     = _lineencoding->bParityType; // Parity_None、Parity_Even など
  uint8_t  databits   = _lineencoding->bDataBits;   // 5、7、8、9、16 など
}
```

Windowsでも macosでも、このイベントは連続して複数回（一般に4回以上）発生する。
それらは連続して同じ値であることも、最後の1回だけが異なることもある。
したがってユーザーアプリケーション側には、重複した要求を省く実装が必要になるかもしれない。

### void cb_cdc_set_linestate (LineState_t* _linestate)

LineState変更要求。ホストから送られた `LineState_t` 構造体へのポインタが渡される。
この構造体のメンバーは以下の通り。

```cpp
void cb_cdc_set_linestate (LineState_t* _linestate) {
  bool dtr = _linestate->bStateDTR;   // DTR出力状態を変更ずべき 0=HIGH、1=LOW
  bool rts = _linestate->bStateRTS;   // RTS出力状態を変更ずべき 0=HIGH、1=LOW
}
```

### void cb_cdc_set_sendbreak (uint16_t _sendbreak)

ブレーク状態変更要求。ホストから送られたブレーク信号継続時間（ミリ単位）が渡される。
ただし 0xFFFF（65535）は次のブレーク状態解除要求があるまで、継続しなければならない。
他はアイドル信号状態に戻すまでの時間を指定する。

- ここで言う BREAK とは、物性的には（転送先の）UART-TxD 出力を`LOW`状態で維持すべきことを示す。

### void cb_cdc_clear_sendbreak (void)

ブレーク状態解除要求。直ちにブレーク信号状態を解除して、アイドル信号状態に復するべき。

- ここで言う IDOL とは、物性的には（転送先の）UART-TxD 出力を`HIGH`またはオープンドレイン解放状態にすべきことを示す。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
