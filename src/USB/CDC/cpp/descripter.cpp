/**
 * @file USB/CDC/cpp/descripter.cpp
 * @author askn (K.Sato) multix.jp
 * @brief Example of using SerialUSB class
 * @version 0.1
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <avr/io.h>
#if defined(__cplusplus) && defined(USB0)
#include <avr/pgmspace.h>
// #include "api/macro_api.h"

#include "../../CDC.h"
#include "../constants.h"

#include "api/btools.h" /* from Micro_API */

/*** WARNING ***
 * This USB device information applies only when using Microchip Technology Inc. products.
 * It must be modified when porting to other companies' products.
 */
#define USB_VENDOR_ID      0x04D8   /* 0x04D8=MCHP VID. */
#define USB_PRODUCT_ID     0x0B15   /* PID 0x0010-0x002F reserved for testing/non-public demos. */

#define USB_MANUFACTURER   L"Microchip Technology Inc."
#define USB_PRODUCT        L"USB Serial for AVR-DU series"
// #define USB_SERIALNUMBER   L"000000"
#define USB_SERIALNUMBER_CHIP       /* Uses the serial number embedded in the chip */
// #define USB_DEVICENAME     L"VCOM" /* for IAD String */

using namespace USB_NAMESPACE;

#ifdef USB_MANUFACTURER
alignas(2) const USB_String_Descriptor PROGMEM manufacturer_string = {
  .bLength              = sizeof(USB_MANUFACTURER)
, .bDescriptorType      = USB_DTYPE_String
, USB_MANUFACTURER
};
#endif

#ifdef USB_PRODUCT
alignas(2) const USB_String_Descriptor PROGMEM product_string = {
  .bLength              = sizeof(USB_PRODUCT)
, .bDescriptorType      = USB_DTYPE_String
, USB_PRODUCT
};
#endif

#ifdef USB_SERIALNUMBER
/* If this is enabled, it returns the USB_SERIAL_NUMBER defined value. */
alignas(2) const USB_String_Descriptor PROGMEM serialnumber_string = {
  .bLength              = sizeof(USB_SERIALNUMBER)
, .bDescriptorType      = USB_DTYPE_String
, USB_SERIALNUMBER
};
#endif

#ifdef USB_DEVICENAME
alignas(2) const USB_String_Descriptor PROGMEM interface_string = {
  .bLength              = sizeof(USB_DEVICENAME)
, .bDescriptorType      = USB_DTYPE_String
, USB_DEVICENAME
};
#define USB_DEVICENAME_INDEX 4
#else
#define USB_DEVICENAME_INDEX 0
#endif

#ifdef USB_MANUFACTURER
#define USB_MANUFACTURER_INDEX 1
#else
#define USB_MANUFACTURER_INDEX 0
#endif

#ifdef USB_PRODUCT
#define USB_PRODUCT_INDEX 2
#else
#define USB_PRODUCT_INDEX 0
#endif

#if defined(USB_SERIALNUMBER) || defined(USB_SERIALNUMBER_CHIP)
#define USB_SERIALNUMBER_INDEX 3
#else
#define USB_SERIALNUMBER_INDEX 0
#endif

alignas(2) const USB_String_Descriptor PROGMEM language_string = {
  .bLength              = 2
, .bDescriptorType      = USB_DTYPE_String
, .wString              = {USB_LANGUAGE_EN_US}
};

alignas(2) const USB_Device_Descriptor PROGMEM device_descriptor = {
  .bLength              = sizeof(USB_Device_Descriptor)
, .bDescriptorType      = USB_DTYPE_Device
, .bcdUSB               = 0x0200    // valid 0x0200 (2.0), 0x0100 (1.0) and 0x0110 (1.1)
, .bDeviceClass         = 0x02      // CSCP_IADDeviceClass; was 2
, .bDeviceSubClass      = 0x00      // CSCP_IADDeviceSubclass; was 0
, .bDeviceProtocol      = 0x00      // CSCP_IADDeviceProtocol; was 0
, .bMaxPacketSize0      = USB_SETUP_PK_SIZE   // EndPoint-0 Max packet size
, .idVendor             = USB_VENDOR_ID
, .idProduct            = USB_PRODUCT_ID
, .bcdDevice            = 0x0010    // user device version 0.1
, .iManufacturer        = USB_MANUFACTURER_INDEX
, .iProduct             = USB_PRODUCT_INDEX
, .iSerialNumber        = USB_SERIALNUMBER_INDEX
, .bNumConfigurations   = 1
};

