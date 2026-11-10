/**
 * @file Descriptor_EDBG.cpp
 * @author askn (K.Sato) multix.jp
 * @brief 
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
  #define USB_PRODUCT_ID  0x0B12  /* PID 0x0010-0x002F reserved for testing/non-public demos. */

  /* All strings are concatenated and written in ASCIIZ format. */
  /* It is dynamically converted to UTF16-LE format. */
  #define USB_STR_1 "euboot:CMSIS-DAP:EDBG"
  #define USB_DESC_STRS USB_STR_1

  /* We decide which NVM to place the generated results in. */
  // #define USB_DESC_MEM 0

  #ifndef USB_DESC_MEM
    #if (PROGMEM_SIZE <= 32768UL)
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

  /* CMSIS-DAP/EDBG (Full-Speed 64 byte payload) */
  #define HID_Report_Descriptor_0 { \
    HR_wUsage_Page(0xFF00), \
    HR_bUsage(1), \
    HR_bCollection(1), \
      HR_bLogical_Minimum_0, \
      HR_wLogical_Maximum(255), \
      HR_bReport_Size(8), \
      HR_bReport_Count(64), \
      HR_bUsage(1), \
      HR_bInput(2), \
      HR_bUsage(1), \
      HR_bOutput(2), \
      HR_bReport_Count(4), \
      HR_bUsage(1), \
      HR_bFeature(2), \
    HR_EndCollection \
  }
  #define HID_Report_Descriptor_0_temp (uint8_t[])HID_Report_Descriptor_0

  /**********************
   USB Descriptor Section
  **********************/

  const USB_Device_Descriptor_t USB_Device_Descriptor USB_DESC_ATTR = {
      .bLength              = sizeof(USB_Device_Descriptor_t)
    , .bDescriptorType      = bDescriptorType_DEVICE
    , .bcdUSB               = bcdUSB_2
    , .bDeviceClass         = bClassCode_Composite
    , .bDeviceSubClass      = bSubClassCode_Composite
    , .bDeviceProtocol      = bProtocolCode_Composite
    , .bMaxPacketSize0      = 64
    , .idVendor             = USB_VENDOR_ID
    , .idProduct            = USB_PRODUCT_ID
    , .bcdDevice            = 0x0372
    , .iManufacturer        = 1         // sManufacturer
    , .iProduct             = 2         // sProduct
    , .iSerialNumber        = 0xFF      // automatic generation
    , .bNumConfigurations   = 1
  };

  const USB_Device_Qualifier_Descriptor_t USB_Device_Qualifier_Descriptor USB_DESC_ATTR = {
      .bLength              = sizeof(USB_Device_Qualifier_Descriptor_t)
    , .bDescriptorType      = bDescriptorType_DEVICE_QUALIFIER
    , .bcdUSB               = bcdUSB_2
    , .bDeviceClass         = bClassCode_Composite
    , .bDeviceSubClass      = bSubClassCode_Composite
    , .bDeviceProtocol      = bProtocolCode_Composite
    , .bMaxPacketSize0      = 64
    , .bNumConfigurations   = 1
    , .bReserved            = 0
  };

  /* A structure template is required to determine */
  /* the total length of the General Configuration Set. */

  /* Configuration Set #0 */
  typedef struct {
    USB_Configuration_Descriptor_t                USB_Configuration_Descriptor_0;
    USB_Interface_Descriptor_t                    USB_Interface_Descriptor_0;
      USB_HID_Class_Descriptor_t                  USB_HID_Class_Descriptor_0;
        USB_HID_Item_Descriptors_List_t           USB_HID_Item_Descriptor_0[1];
      USB_Endpoint_Descriptor_t                   USB_Endpoint_Descriptor_81;
      USB_Endpoint_Descriptor_t                   USB_Endpoint_Descriptor_02;
  } USB_Genelal_Configuration_Descriptor_0_t;

  const USB_Genelal_Configuration_Descriptor_0_t USB_Genelal_Configuration_Descriptor_0 USB_DESC_ATTR = {
    .USB_Configuration_Descriptor_0 = {
        .bLength                = sizeof(USB_Configuration_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_CONFIGURATION
      , .wTotalLength           = sizeof(USB_Genelal_Configuration_Descriptor_0_t)
      , .bNumInterfaces         = 1     // Interface x 1
      , .bConfigurationValue    = 1     // Configuration#1
      , .iConfiguration         = 2     // iStrings
      , .bmAttributes           = 0
      , .bMaxPower              = 100 / 2
    },
    .USB_Interface_Descriptor_0 = {
        .bLength                = sizeof(USB_Interface_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_INTERFACE
      , .bInterfaceNumber       = 0     // Interface#0
      , .bAlternateSetting      = 0     // not used
      , .bNumEndpoints          = 2     // 0x81 + 0x02
      , .bInterfaceClass        = bClassCode_Human_Interface_Device
      , .bInterfaceSubClass     = bSubClassCode_None
      , .bInterfaceProtocol     = bProtocolCode_None
      , .iInterface             = 3     // iStrings
    },
    .USB_HID_Class_Descriptor_0 = {
        .bLength                = sizeof(USB_HID_Class_Descriptor_t) + sizeof(USB_HID_Item_Descriptors_List_t) * 1
      , .bDescriptorType        = bDescriptorType_HID_CLASS
      , .bcdHID                 = 0x111
      , .bCountryCode           = 0
      , .bNumDescriptors        = 1
    },
    .USB_HID_Item_Descriptor_0 = {
      { bDescriptorType_HID_REPORT, sizeof(HID_Report_Descriptor_0_temp) }
    },
    .USB_Endpoint_Descriptor_81 = {
        .bLength                = sizeof(USB_Endpoint_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_ENDPOINT
      , .bEndpointAddress       = 0x81
      , .bmAttributes           = bmEP_Type_Interrupt
      , .wMaxPacketSize         = 64
      , .bInterval              = 1
    },
    .USB_Endpoint_Descriptor_02 = {
        .bLength                = sizeof(USB_Endpoint_Descriptor_t)
      , .bDescriptorType        = bDescriptorType_ENDPOINT
      , .bEndpointAddress       = 0x02
      , .bmAttributes           = bmEP_Type_Interrupt
      , .wMaxPacketSize         = 64
      , .bInterval              = 1
    }
  };

  /**************************
   List-up Descriptor Section
  **************************/

  typedef struct {
    uint8_t head_0[3];    uint8_t body_0[sizeof(USB_DESC_STRS)];  // end null is head_1[1]
  /*uint8_t head_1[1];*/  USB_Device_Descriptor_t body_1;
    uint8_t head_2[1];    USB_Genelal_Configuration_Descriptor_0_t body_2;
    uint8_t head_3[3];    uint8_t body_3[sizeof(HID_Report_Descriptor_0_temp)];
    uint8_t head_4[1];    USB_Device_Qualifier_Descriptor_t body_4;
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
    , {0,  2 + sizeof(HID_Report_Descriptor_0_temp), bDescriptorType_HID_REPORT}
    ,      HID_Report_Descriptor_0
    , {0}, USB_Device_Qualifier_Descriptor
    /* Strings */
    , {1,  sizeof(USB_STR_1)-1, bDescriptorType_STRING, 3}
    , {2,                    9, bDescriptorType_STRING, 3 + 7}
    , {3,                    4, bDescriptorType_STRING, 3 + 17}
    /* Strings LANGID's : Table Terminator */
    , {0, 4, bDescriptorType_STRING}, {wLANG_EN_US}
  };

  // const uint8_t* USB_DescriptorSet = (uint8_t*)&USB_Descriptor + USB_DESC_ADDR;
  // const size_t   USB_DescriptorSetSize = sizeof(USB_Descriptor);
  // const uint8_t  USB_DescriptorSetSection = USB_DESC_ADDR;

  extern "C" {
    extern const uint8_t* USB_Descriptor_EDBG_Set;
    extern const size_t   USB_Descriptor_EDBG_SetSize;
    extern const uint8_t  USB_Descriptor_EDBG_SetSection;
  }
  const uint8_t* USB_Descriptor_EDBG_Set = (uint8_t*)&USB_Descriptor + USB_DESC_ADDR;
  const size_t   USB_Descriptor_EDBG_SetSize = sizeof(USB_Descriptor);
  const uint8_t  USB_Descriptor_EDBG_SetSection = USB_DESC_ADDR;

} // anonymous namespace

// end of code