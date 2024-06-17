/**
 * @file USB/CDC/define,h
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#pragma once
#include <avr/io.h>
#if defined(__cplusplus) && defined(USB0)
#include <stddef.h>

#define PACKED __attribute__((packed))
#define WEAK   __attribute__((weak))
#define RODATA __attribute__((__progmem__))
#define NOINIT __attribute__((section(".noinit")))

#if defined(DEBUG)
#include <peripheral.h> /* from Micro_API : inport Serial (Debug) */
#define D1PRINTF(...) Serial.printf(__VA_ARGS__)
#if DEBUG == 2
#define D1PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define D2PRINTF(...)
#endif
#else
#define D1PRINTF(...)
#define D2PRINTF(...)
#endif

namespace USB_NAMESPACE {

  enum {
    USB_LANGUAGE_EN_US              = 0x0409
  , USB_LANGUAGE_JA_JP              = 0x0411

  /* bInterfaceNumber */
  , USB_INTERFACE_ID_CDC_CCI        = 0
  , USB_INTERFACE_ID_CDC_DCI        = 1

  /* bEndpointAddress */
  , USB_EP_DESCRIPTOR_DIR_IN_bm     = (1 << 7)
  , USB_EP_DESCRIPTOR_DIR_OUT_bm    = (0 << 7)

  /* bmRequestType */
  , USB_REQTYPE_DIRECTION_gp        = 7
  , USB_REQTYPE_DIRECTION_gm        = (1 << 7)
  , USB_REQTYPE_TYPE_gm             = 0x60
  , USB_REQTYPE_RECIPIENT_gm        = 0x1F
  , USB_RECIPIENT_DEVICE_gc         = (0 << 0)  // 0x00
  , USB_RECIPIENT_INTERFACE_gc      = (1 << 0)  // 0x01
  , USB_RECIPIENT_ENDPOINT_gc       = (2 << 0)  // 0x02
  , USB_RECIPIENT_OTHER_gc          = (3 << 0)  // 0x03
  , USB_REQTYPE_STANDARD_gc         = (0 << 5)  // 0x00
  , USB_REQTYPE_CLASS_gc            = (1 << 5)  // 0x20
  , USB_REQTYPE_VENDOR_gc           = (2 << 5)  // 0x40

  /* bmAttributes */
  , USB_CONFIG_ATTR_BUSPOWERED_bm   = (0 << 6)
  , USB_CONFIG_ATTR_SELFPOWERED_bm  = (1 << 6)
  , USB_CONFIG_ATTR_REMOTEWAKEUP_bm = (1 << 5)

  /* bmAttributes */
  , USB_EP_TYPE_CONTROL_gc          = (0 << 0)
  , USB_EP_TYPE_ISOCHRONOUS_gc      = (1 << 0)
  , USB_EP_TYPE_BULK_gc             = (2 << 0)
  , USB_EP_TYPE_INTERRUPT_gc        = (3 << 0)
  , USB_EP_ATTR_NOSYNC_gc           = (0 << 2)
  , USB_EP_ATTR_ASYNC_gc            = (1 << 2)
  , USB_EP_ATTR_ADAPTIVE_gc         = (2 << 2)
  , USB_EP_ATTR_SYNC_gc             = (3 << 2)
  , USB_EP_USAGE_DATA_gc            = (0 << 4)
  , USB_EP_USAGE_FEEDBACK_gc        = (1 << 4)
  , USB_EP_USAGE_IMPLICIT_gc        = (2 << 4)

  /* bRequest */
  , USB_REQ_GetStatus               = 0
  , USB_REQ_ClearFeature            = 1
  , USB_REQ_SetFeature              = 3
  , USB_REQ_SetAddress              = 5
  , USB_REQ_GetDescriptor           = 6
  , USB_REQ_SetDescriptor           = 7
  , USB_REQ_GetConfiguration        = 8
  , USB_REQ_SetConfiguration        = 9
  , USB_REQ_GetInterface            = 10
  , USB_REQ_SetInterface            = 11
  , USB_REQ_SynchFrame              = 12

  , USB_FEATURE_EndpointHalt_bm     = 0x00
  , USB_FEATURE_RemoteWakeup_bm     = 0x01
  , USB_FEATURE_TestMode_bm         = 0x02