typedef struct USB_General_Configuration_Descriptor {
  USB_Configuration_Descriptor              Config;
  USB_InterfaceAssociation_Descriptor       CDC_IAD;
  USB_Interface_Descriptor                  CDC_CCI_Interface;
  USB_CDC_Header_Functional_Descriptor      CDC_Header_Functional_Descriptor;
  /*
  CDC_CallManagement_Functional_Descriptor  CallManagement_Functional_Descriptor;
  */
  CDC_ACM_Functional_Descriptor             ACM_Functional_Descriptor;
  CDC_Union_Functional_Descriptor           Union_Functional_Descriptor;
  USB_Endpoint_Descriptor                   CDC_Notification_Endpoint;
  USB_Interface_Descriptor                  CDC_DCI_Interface;
  USB_Endpoint_Descriptor                   CDC_DataOut_Endpoint;
  USB_Endpoint_Descriptor                   CDC_DataIn_Endpoint;
} USB_General_Configuration_Descriptor;

alignas(2) const USB_General_Configuration_Descriptor PROGMEM general_configuration_descriptor = {
  .Config = {
    .bLength              = sizeof(USB_Configuration_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Configuration
  , .wTotalLength         = sizeof(USB_General_Configuration_Descriptor)
  , .bNumInterfaces       = 2
  , .bConfigurationValue  = 1       // Request SET_CONFIGURATION Number
  , .iConfiguration       = 0
  , .bmAttributes         = USB_CONFIG_ATTR_BUSPOWERED_bm
  , .bMaxPower            = 100 / 2
  },
  .CDC_IAD = {
    .bLength              = sizeof(USB_InterfaceAssociation_Descriptor)
  , .bDescriptorType      = USB_DTYPE_InterfaceAssociation
  , .bFirstInterface      = USB_INTERFACE_ID_CDC_CCI
  , .bInterfaceCount      = 2
  , .bFunctionClass       = 0x02    // CDC_CSCP_CDCClass
  , .bFunctionSubClass    = 0x02    // CDC_CSCP_ACMSubclass
  , .bFunctionProtocol    = 0x01    // CSC_CSCP_ATCommandProtocol
  , .iFunction            = USB_DEVICENAME_INDEX
  },
  .CDC_CCI_Interface = {
    .bLength              = sizeof(USB_Interface_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Interface
  , .bInterfaceNumber     = USB_INTERFACE_ID_CDC_CCI
  , .bAlternateSetting    = 0
  , .bNumEndpoints        = 1       // 1:EP_IN
  , .bInterfaceClass      = 0x02    // CDC_CSCP_CDCClass
  , .bInterfaceSubClass   = 0x02    // CDC_CSCP_ACMSubclass
  , .bInterfaceProtocol   = 0x01    // CDC_CSCP_ATCommandProtocol
  , .iInterface           = 0
  },
  .CDC_Header_Functional_Descriptor = {
    .bLength              = sizeof(USB_CDC_Header_Functional_Descriptor)
  , .bDescriptorType      = USB_DTYPE_CSInterface
  , .bDescriptorSubtype   = 0x00    // CDC_DSUBTYPE_CSInterface_Header
  , .CDCSpecification     = 0x0010  // bcdCDC
  },
  /*
  .CallManagement_FunctionalDescriptor = {
    .bLength              = sizeof(CDC_CallManagement_Functional_Descriptor)
  , .bDescriptorType      = USB_DTYPE_CSInterface
  , .bDescriptorSubtype   = 0x01
  , .bmCapabilities       = 0x00    // 1:Device handles call management itself.
  , .bDataInterface       = 1
  },
  */
  .ACM_Functional_Descriptor = {
    .bLength              = sizeof(CDC_ACM_Functional_Descriptor)
  , .bDescriptorType      = USB_DTYPE_CSInterface
  , .bDescriptorSubtype   = 0x02    // CDC_DSUBTYPE_CSInterface_ACM
  , .bmCapabilities       = 0x06    // 4:Send_Break 2:Serial_State
  },
  .Union_Functional_Descriptor = {
    .bLength              = sizeof(CDC_Union_Functional_Descriptor)
  , .bDescriptorType      = USB_DTYPE_CSInterface
  , .bDescriptorSubtype   = 0x06    // CDC_DSUBTYPE_CSInterface_Union
  , .bMasterInterface     = USB_INTERFACE_ID_CDC_CCI
  , .bSlaveInterface0     = USB_INTERFACE_ID_CDC_DCI
  },
  .CDC_Notification_Endpoint = {
    .bLength              = sizeof(USB_Endpoint_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Endpoint
  , .bEndpointAddress     = 1 | USB_EP_DESCRIPTOR_DIR_IN_bm
  , .bmAttributes         = USB_EP_TYPE_INTERRUPT_gc
  , .wMaxPacketSize       = USB_INTR_PK_SIZE
  , .bInterval            = USB_INTR_INTERVAL
  },
  .CDC_DCI_Interface = {
    .bLength              = sizeof(USB_Interface_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Interface
  , .bInterfaceNumber     = USB_INTERFACE_ID_CDC_DCI
  , .bAlternateSetting    = 0
  , .bNumEndpoints        = 2       // 1:EP_OUT + 2:EP_IN
  , .bInterfaceClass      = 0x0A    // CSC_CSCP_CDCDataClass
  , .bInterfaceSubClass   = 0x00    // CDC_CSCP_NoDataSubclass
  , .bInterfaceProtocol   = 0x00    // CDC_CSCP_NoDataProtocol
  , .iInterface           = 0
  },
  .CDC_DataOut_Endpoint = {
    .bLength              = sizeof(USB_Endpoint_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Endpoint
  , .bEndpointAddress     = 2 | USB_EP_DESCRIPTOR_DIR_OUT_bm
  , .bmAttributes         = USB_EP_TYPE_BULK_gc
  , .wMaxPacketSize       = USB_DATA_PK_SIZE
  , .bInterval            = USB_BULK_INTERVAL
  },
  .CDC_DataIn_Endpoint = {
    .bLength              = sizeof(USB_Endpoint_Descriptor)
  , .bDescriptorType      = USB_DTYPE_Endpoint
  , .bEndpointAddress     = 2 | USB_EP_DESCRIPTOR_DIR_IN_bm
  , .bmAttributes         = USB_EP_TYPE_BULK_gc
  , .wMaxPacketSize       = USB_DATA_PK_SIZE
  , .bInterval            = USB_BULK_INTERVAL
  },
};

alignas(2) const USB_DeviceQualifier_Descriptor PROGMEM devicequalifier_descriptor = {
  /* This descriptor is used when inserted into a USB 3.x port. */
  .bLength                = sizeof(USB_DeviceQualifier_Descriptor)
, .bDescriptorType        = USB_DTYPE_DeviceQualifier
, .bcdUSB                 = 0x200
, .bDeviceClass           = 0x02
, .bDeviceSubClass        = 0x00
, .bDeviceProtocol        = 0x00
, .bMaxPacketSize0        = USB_SETUP_PK_SIZE
, .bNumConfigurations     = 0
, .bReserved              = 0
};

namespace USB_NAMESPACE {

  /*
   * Configuration callback Methods
   */

  /* Sets the specified device/configuration descriptor for the endpoint. */
  /* It then returns the payload size. */
  WEAK size_t cb_get_descriptor_data (uint16_t _index, uint8_t* _buffer) {
    uint8_t* _pgmem = 0;
    size_t   _size = 0;
    if (_index == USB_DATA_GetDescripter_Device) {
      _pgmem = (uint8_t*)&device_descriptor;
      _size = sizeof(device_descriptor);
    }
    else if (_index == USB_DATA_GetDescripter_Config) {
      _pgmem = (uint8_t*)&general_configuration_descriptor;
      _size = sizeof(general_configuration_descriptor);
    }
    else if (_index == USB_DATA_GetDescripter_Qualifier) {
      _pgmem = (uint8_t*)&devicequalifier_descriptor;
      _size = sizeof(devicequalifier_descriptor);
    }
    else {
      if (_index == USB_DATA_GetString_Index_0) {
        _pgmem = (uint8_t*)&language_string;
      }
      #if USB_MANUFACTURER_INDEX != 0
      else if (_index == USB_DATA_GetString_Index_1) {
        _pgmem = (uint8_t*)&manufacturer_string;
      }
      #endif
      #if USB_PRODUCT_INDEX != 0
      else if (_index == USB_DATA_GetString_Index_2) {
        _pgmem = (uint8_t*)&product_string;
      }
      #endif
      #if USB_SERIALNUMBER_INDEX != 0
      else if (_index == USB_DATA_GetString_Index_3) {
        #ifdef USB_SERIALNUMBER_CHIP
        /*** Otherwise, get the chip's unique manufacturer number. ***/
        /*** This function requires a 66-byte buffer. ***/
        uint8_t *p = (uint8_t*)&SIGROW_SERNUM0;
        uint8_t *q = _buffer;
        *q++ = 64;    /* .bLength */
        *q++ = USB_DTYPE_String;
        for (uint8_t i = 0; i < 16; i++) {
          uint8_t c = *p++;
          *q++ = btoh(c >> 4);  /* High nibble hex */
          *q++ = 0;             /* Zero padding */
          *q++ = btoh(c);       /* Low nibble hex */
          *q++ = 0;             /* Zero padding */
        }
        return 66;
        #else
        _pgmem = (uint8_t*)&serialnumber_string;
        #endif
      }
      #endif
      #if USB_DEVICENAME_INDEX != 0
      else if (_index == USB_DATA_GetString_Index_4) {
        _pgmem = (uint8_t*)&interface_string;
      }
      #endif
      if (_pgmem) _size = 2 + pgm_read_byte_near(_pgmem);
    }
    if (_size) memcpy_P(_buffer, _pgmem, _size);
    return _size;
  }

} /* namespace USB_CDC */

#endif
// end of code
