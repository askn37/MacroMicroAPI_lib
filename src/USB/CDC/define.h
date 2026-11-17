/**
 * @file USB/CDC/define.h
 * @author askn (K.Sato) multix.jp
 * @brief Implementation sample for the USB-CDC-ACM class.
 * @version 0.4.12
 * @date 2026-11-13
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#pragma once
#include <avr/io.h>
/* Compilation with anything other than the AVR-DU series will be rejected. */
#if defined(__cplusplus) && defined(USB0)
#include <stddef.h>
#include "api/macro_api.h"  /* interrupts and ATOMIC_BLOCK */
#include "../Descriptor.h"

#define PACKED __attribute__((__packed__))
#define WEAK   __attribute__((__weak__))
#define RODATA __attribute__((__progmem__))
#define NOINIT __attribute__((section(".noinit")))

#if defined(DEBUG)
#ifndef SerialDBG
#define SerialDBG Serial1C
#endif
#include <peripheral.h> /* from Micro_API : inport Serial (Debug) */
#define D1PRINTF(...) SerialDBG.printf(__VA_ARGS__)
#define D1FLUSH(...) SerialDBG.flush()
#if DEBUG == 2
#define D2PRINTF(...) SerialDBG.printf(__VA_ARGS__)
#else
#define D2PRINTF(...)
#endif
#else
#define D1PRINTF(...)
#define D2PRINTF(...)
#define D1FLUSH(...)
#endif

namespace USB_NAMESPACE {

  enum {

  /* bmRequestType */
    USB_REQTYPE_DIRECTION_bp          = 7
  , USB_REQTYPE_DIRECTION_bm          = (1 << 7)
  , USB_REQTYPE_TYPE_gm               = (3 << 5)
  , USB_REQTYPE_RECIPIENT_gm          = (31)
  , USB_RECIPIENT_DEVICE_gc           = (0 << 0)  // 0x00
  , USB_RECIPIENT_INTERFACE_gc        = (1 << 0)  // 0x01
  , USB_RECIPIENT_ENDPOINT_gc         = (2 << 0)  // 0x02
  , USB_RECIPIENT_OTHER_gc            = (3 << 0)  // 0x03
  , USB_REQTYPE_STANDARD_gc           = (0 << 5)  // 0x00
  , USB_REQTYPE_CLASS_gc              = (1 << 5)  // 0x20
  , USB_REQTYPE_VENDOR_gc             = (2 << 5)  // 0x40

  /* bRequest */
  , USB_REQ_GetStatus                 = 0
  , USB_REQ_ClearFeature              = 1
  , USB_REQ_SetFeature                = 3
  , USB_REQ_SetAddress                = 5
  , USB_REQ_GetDescriptor             = 6
  , USB_REQ_SetDescriptor             = 7
  , USB_REQ_GetConfiguration          = 8
  , USB_REQ_SetConfiguration          = 9
  , USB_REQ_GetInterface              = 10
  , USB_REQ_SetInterface              = 11
  , USB_REQ_SynchFrame                = 12

  /* bRequest */
  , CDC_REQ_SendEncapsulatedCommand   = 0x00
  , CDC_REQ_GetEncapsulatedResponse   = 0x01
  , CDC_REQ_SetLineEncoding           = 0x20
  , CDC_REQ_GetLineEncoding           = 0x21
  , CDC_REQ_SetLineState              = 0x22
  , CDC_REQ_SendBreak                 = 0x23
  , CDC_REQ_ResponeAvailable          = 0x00
  , CDC_REQ_SerialState               = 0x20

  /* USB_REQ_GetDescriptor / wValue */
  , USB_DATA_GetDescripter_Device     = (bDescriptorType_DEVICE           << 8)
  , USB_DATA_GetDescripter_Config     = (bDescriptorType_CONFIGURATION    << 8)
  , USB_DATA_GetDescripter_Qualifier  = (bDescriptorType_DEVICE_QUALIFIER << 8)
  , USB_DATA_GetString_Index_0        = ((bDescriptorType_STRING << 8) + 0)
  , USB_DATA_GetString_Index_1        = ((bDescriptorType_STRING << 8) + 1)
  , USB_DATA_GetString_Index_2        = ((bDescriptorType_STRING << 8) + 2)
  , USB_DATA_GetString_Index_3        = ((bDescriptorType_STRING << 8) + 3)
  , USB_DATA_GetString_Index_4        = ((bDescriptorType_STRING << 8) + 4)