  /* bRequest */
  , USB_DTYPE_Device                = 0x01
  , USB_DTYPE_Configuration         = 0x02
  , USB_DTYPE_String                = 0x03
  , USB_DTYPE_Interface             = 0x04
  , USB_DTYPE_Endpoint              = 0x05
  , USB_DTYPE_DeviceQualifier       = 0x06
  , USB_DTYPE_Other                 = 0x07
  , USB_DTYPE_InterfacePower        = 0x08
  , USB_DTYPE_InterfaceAssociation  = 0x0B
  , USB_DTYPE_CSInterface           = 0x24
  , USB_DTYPE_CSEndpoint            = 0x25

  , USB_CSCP_NoDeviceClass          = 0x00
  , USB_CSCP_NoDeviceSubclass       = 0x00
  , USB_CSCP_NoDeviceProtocol       = 0x00
  , USB_CSCP_VendorSpecificClass    = 0xFF
  , USB_CSCP_VendorSpecificSubclass = 0xFF
  , USB_CSCP_VendorSpecificProtocol = 0xFF
  , USB_CSCP_IADDeviceClass         = 0xEF
  , USB_CSCP_IADDeviceSubclass      = 0x02
  , USB_CSCP_IADDeviceProtocol      = 0x01

  , CDC_REQ_SendEncapsulatedCommand = 0x00
  , CDC_REQ_GetEncapsulatedResponse = 0x01
  , CDC_REQ_SetLineEncoding         = 0x20
  , CDC_REQ_GetLineEncoding         = 0x21
  , CDC_REQ_SetLineState            = 0x22
  , CDC_REQ_SendBreak               = 0x23
  , CDC_NOTIF_SerialState           = 0x20

  /* USB_REQ_GetDescriptor / wValue */
  , USB_DATA_GetDescripter_Device   = (USB_DTYPE_Device          << 8)
  , USB_DATA_GetDescripter_Config   = (USB_DTYPE_Configuration   << 8)
  , USB_DATA_GetDescripter_Qualifier= (USB_DTYPE_DeviceQualifier << 8)
  , USB_DATA_GetString_Index_0      = ((USB_DTYPE_String << 8) + 0)
  , USB_DATA_GetString_Index_1      = ((USB_DTYPE_String << 8) + 1)
  , USB_DATA_GetString_Index_2      = ((USB_DTYPE_String << 8) + 2)
  , USB_DATA_GetString_Index_3      = ((USB_DTYPE_String << 8) + 3)
  , USB_DATA_GetString_Index_4      = ((USB_DTYPE_String << 8) + 4)

  /* bRequest + bRequest = wRequestType */
  , USB_REQTYPE_GetStatus           = ((USB_REQ_GetStatus           << 8) | 0x80)   // 0x0080 DV,IF,EP
  , USB_REQTYPE_ClearFeature        = ((USB_REQ_ClearFeature        << 8))          // 0x0100 DV,IF,EP
  , USB_REQTYPE_SetFeature          = ((USB_REQ_SetFeature          << 8))          // 0x0300 DV,IF,EP
  , USB_REQTYPE_SetAddress          = ((USB_REQ_SetAddress          << 8))          // 0x0500 DV
  , USB_REQTYPE_GetDescriptor       = ((USB_REQ_GetDescriptor       << 8) | 0x80)   // 0x0680 DV
  , USB_REQTYPE_SetDescriptor       = ((USB_REQ_SetDescriptor       << 8))          // 0x0700 DV Optional
  , USB_REQTYPE_GetConfiguration    = ((USB_REQ_GetConfiguration    << 8))          // 0x0880 DV
  , USB_REQTYPE_SetConfiguration    = ((USB_REQ_SetConfiguration    << 8))          // 0x0900 DV
  , USB_REQTYPE_GetInterface        = ((USB_REQ_GetInterface        << 8) | 0x81)   // 0x0A81 IF
  , USB_REQTYPE_SetInterface        = ((USB_REQ_SetInterface        << 8) | 0x01)   // 0x0B01 IF
  , USB_REQTYPE_SynchFrame          = ((USB_REQ_SynchFrame          << 8) | 0x82)   // 0x0C82 EP
  , USB_REQTYPE_ClearFeature_EP     = ((USB_REQ_ClearFeature        << 8) | 0x02)   // 0x0102 EP

