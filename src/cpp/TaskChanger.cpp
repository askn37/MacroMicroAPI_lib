/**
 * @file TaskChanger.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <avr/io.h>
#include <util/atomic.h>
#include <api/capsule.h>
#include "../TaskChanger.h"

#if !defined(NOTUSED_OVERWRITE_YIELD)
void yield (void) {
  TaskChanger::yield();
}
#endif

namespace TaskChanger {
  volatile TaskChanger_work_t __worker = {0, (char*)-1, (char*)-1, (char*)-1, 0};

  __attribute__((naked,used)) void yield (void) {
    __asm__ __volatile__ ( R"#ASM#(
        PUSH    R1              ; enter function
        IN      R1, __SREG__    ; load SREG
        PUSH    R1              ; keep SREG
    ; TIMEOUT_BLOCK実行中（RTC_CMP_bp==1）はタスクを切り替えない
        LDS     R1, %2          ; R1 := RTC_INTCTRL
        SBRC    R1, %3          ; check bit RTC_CMP_bp
        RJMP    3f              ; ENABLE bit is all skip
    ; 割込を禁止して残りの全レジスタを保存
        CLI                     ; change SREG
        PUSH    R0              ; keep all
        PUSH    R2              ;
        PUSH    R3              ;
        PUSH    R4              ;
        PUSH    R5              ;
        PUSH    R6              ;
        PUSH    R7              ;
        PUSH    R8              ;
        PUSH    R9              ;
        PUSH    R10             ;
        PUSH    R11             ;
        PUSH    R12             ;
        PUSH    R13             ;
        PUSH    R14             ;
        PUSH    R15             ;
        PUSH    R16             ;
        PUSH    R17             ;
        PUSH    R18             ;
        PUSH    R19             ;
        PUSH    R20             ;
        PUSH    R21             ;
        PUSH    R22             ;
        PUSH    R23             ;
        PUSH    R24             ;
        PUSH    R25             ;
        PUSH    R26             ;
        PUSH    R27             ;
        PUSH    R28             ;
        PUSH    R29             ;
        PUSH    R30             ;
        PUSH    R31             ;
        IN      R24, __SP_L__   ; SPL 取得
        IN      R25, __SP_H__   ; SPH 取得
1:      LDI     YL, lo8(%0)     ; スタック管理先頭番地
        LDI     YH, hi8(%0)     ;
        LDD     XL, Y+%1        ; スタック管理指標
        CLR     XH              ;
        MOVW    ZL, YL          ; 作業ワードレジスタに管理先頭番地複製
        ADD     ZL, XL          ; オフセット加算
        ADC     ZL, XH          ;
        STD     Z+0, R24        ; SPL 保存
        STD     Z+1, R25        ; SPH 保存
2:      ADIW    XL, 2           ; 次の指標（線形探索）
        ANDI    XL, 7           ; 指標範囲制限（オフセット0,2,4,6のラウンドロビン）
        MOVW    ZL, YL          ; 作業ワードレジスタに管理先頭番地複製
        ADD     ZL, XL          ; オフセット加算
        ADC     ZH, XH          ;
        LDD     R25, Z+1        ; 保存されている SPH 取得
        SBRC    R25, 7          ; SPH の MSB をテスト
        RJMP    2b              ; MSB がセットならタスク無効なので次の指標をテスト
        STD     Y+%1, XL        ; 選択された指標を保存
        LDD     R24, Z+0        ; 管理に保存されている SPL 取得
        OUT     __SP_L__, R24   ; SPL 復帰
        OUT     __SP_H__, R25   ; SPH 復帰
    ; 復元したスタックから全レジスタを戻す
        POP     R31             ; restore all
        POP     R30             ;
        POP     R29             ;
        POP     R28             ;
        POP     R27             ;
        POP     R26             ;
        POP     R25             ;
        POP     R24             ;
        POP     R23             ;
        POP     R22             ;
        POP     R21             ;
        POP     R20             ;
        POP     R19             ;
        POP     R18             ;
        POP     R17             ;
        POP     R16             ;
        POP     R15             ;
        POP     R14             ;
        POP     R13             ;
        POP     R12             ;
        POP     R11             ;
        POP     R10             ;
        POP     R9              ;
        POP     R8              ;
        POP     R7              ;
        POP     R6              ;
        POP     R5              ;
        POP     R4              ;
        POP     R3              ;
        POP     R2              ;
        POP     R0              ;
3:      POP     R1              ;
        OUT     __SREG__, R1    ; restore SREG
        POP     R1              ; restore work
        RET                     ; leave function
__TaskChanger_detach_task:
    ; 終了タスクをスタック管理テーブルから削除する
        LDI     R25, 0xFF       ; スタック未使用（MSB）をマーク
        RJMP    1b
      )#ASM#"
      ::"p" (_SFR_MEM_ADDR(TaskChanger::__worker.stack))
      , "n" (_SFR_MEM_ADDR(TaskChanger::__worker.current) - _SFR_MEM_ADDR(TaskChanger::__worker.stack))
      , "p" (_SFR_MEM_ADDR(RTC_INTCTRL))
      , "n" (RTC_CMP_bp)
      : "memory"
    );
  }

  /*
   * param: タスク番号 1-3（0は親で指定禁止）
   * param: スタック配列
   * param: その大きさ
   * param: タスク開始関数
   */
  void attach_task (uint8_t __task_index, volatile char __local_stack[], size_t __local_stack_size, void (*__start_task)()) {
    assert(1 <= __task_index && __task_index <= 3);
    assert(64 <= __local_stack_size);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      /* ローカルスタックを初期化する */
      /* スタック最上位は 終わったタスクを片付ける処理の番地（BigEndian） */
      uint16_t __temp;
      __asm__ __volatile__ ( R"#ASM#(
          LDI     %A0, lo8(pm(__TaskChanger_detach_task)) ;
          LDI     %B0, hi8(pm(__TaskChanger_detach_task)) ;
        )#ASM#"
        : "=r" (__temp)
      );
      __local_stack[__local_stack_size - 1] = __temp;
      __local_stack[__local_stack_size - 2] = __temp >> 8;
      /* 実行開始番地 */
      __local_stack[__local_stack_size - 3] = (uint16_t)__start_task;
      __local_stack[__local_stack_size - 4] = (uint16_t)__start_task >> 8;
      /* R1 の初期値 */
      __local_stack[__local_stack_size - 5] = 0; // R1 == __zero_reg__
      /* SREG の初期値 */
      __local_stack[__local_stack_size - 6] = 0x80; // local SREG == sei()
      /* SP を計算 */
      __worker.stack[__task_index] = (char*)__local_stack + __local_stack_size - (32 + 6);
    }
  }

  bool joined_task (uint8_t __task_index) {
    return ((uint16_t)(__worker.stack[__task_index]) & 0x8000);
  }

  void detach_task (uint8_t __task_index) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _CAPS16(__worker.stack[__task_index])->bytes[1] = 0x80;
    }
  }

  size_t remaining_stack (uint8_t __task_index, volatile char __local_stack[]) {
    return (size_t)__worker.stack[__task_index] - (size_t)__local_stack;
  }
}

// end of code