  /* bRequest + bRequest = wRequestType */
  , USB_REQTYPE_GetStatus             = ((USB_REQ_GetStatus           << 8) | 0x80)   // 0x0080 DV,IF,EP
  , USB_REQTYPE_ClearFeature          = ((USB_REQ_ClearFeature        << 8))          // 0x0100 DV,IF,EP
  , USB_REQTYPE_SetFeature            = ((USB_REQ_SetFeature          << 8))          // 0x0300 DV,IF,EP
  , USB_REQTYPE_SetAddress            = ((USB_REQ_SetAddress          << 8))          // 0x0500 DV
  , USB_REQTYPE_GetDescriptor         = ((USB_REQ_GetDescriptor       << 8) | 0x80)   // 0x0680 DV
  , USB_REQTYPE_SetDescriptor         = ((USB_REQ_SetDescriptor       << 8))          // 0x0700 DV Optional
  , USB_REQTYPE_GetConfiguration      = ((USB_REQ_GetConfiguration    << 8))          // 0x0880 DV
  , USB_REQTYPE_SetConfiguration      = ((USB_REQ_SetConfiguration    << 8))          // 0x0900 DV
  , USB_REQTYPE_GetInterface          = ((USB_REQ_GetInterface        << 8) | 0x81)   // 0x0A81 IF
  , USB_REQTYPE_SetInterface          = ((USB_REQ_SetInterface        << 8) | 0x01)   // 0x0B01 IF
  , USB_REQTYPE_SynchFrame            = ((USB_REQ_SynchFrame          << 8) | 0x82)   // 0x0C82 EP
  , USB_REQTYPE_ClearFeature_EP       = ((USB_REQ_ClearFeature        << 8) | 0x02)   // 0x0102 EP

  , CDC_REQTYPE_SetLineEncoding       = ((CDC_REQ_SetLineEncoding     << 8) | 0x21)   // 0x2021 SET_CLASS_INT
  , CDC_REQTYPE_GetLineEncoding       = ((CDC_REQ_GetLineEncoding     << 8) | 0xA1)   // 0x21A1 GET_CLASS_INT
  , CDC_REQTYPE_SetLineState          = ((CDC_REQ_SetLineState        << 8) | 0x21)   // 0x2221 SET_CLASS_INT
  , CDC_REQTYPE_SendBreak             = ((CDC_REQ_SendBreak           << 8) | 0x21)   // 0x2321 SET_CLASS_INT
  };

  /* USB_Setup_Packet : 8 bytes */
  typedef struct {
    union {
      uint16_t  wRequestType;
      struct {
        uint8_t bmRequestType;
        uint8_t bRequest;
      };
    };
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
  } PACKED Setup_Packet;

  /* USB_Descriptor_Header : 2 bytes */
  typedef struct {
    union {
      struct {
        uint8_t  bLength;
        uint8_t  bDescriptorType;
      };
      struct {
        uint32_t dwValue;
      };
    };
  } PACKED Descriptor_Header;

  /*
   * CDC-ACM structers
   */

  enum LineEncoding_enum {
    StopBits_1   = 0
  , StopBits_1H  = 1
  , StopBits_2   = 2
  , Parity_None  = 0
  , Parity_Odd   = 1
  , Parity_Even  = 2
  , Parity_Mark  = 3
  , Parity_Space = 4
  };

  /* CDC Line Coding : 7 bytes */
  typedef struct {
    uint32_t dwDTERate   = 9600U;
    uint8_t  bCharFormat = StopBits_1;
    uint8_t  bParityType = Parity_None;
    uint8_t  bDataBits   = 8;   /* 5,6,7,8,16(9) */
  } PACKED LineEncoding_t;

  /* CDC SerialState : 2 bytes */
  typedef struct {              /* DTE    DCE       */
    bool     bRxCarrier  : 1;   /* DCD <- CD        */
    bool     bTxCarrier  : 1;   /* DSR <- ER or DTR */
    bool     bBreak      : 1;
    bool     bRingSignal : 1;   /*  RI <- RI        */
    bool     bFraming    : 1;   /* Frame Error   */
    bool     bParity     : 1;   /* Parity Error  */
    bool     bOverRun    : 1;   /* Overrun Error */
    bool     reserve     : 1;
    uint8_t  reserved;
  } PACKED SerialState_t;

  typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    SerialState_t wmSerialState;
  } PACKED Notification_SerialState_t;

  /* CDC ControlLineState : 1 bytes */
  typedef struct {
    bool     bStateDTR   : 1;   /* DTR -> DSR or DCD */
    bool     bStateRTS   : 1;   /* RTS -> CTS        */
  } PACKED LineState_t;

  /* USB InterfaceState : 16 bytes */
  typedef struct {
    register8_t  CONFIG   = 0;
    register8_t  SOF      = 0;
    register8_t  T1200    = 1;
    register8_t  reserve0 = 0;
    register16_t SENDCNT  = 0;
    register16_t RECVCNT  = 0;
    register16_t BREAKCNT = 0;
    register16_t TIMEOUT  = 1000;
    SerialState_t SerialState;
    union {
      LineEncoding_t LineEncoding = {};
      uint8_t sLineEncoding[sizeof(LineEncoding_t)];
    };
    union {
      LineState_t bmLineState;
      register8_t bLineState = 0;
    };
  } PACKED Interface_State;

} /* USB_CDC */

#endif
// end of header
