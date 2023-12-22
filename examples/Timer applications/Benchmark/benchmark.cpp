/**
 * @file benchmark.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <inttypes.h>
#include <util/atomic.h>
#include <api/capsule.h>
#include <variant.h>
#include "benchmark.h"

#if !defined(TCB_CASCADE_bm)
  #error This MCU not supported
    #include BUILD_STOP
#endif

namespace Benchmark {
  void init (void) {
    /* TCB0とTCB1の捕獲事象に接続 */
    EVSYS_CHANNEL4 = EVSYS_CHANNEL_OFF_gc;
    EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL4_gc;
    EVSYS_USERTCB1CAPT = EVSYS_USER_CHANNEL4_gc;

    /* ch5=TCB0溢れ*/
    /* TCB1の計数事象に接続 */
    EVSYS_CHANNEL5 = EVSYS_CHANNEL_TCB0_OVF_gc;
    EVSYS_USERTCB1COUNT = EVSYS_USER_CHANNEL5_gc;

    /* TCB1は計数捕獲周波数測定動作かつ連結上位 */
    TCB1_EVCTRL = TCB_CAPTEI_bm;
    TCB1_CTRLB = TCB_CNTMODE_FRQ_gc;
    TCB1_CTRLA = TCB_RUNSTDBY_bm | TCB_CASCADE_bm | TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm;

    /* TCB0は計数捕獲周波数測定動作かつ連結下位 */
    /* CLK元は主クロック（F_CPU）*/
    TCB0_EVCTRL = TCB_CAPTEI_bm;
    TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
    TCB0_CTRLA = TCB_RUNSTDBY_bm | TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
  }

  uint32_t test (void(*_testfunc)(void)) {
    uint32_t _count;

    /* テスト計測 : テスト関数1回分の実効クロックを得る */
    EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm; /* 計数捕獲（リセット）*/
    _testfunc();
    EVSYS_SWEVENTA = EVSYS_SWEVENTA_4_bm; /* 計数捕獲 */
    _CAPS32(_count)->words[0] = TCB0_CCMP;
    _CAPS32(_count)->words[1] = TCB1_CCMP;

    /* EVSYS(1)+RCALL(4)+RET(2) に要するオーバーヘッドを引く */
    /* これは6クロック未満になることはない（なる場合はインライン最適化を受けている）*/
    if (_count < 7) _count = 0;
    else _count -= 7;

    /* 実効クロック数を返す */
    return _count;
  }
}

// end of code