  , CDC_REQTYPE_SetLineEncoding     = ((CDC_REQ_SetLineEncoding     << 8) | 0x21)   // 0x2021 SET_CLASS_INT
  , CDC_REQTYPE_GetLineEncoding     = ((CDC_REQ_GetLineEncoding     << 8) | 0xA1)   // 0x21A1 GET_CLASS_INT
  , CDC_REQTYPE_SetLineState        = ((CDC_REQ_SetLineState        << 8) | 0x21)   // 0x2221 SET_CLASS_INT
  , CDC_REQTYPE_SendBreak           = ((CDC_REQ_SendBreak           << 8) | 0x21)   // 0x2321 SET_CLASS_INT
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
    uint8_t  bLength;
    uint8_t  bDescriptorType;
  } PACKED Descriptor_Header;

  /* USB_Device_Descriptor : 18 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
  } PACKED USB_Device_Descriptor;

  /* USB_DeviceQualifier_Descriptor (DQD) : 10 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint8_t  bNumConfigurations;
    uint8_t  bReserved;
  } PACKED USB_DeviceQualifier_Descriptor;

  /* USB_Configuration_Descriptor : 9 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
  } PACKED USB_Configuration_Descriptor;

  /* USB_Interface_Descriptor : 9 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
  } PACKED USB_Interface_Descriptor;

  /* USB_InterfaceAssociation_Descriptor (IAD) : 8 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bFirstInterface;
    uint8_t  bInterfaceCount;
    uint8_t  bFunctionClass;
    uint8_t  bFunctionSubClass;
    uint8_t  bFunctionProtocol;
    uint8_t  iFunction;
  } PACKED USB_InterfaceAssociation_Descriptor;

  /* USB_Endpoint_Descriptor : 7 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
  } PACKED USB_Endpoint_Descriptor;

  /* USB_String_Descriptor : 2 + 2 * bLength bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    wchar_t  wString[];
  } PACKED USB_String_Descriptor;

  /* USB_CDC_Header_Functional_Descriptor (HFD) : 5 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t CDCSpecification;
  } PACKED USB_CDC_Header_Functional_Descriptor;

  /* CDC_CallManagement_Functional_Descriptor (CFD) : 5 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmCapabilities;
    uint8_t  bDataInterface;
  } PACKED CDC_CallManagement_Functional_Descriptor;

  /* CDC_ACM_Functional_Descriptor (AFD) : 4 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmCapabilities;
  } PACKED CDC_ACM_Functional_Descriptor;

  /* CDC_Union_Functional_Descriptor (UFD) : 5 bytes */
  typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bMasterInterface;
    uint8_t  bSlaveInterface0;
  } PACKED CDC_Union_Functional_Descriptor;

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
    uint32_t dwDTERate;
    uint8_t  bCharFormat;
    uint8_t  bParityType;
    uint8_t  bDataBits;
  } PACKED LineEncoding_t;

  /* CDC SerialState : 2 bytes */
  typedef struct {
    bool     bRxCarrier  : 1;
    bool     bTxCarrier  : 1; /* DSR<-DTR */
    bool     bBreak      : 1;
    bool     bRingSignal : 1;
    bool     bFraming    : 1;
    bool     bParity     : 1;
    bool     bOverRun    : 1;
    bool     reserve     : 1;
    uint8_t  reserved;
  } PACKED SerialState_t;

  typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    SerialState_t data;
  } PACKED Notification_SerialState_t;

  /* CDC ControlLineState : 1 bytes */
  typedef struct {
    bool     bStateDTR   : 1;
    bool     bStateRTS   : 1;
  } PACKED LineState_t;

  /* USB InterfaceState : 16 bytes */
  typedef struct {
    SerialState_t SerialState;
    union {
      uint8_t sLineEncoding[sizeof(LineEncoding_t)];
      LineEncoding_t LineEncoding = {
        9600UL, StopBits_1, Parity_None, 8
      };
    };
    union {
      register8_t bLineState;
      LineState_t bmLineState;
    };
    register8_t  CONFIG;
    register16_t RECVCNT;
    register16_t SENDCNT;
    register16_t BREAKCNT;
    register16_t TIMEOUT = 1000;
  } PACKED Interface_State;

} /* USB_CDC */

#endif
// end of header
