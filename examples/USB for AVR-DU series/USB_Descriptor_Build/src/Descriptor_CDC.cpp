/**
 * @file Descriptor_CDC.cpp
 * @author askn (K.Sato) multix.jp
 * @brief USB Descriptor Sample
 * @version 0.1
 * @date 2026-11-10
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#include <avr/io.h>
#include "USB_Utils.h"
#include "USB/Descriptor.h"
#include "../usrdef.h"

namespace {

  /*** WARNING ***
   * This USB device information applies only when using Microchip Technology Inc. products.
   * It must be modified when porting to other companies' products.
   */
  #define USB_VENDOR_ID   0x04D8  /* 0x04D8=MCHP VID. */
  #define USB_PRODUCT_ID  0x0B15  /* PID 0x0010-0x002F reserved for testing/non-public demos. */

  /* All strings are concatenated and written in ASCIIZ format. */
  /* It is dynamically converted to UTF16-LE format. */
  #define USB_STR_1 "Microchip Technology Inc."
  #define USB_STR_2 "USB-CDC-ACM-VCP for AVR-DU series"
  #define USB_DESC_STRS USB_STR_1 USB_STR_2

  /* We decide which NVM to place the generated results in. */
  // #define USB_DESC_MEM 0

  #ifndef USB_DESC_MEM
    #if (__AVR_ARCH__ == 103)
      #define USB_DESC_MEM 1
    #else
      #define USB_DESC_MEM 0
    #endif
  #endif

  #if (USB_DESC_MEM == 1)   /* 1:DATA */
    #define USB_DESC_ATTR
    #define USB_DESC_ADDR 0
  #elif (USB_DESC_MEM == 2) /* 2:EEPROM */
    #define USB_DESC_ATTR __attribute__((__eeprom__))
    #define USB_DESC_ADDR EEPROM_START
  #elif (USB_DESC_MEM == 3) /* 3:USERROW */
    #define USB_DESC_ATTR __attribute__((section(".user_signatures")))
    #define USB_DESC_ADDR USER_SIGNATURES_START
  #elif (USB_DESC_MEM == 4) /* 4:BOOTROW */
    #define USB_DESC_ATTR __attribute__((section(".boot_signatures")))
    #define USB_DESC_ADDR BOOTROW_START
  #else
    #define USB_DESC_ATTR __attribute__((__progmem__))
    #define USB_DESC_ADDR 0
  #endif

  /**********************
   HID Descriptor Section
  **********************/

  /* HID_REPORT is defined as a macro to determine the total length. */

  /**********************
   USB Descriptor Section
  **********************/

  const USB_Device_Descriptor_t USB_Device_Descriptor USB_DESC_ATTR = {
      .bLength              = sizeof(USB_Device_Descriptor_t)
    , .bDescriptorType      = bDescriptorType_DEVICE
    , .bcdUSB               = bcdUSB_2
    , .bDeviceClass         = bClassCode_Communication_Device
    , .bDeviceSubClass      = bSubClassCode_Composite
    , .bDeviceProtocol      = bProtocolCode_Composite
    , .bMaxPacketSize0      = 64
    , .idVendor             = USB_VENDOR_ID
    , .idProduct            = USB_PRODUCT_ID
    , .bcdDevice            = 0x0001
    , .iManufacturer        = 1         // sManufacturer
    , .iProduct             = 2         // sProduct
    , .iSerialNumber        = 0xFF      // automatic generation
    , .bNumConfigurations   = 1
  };

  /* A structure template is required to determine */
  /* the total length of the General Configuration Set. */

  /* Configuration Set #0 */
  typedef struct {
    USB_Configuration_Descriptor_t                USB_Configuration_Descriptor_0;
    USB_Interface_Association_Descriptor_t        USB_Interface_Association_Descriptor_1;
    USB_Interface_Descriptor_t                    USB_Interface_Descriptor_1;
      USB_Header_Functional_Descriptor_t          USB_Header_Functional_Descriptor_1;
      USB_ACM_Functional_Descriptor_t             USB_ACM_Functional_Descriptor_1;
      USB_Union_Functional_Descriptor_t           USB_Union_Functional_Descriptor_1;
                                          uint8_t bSubordinateInterface_1[1];
      USB_Call_Management_Functional_Descriptor_t USB_Call_Management_Functional_Descriptor_1;
      USB_Endpoint_Descriptor_t                   USB_Endpoint_Descriptor_81;
    USB_Interface_Descriptor_t                    USB_Interface_Descriptor_2;
      USB_Endpoint_Descriptor_t                   USB_Endpoint_Descriptor_82;
      USB_Endpoint_Descriptor_t                   USB_Endpoint_Descriptor_02;
  } USB_Genelal_Configuration_Descriptor_0_t;

  const USB_Genelal_Configuration_Descriptor_0_t USB_Genelal_Configuration_Descriptor_0 USB_DESC_ATTR = {
    .USB_Configuration_Descriptor_0 = {
        .bLength                = sizeof(USB_Configuration_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_CONFIGURATION
      , .wTotalLength           = sizeof(USB_Genelal_Configuration_Descriptor_0_t)
      , .bNumInterfaces         = 2     // Interface x 2
      , .bConfigurationValue    = 1     // Configuration#1
      , .iConfiguration         = 3     // iStrings
      , .bmAttributes           = 0x80
      , .bMaxPower              = 100 / 2
    },
    .USB_Interface_Association_Descriptor_1 = {
        .bLength                = sizeof(USB_Interface_Association_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_INTERFACE_ASSOCIATION
      , .bFirstInterface        = 1     // Interface#1
      , .bInterfaceCount        = 2     // Interface x 2
      , .bFunctionClass         = bClassCode_Communication_Device
      , .bFunctionSubClass      = bSubClassCode_Abstract_Model
      , .bFunctionProtocol      = bProtocolCode_AT_Commands
      , .iFunction              = 3     // iStrings
    },
    .USB_Interface_Descriptor_1 = {
        .bLength                = sizeof(USB_Interface_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_INTERFACE
      , .bInterfaceNumber       = 1     // Interface#1
      , .bAlternateSetting      = 0     // not used
      , .bNumEndpoints          = 1     // 0x81
      , .bInterfaceClass        = bClassCode_Communication_Device
      , .bInterfaceSubClass     = bSubClassCode_Common
      , .bInterfaceProtocol     = bProtocolCode_Composite
      , .iInterface             = 3     // iStrings
    },
    .USB_Header_Functional_Descriptor_1 = {
        .bLength                = sizeof(USB_Header_Functional_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_CS_INTERFACE
      , .bDescriptorSubtype     = bDescriptorSubtype_Header
      , .bcdCDC                 = 0x110 // Revision 1.10
    },
    .USB_ACM_Functional_Descriptor_1 = {
        .bLength                = sizeof(USB_ACM_Functional_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_CS_INTERFACE
      , .bDescriptorSubtype     = bDescriptorSubtype_Abstract_Control_Management
      , .bmCapabilities         = bmACM_SEND_BREAK | bmACM_SERIAL_STATE
    },
    .USB_Union_Functional_Descriptor_1 = {
        .bLength                = sizeof(USB_Union_Functional_Descriptor_t) + 1
      , .bDescriptorType        = bDescriptorType_CS_INTERFACE
      , .bDescriptorSubtype     = bDescriptorSubtype_Union
      , .bControlInterface      = 1     // length
    },
    .bSubordinateInterface_1 = {2},     // USB_Interface_Descriptor_2
    .USB_Call_Management_Functional_Descriptor_1 = {
        .bLength                = sizeof(USB_Call_Management_Functional_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_CS_INTERFACE
      , .bDescriptorSubtype     = bDescriptorSubtype_Call_Management
      , .bmCapabilities         = 0     // or 3
      , .bDataInterface         = 2     // to USB_Interface_Descriptor_2
    },
    .USB_Endpoint_Descriptor_81 = {
        .bLength                = sizeof(USB_Endpoint_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_ENDPOINT
      , .bEndpointAddress       = 0x81
      , .bmAttributes           = bmEP_Type_Interrupt
      , .wMaxPacketSize         = 64
      , .bInterval              = 4
    },
    .USB_Interface_Descriptor_2 = {
        .bLength                = sizeof(USB_Interface_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_INTERFACE
      , .bInterfaceNumber       = 2     // Interface#2
      , .bAlternateSetting      = 0     // not used
      , .bNumEndpoints          = 2     // 0x83 + 0x03
      , .bInterfaceClass        = bClassCode_Communications_Data
      , .bInterfaceSubClass     = bSubClassCode_Composite
      , .bInterfaceProtocol     = bProtocolCode_Composite
      , .iInterface             = 3     // iStrings
    },
    .USB_Endpoint_Descriptor_82 = {
        .bLength                = sizeof(USB_Endpoint_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_ENDPOINT
      , .bEndpointAddress       = 0x82
      , .bmAttributes           = bmEP_Type_Bulk
      , .wMaxPacketSize         = 64
      , .bInterval              = 0
    },
    .USB_Endpoint_Descriptor_02 = {
        .bLength                = sizeof(USB_Endpoint_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_ENDPOINT
      , .bEndpointAddress       = 0x02
      , .bmAttributes           = bmEP_Type_Bulk
      , .wMaxPacketSize         = 64
      , .bInterval              = 0
    }
  };

  /**************************
   List-up Descriptor Section
  **************************/

  typedef struct {
    uint8_t head_0[3];    uint8_t body_0[sizeof(USB_DESC_STRS)];  // end null is head_1[1]
  /*uint8_t head_1[1];*/  USB_Device_Descriptor_t body_1;
    uint8_t head_2[1];    USB_Genelal_Configuration_Descriptor_0_t body_2;
    uint8_t string_1[4];
    uint8_t string_2[4];
    uint8_t string_3[4];
    uint8_t string_0[3];  uint16_t wLAGID[1];
  } USB_Descriptor_t;

  /* The elements defined so far are then assembled into a single table. */
  /* It's impossible to achieve this automatically using only C/C++ macros. */
  /* Please refer to the relevant documentation for details. */
  const USB_Descriptor_t USB_Descriptor USB_DESC_ATTR = {
    /* Strings store */
      {0,  1 + sizeof(USB_DESC_STRS), bDescriptorType_NONCE}
    ,      USB_DESC_STRS
    /* Device */
    ,      USB_Device_Descriptor
    /* Configuration */
    , {0}, USB_Genelal_Configuration_Descriptor_0
    /* Strings */
    , {1,  sizeof(USB_STR_1)-1, bDescriptorType_STRING, 3}
    , {2,  sizeof(USB_STR_2)-1, bDescriptorType_STRING, 3 + sizeof(USB_STR_1)-1}
    , {3,                   11, bDescriptorType_STRING, 3 + sizeof(USB_STR_1)-1 + 4}
    /* Strings LANGID's : Table Terminator */
    , {0, 4, bDescriptorType_STRING}, {wLANG_EN_US}
  };

  extern "C" {
  // const uint8_t* USB_DescriptorSet = (uint8_t*)&USB_Descriptor + USB_DESC_ADDR;
  // const size_t   USB_DescriptorSetSize = sizeof(USB_Descriptor);
  // const uint8_t  USB_DescriptorSetSection = USB_DESC_ADDR;

    extern const uint8_t* USB_Descriptor_CDC_Set;
    extern const size_t   USB_Descriptor_CDC_SetSize;
    extern const uint8_t  USB_Descriptor_CDC_SetSection;
  }
  const uint8_t* USB_Descriptor_CDC_Set = (uint8_t*)&USB_Descriptor + USB_DESC_ADDR;
  const size_t   USB_Descriptor_CDC_SetSize = sizeof(USB_Descriptor);
  const uint8_t  USB_Descriptor_CDC_SetSection = USB_DESC_ADDR;

} // anonymous namespace

// end of code