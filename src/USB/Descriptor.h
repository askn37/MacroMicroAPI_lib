/**
 * @file USB/descriptor.h
 * @author askn (K.Sato) multix.jp
 * @brief A collection of USB descriptor definitions.
 * @version 0.4.12
 * @date 2026-11-13
 * @copyright Copyright (c) 2026 askn37 at github.com
 * @link Product Potal : https://askn37.github.io/
 *         MIT License : https://askn37.github.io/LICENSE.html
 */

#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * USB Standard Descriptor field values
 */

enum USB_bDescriptorType_enum {
  /* Standard Request */
    bDescriptorType_NONCE                       = 0x00
  , bDescriptorType_DEVICE                      = 0x01
  , bDescriptorType_CONFIGURATION	              = 0x02
  , bDescriptorType_STRING                      = 0x03
  , bDescriptorType_INTERFACE                   = 0x04	
  , bDescriptorType_ENDPOINT                    = 0x05
  , bDescriptorType_DEVICE_QUALIFIER	          = 0x06
  , bDescriptorType_OTHER_SPEED_CONFIGURATION   = 0x07	
  , bDescriptorType_INTERFACE_POWER             = 0x08
  , bDescriptorType_OTG                         = 0x09
  , bDescriptorType_DEBUG                       = 0x0A
  , bDescriptorType_INTERFACE_ASSOCIATION       = 0x0B
  , bDescriptorType_BOS                         = 0x0D
  , bDescriptorType_DEVICE_CAPABILITY           = 0x10
  /* Class-Specific Request */
  , bDescriptorType_CS_BOS                      = 0x20
  , bDescriptorType_CS_DEVICE                   = 0x21
  , bDescriptorType_CS_CONFIGURATION            = 0x22
  , bDescriptorType_CS_STRING                   = 0x23
  , bDescriptorType_CS_INTERFACE                = 0x24
  , bDescriptorType_CS_ENDPOINT                 = 0x25
  /* Class-Specific Request : HID */
  , bDescriptorType_HID_CLASS                   = 0x21
  , bDescriptorType_HID_REPORT                  = 0x22
  , bDescriptorType_HID_PHYSICAL                = 0x23
};

enum USB_bClassCode_enum {
    bClassCode_Composite                  = 0x00  /* 00 00 00 */
  , bClassCode_Audio                      = 0x01
  , bClassCode_Communication_Device       = 0x02
  , bClassCode_Communication_Control      = 0x02
  , bClassCode_Human_Interface_Device     = 0x03
  , bClassCode_Physical                   = 0x05
  , bClassCode_Still_Imaging              = 0x06  /* 06 01 01 */
  , bClassCode_Image                      = 0x06  /* .. */
  , bClassCode_Printer                    = 0x07
  , bClassCode_Mass_Storage               = 0x08
  , bClassCode_HUB                        = 0x09  /* 09 00 [00-02] */
  , bClassCode_Communications_Data        = 0x0A
  , bClassCode_Smart_Card                 = 0x0B
  , bClassCode_Content_Security           = 0x0D  /* 0D 00 00 */
  , bClassCode_Video                      = 0x0E
  , bClassCode_Personal_Healthcare        = 0x0F
  , bClassCode_Audio_Video_Devices        = 0x10  /* 10 [01-03] 00 */
  , bClassCode_Billboard_Device           = 0x11  /* 11 00 00 */
  , bClassCode_USB_Type_C_Bridge          = 0x12  /* 12 00 00 */
  , bClassCode_Display_Protocol_Device    = 0x13  /* 13 00 00 */
  , bClassCode_MCTP_over_USB_Protocol     = 0x14  /* 14 [01,02] [01,02] */
  , bClassCode_I3C_Device                 = 0x3C  /* 3C 00 00 */
  , bClassCode_Diagnostic_Device          = 0xDC  /* DC [01-08] [00,01] */
  , bClassCode_Wireless_Controller        = 0xE0  /* E0 [01,02] [*] */
  , bClassCode_Miscellaneous              = 0xEF  /* EF [01-09] [*] */
                                                  /* ASD     : EF 01 01 */
                                                  /* Palm    : EF 01 02 */
                                                  /* IAD ECN : EF 02 01 */
                                                  /* RNDIS   : EF 04 [01-07] */
                                                  /* USB3    : EF 05 [00-02] */
  , bClassCode_Application_Specific       = 0xFE  /* FE [01-03] [*] */
                                                  /* DFU     : FE 01 01 */
                                                  /* IRDA    : FE 02 00 */
  , bClassCode_Vendor_Specific            = 0xFF
};

enum USB_bSubClassCode_enum {
    bSubClassCode_None                    = 0x00
  , bSubClassCode_Composite               = 0x00
  , bSubClassCode_Control                 = 0x01  /* bClassCode_Application_Specific */
  , bSubClassCode_Common                  = 0x02  /* bClassCode_Miscellaneous */
  , bSubClassCode_Abstract_Model          = 0x02  /* bClassCode_Communication_Device */
  , bSubClassCode_Streaming               = 0x02
  , bSubClassCode_SCSI                    = 0x06  /* bClassCode_Mass_Storage */
  , bSubClassCode_Vendor_Specific_253     = 0xFD
  , bSubClassCode_Vendor_Specific_254     = 0xFE
  , bSubClassCode_Vendor_Specific         = 0xFF  /* bClassCode_Vendor_Specific */
};

enum USB_bProtocolCode_enum {
    bProtocolCode_None                    = 0x00
  , bProtocolCode_Composite               = 0x00
  , bProtocolCode_Interface_Association   = 0x01  /* bSubClassCode_Common */
  , bProtocolCode_AT_Commands             = 0x01  /* bSubClassCode_Abstract_Model */
  , bProtocolCode_Device_Firmware_Upgrade = 0x01  /* bSubClassCode_Control */
  , bProtocolCode_Full_Low_Speed          = 0x00
  , bProtocolCode_High_Speed              = 0x02
  , bProtocolCode_Super_Speed             = 0x03
  , bProtocolCode_Bulk_Only               = 0x50
  , bProtocolCode_Vendor_Specific         = 0xFF  /* bSubClassCode_Vendor_Specific */
};

enum USB_bDescriptorSubtype_enum {
    bDescriptorSubtype_Header                         = 0x00
  , bDescriptorSubtype_Call_Management                = 0x01
  , bDescriptorSubtype_Abstract_Control_Management    = 0x02
  , bDescriptorSubtype_Direct_Line_Management         = 0x03
  , bDescriptorSubtype_Telephone_Ringer               = 0x04
  , bDescriptorSubtype_Reporting_Capabilities         = 0x05
  , bDescriptorSubtype_Union                          = 0x06
  , bDescriptorSubtype_Country_Selection              = 0x07
  , bDescriptorSubtype_Telephone_Operational_Modes    = 0x08
  , bDescriptorSubtype_USB_Terminal                   = 0x09
  , bDescriptorSubtype_Network_Channel_Terminal       = 0x0A
  , bDescriptorSubtype_Protocol_Unit                  = 0x0B
  , bDescriptorSubtype_Extension_Unit                 = 0x0C
  , bDescriptorSubtype_Multi_Channel_Management       = 0x0D
  , bDescriptorSubtype_CAPI_Control_Management        = 0x0E
  , bDescriptorSubtype_Ethernet_Networking            = 0x0F
  , bDescriptorSubtype_ATM_Networking                 = 0x10
  , bDescriptorSubtype_Wireless_Handset_Control_Model = 0x11
  , bDescriptorSubtype_Mobile_Direct_Line_Model       = 0x12
  , bDescriptorSubtype_MDLM_Detail                    = 0x13
  , bDescriptorSubtype_Device_Management_Model        = 0x14
  , bDescriptorSubtype_OBEX                           = 0x15
  , bDescriptorSubtype_Command_Set                    = 0x16
  , bDescriptorSubtype_Command_Set_Detail             = 0x17
  , bDescriptorSubtype_Telephone_Control_Model        = 0x18
  , bDescriptorSubtype_OBEX_Service_Identifier        = 0x19
  , bDescriptorSubtype_NCM                            = 0x1A
};

enum USB_bcdUSB_enum {
    bcdUSB_1    = 0x0100
   ,bcdUSB_1_1  = 0x0101
   ,bcdUSB_2    = 0x0200
   ,bcdUSB_2_1  = 0x0201  /* ECN */
   ,bcdUSB_2_10 = 0x0210  /* ECN */
   ,bcdUSB_3    = 0x0300
};

enum USB_bmAttributes_enum {
  /* Endpoint_Descriptor */
    bmEP_Type_Control     = (0 << 0)
  , bmEP_Type_Isochronous = (1 << 0)
  , bmEP_Type_Bulk        = (2 << 0)
  , bmEP_Type_Interrupt   = (3 << 0)
  , bmEP_Attr_Nosync      = (0 << 2)
  , bmEP_Attr_Async       = (1 << 2)
  , bmEP_Attr_Adaptive    = (2 << 2)
  , bmEP_Attr_Sync        = (3 << 2)
  , bmEP_Usage_Data       = (0 << 4)
  , bmEP_Usage_Feedback   = (1 << 4)
  , bmEP_Usage_Implicit   = (2 << 4)
  /* Configuration_Descriptor */
  , bmPW_RemoteWakeup     = (1 << 5)
  , bmPW_SelfPowered      = (1 << 6)
  , bmPW_BusPowered       = (1 << 7)
};

enum USB_wLANGID_enum {
    wLANG_EN_US = 0x0409
  , wLANG_JA_JP = 0x0411
};

/*
 * USB Standard Descriptors
 */

/* USB_Device_Descriptor (1:DD) : 18 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Device_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x01 bDescriptorType_DEVICE */
  uint16_t bcdUSB;              /* see USB_bcdUSB_enum */
  uint8_t  bDeviceClass;        /* see USB_bClassCode_enum */
  uint8_t  bDeviceSubClass;     /* see USB_bSubClassCode_enum */
  uint8_t  bDeviceProtocol;     /* see USB_bProtocolCode_enum */
  uint8_t  bMaxPacketSize0;     /* 8,16,32,64 */
  uint16_t idVendor;
  uint16_t idProduct;           
  uint16_t bcdDevice;
  uint8_t  iManufacturer;       /* iStrings */
  uint8_t  iProduct;            /* iStrings */
  uint8_t  iSerialNumber;       /* iStrings */
  uint8_t  bNumConfigurations;  /* not 0 */
} USB_Device_Descriptor_t;

/* USB_Configuration_Descriptor (2:CD) : 9 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Configuration_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x02 bDescriptorType_CONFIGURATION */
  uint16_t wTotalLength;
  uint8_t  bNumInterfaces;      /* 0... */
  uint8_t  bConfigurationValue; /* Configuration #ID */
  uint8_t  iConfiguration;      /* iStrings */
  uint8_t  bmAttributes;        /* see USB_bmAttributes_enum */
  uint8_t  bMaxPower;           /* x 2mA */
} USB_Configuration_Descriptor_t;

/* USB_String_Descriptor (3:SD) : 2+2N bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_String_Descriptor_t)+2N */
  uint8_t  bDescriptorType;     /* 0x03 bDescriptorType_STRING */
//uint16_t wString[N];
} USB_String_Descriptor_t;

/* USB_Interface_Descriptor (4:ID) : 9 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Interface_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x04 bDescriptorType_INTERFACE */
  uint8_t  bInterfaceNumber;    /* Interface #ID */
  uint8_t  bAlternateSetting;   /* 0 is not used or current interface */
  uint8_t  bNumEndpoints;       /* not 0 */
  uint8_t  bInterfaceClass;     /* see USB_bClassCode_enum */
  uint8_t  bInterfaceSubClass;  /* see USB_bSubClassCode_enum */
  uint8_t  bInterfaceProtocol;  /* see USB_bProtocolCode_enum */
  uint8_t  iInterface;          /* iStrings */
} USB_Interface_Descriptor_t;

/* USB_Endpoint_Descriptor (5:ED) : 7 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Endpoint_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x05 bDescriptorType_ENDPOINT */
  uint8_t  bEndpointAddress;    /* Endpoint #ID */
  uint8_t  bmAttributes;        /* see USB_bmAttributes_enum */
  uint16_t wMaxPacketSize;      /* 8,16,32,64,128,256,512,1023(1024) */
  uint8_t  bInterval;           /* 1,2,4...128 */
} USB_Endpoint_Descriptor_t;

/* USB_Device_Qualifier_Descriptor (6:DQD) : 10 bytes */
/*
 * NOTE: For this descriptor, USB 2.0 Full-Speed-only devices must always
 * respond with a STALL (rejection). Failure to do so will cause an error
 * in Linux. On other operating systems, a 0x07:QSCD request will not be
 * issued, provided the configuration is the same as that for Full-Speed.
 */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Device_Qualifier_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x06 bDescriptorType_DEVICE_QUALIFIER */
  uint16_t bcdUSB;              /* see USB_bcdUSB_enum */
  uint8_t  bDeviceClass;        /* see USB_bClassCode_enum */
  uint8_t  bDeviceSubClass;     /* see USB_bSubClassCode_enum */
  uint8_t  bDeviceProtocol;     /* see USB_bProtocolCode_enum */
  uint8_t  bMaxPacketSize0;     /* 8,16,32,64 */
  uint8_t  bNumConfigurations;  /* not 0 */
  uint8_t  bReserved;           /* 0 */
} USB_Device_Qualifier_Descriptor_t;

/* USB_Other_Speed_Configuration_Descriptor (7:QSCD) : 9 bytes */
/*
 * NOTE: When responding to a 0x06:DQD on Linux, a 0x07:QSCD is required
 * even for non-High-Speed ​​devices. Furthermore,
 * if any issues arise, a 0x0A:DEBUG will be requested.
 */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Other_Speed_Configuration_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x07 bDescriptorType_SPEED_CONFIGURATION */
  uint16_t wTotalLength;
  uint8_t  bNumInterfaces;      /* not 0 */
  uint8_t  bConfigurationValue; /* Configuration #ID */
  uint8_t  iConfiguration;      /* iStrings */
  uint8_t  bmAttributes;        /* see USB_bmAttributes_enum */
  uint8_t  bMaxPower;           /* x 2mA */
} USB_Other_Speed_Configuration_Descriptor_t;

/* USB_Debug_Descriptor (0x0A:DEBUG) : 4 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Debug_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x0A bDescriptorType_DEBUG */
  uint8_t  bDebugInEndpoint;
  uint8_t  bDebugOutEndpoint;
} USB_Debug_Descriptor_t;

/* USB_Interface_Association_Descriptor (0x0B:IAD) : 8 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Interface_Association_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x0B bDescriptorType_INTERFACE_ASSOCIATION */
  uint8_t  bFirstInterface;     /* Interface #ID */
  uint8_t  bInterfaceCount;     /* not 0 */
  uint8_t  bFunctionClass;      /* see USB_bClassCode_enum */
  uint8_t  bFunctionSubClass;   /* see USB_bSubClassCode_enum */
  uint8_t  bFunctionProtocol;   /* see USB_bProtocolCode_enum */
  uint8_t  iFunction;           /* iStrings */
} USB_Interface_Association_Descriptor_t;

/*
 * USB Class-Specific Descriptors
 */

/* USB_HID_Class_Descriptor (0x21:HID) : 6+3N bytes */
typedef struct {
  uint8_t  bLength;           /* sizeof(USB_HID_Class_Descriptor_t)+3N */
  uint8_t  bDescriptorType;   /* 0x21 bDescriptorType_HID_CLASS */
  uint16_t bcdHID;            /* 0x0111 */
  uint8_t  bCountryCode;      /* 0 is not used */
  uint8_t  bNumDescriptors;   /* N */
/*USB_HID_Item_Descriptors_List_t USB_HID_Item_Descriptors[N] */
} USB_HID_Class_Descriptor_t;

typedef struct {
  uint8_t  bDescriptorType;   /* bDescriptorType_HID_REPORT or bDescriptorType_HID_PHYSICAL */
  uint16_t wDescriptorLength;
} USB_HID_Item_Descriptors_List_t;

/*
 * USB HID Report Macro
 */

/* Only the key report items are defined here.*/
#define   HR_bUsage_Page(N)       0x05,N&0xFF
#define   HR_wUsage_Page(N)       0x06,N&0xFF,N>>8
#define   HR_bUsage(N)            0x09,N&0xFF
#define   HR_wUsage(N)            0x0A,N&0xFF,N>>8
#define   HR_bLogical_Minimum_0   0x14
#define   HR_bLogical_Minimum(N)  0x15,N&0xFF
#define   HR_wLogical_Minimum(N)  0x16,N&0xFF,N>>8
#define   HR_bUsage_Minimum_0     0x18
#define   HR_bUsage_Minimum(N)    0x19,N&0xFF
#define   HR_wUsage_Minimum(N)    0x1A,N&0xFF,N>>8
#define   HR_bLogical_Maximum(N)  0x25,N&0xFF
#define   HR_wLogical_Maximum(N)  0x26,N&0xFF,N>>8
#define   HR_bUsage_Maximum(N)    0x29,N&0xFF
#define   HR_wUsage_Maximum(N)    0x2A,N&0xFF,N>>8
#define   HR_bReport_Size(N)      0x75,N&0xFF
#define   HR_wReport_Size(N)      0x76,N&0xFF,N>>8
#define   HR_bReport_ID(N)        0x85,N&0xFF
#define   HR_wReport_ID(N)        0x86,N&0xFF,N>>8
#define   HR_bInput(N)            0x81,N&0xFF
#define   HR_wInput(N)            0x82,N&0xFF,N>>8
#define   HR_bOutput(N)           0x91,N&0xFF
#define   HR_wOutput(N)           0x92,N&0xFF,N>>8
#define   HR_bReport_Count(N)     0x95,N&0xFF
#define   HR_wReport_Count(N)     0x96,N&0xFF,N>>8
#define   HR_bCollection(N)       0xA1,N&0xFF
#define   HR_bFeature(N)          0xB1,N&0xFF
#define   HR_wFeature(N)          0xB2,N&0xFF,N>>8
#define   HR_EndCollection        0xC0

/*
 * USB CDC Functional Descriptor (bDescriptorType_CS_INTERFACE)
 */

/* USB_Header_Functional_Descriptor (0x24,0x00) : 5 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Header_Functional_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x24 bDescriptorType_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;  /* 0x00 bDescriptorSubtype_Header */
  uint16_t bcdCDC;
} USB_Header_Functional_Descriptor_t;

/* USB_Abstract_Control_Management_Functional_Descriptor (0x24,0x02) : 4 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_ACM_Functional_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x24 bDescriptorType_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;  /* 0x02 bDescriptorSubtype_Abstract_Control_Management */
  uint8_t  bmCapabilities;      /* see USB_bmCapabilities_enum */
} USB_ACM_Functional_Descriptor_t;

/* USB_Union_Functional_Descriptor (0x24,0x06) : 4+N bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Union_Functional_Descriptor_t)+N */
  uint8_t  bDescriptorType;     /* 0x24 bDescriptorType_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;  /* 0x06 bDescriptorSubtype_Union */
  uint8_t  bControlInterface;   /* N */
/*uint8_t  bSubordinateInterfaceN; */
} USB_Union_Functional_Descriptor_t;

/* USB_Call_Management_Functional_Descriptor (0x24,0x01) : 5 bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Call_Management_Functional_Descriptor_t) */
  uint8_t  bDescriptorType;     /* 0x24 bDescriptorType_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;  /* 0x01 bDescriptorSubtype_Call_Management */
  uint8_t  bmCapabilities;      /* see USB_ACM_Functional_Descriptor_t */
  uint8_t  bDataInterface;      /* CDC-DATA Interface Number */
} USB_Call_Management_Functional_Descriptor_t;

/* USB_Country_Selection_Functional_Descriptor (0x24,0x07) : 4+2N bytes */
typedef struct {
  uint8_t  bLength;             /* sizeof(USB_Country_Selection_Functional_Descriptor_t)+2N */
  uint8_t  bDescriptorType;     /* 0x24 bDescriptorType_CS_INTERFACE */
  uint8_t  bDescriptorSubtype;  /* 0x07 bDescriptorSubtype_Country_Selection */
  uint8_t  iCountryCodeRelDate; /* iStrings */
/*uint16_t wCountryCode[N]; */
} USB_Country_Selection_Functional_Descriptor_t;

enum USB_bmCapabilities_enum {
  /* USB_ACM_Functional_Descriptor_t */
    bmACM_NETWORK_CONNECTION  = 0x08
  , bmACM_SEND_BREAK          = 0x04
  , bmACM_SERIAL_STATE        = 0x02
  , bmACM_COMM_FEATURE        = 0x01
};

/*
 * List-up Descriptor workspace
 */

extern const uint8_t* USB_DescriptorSet;
extern const size_t   USB_DescriptorSetSize;
extern const uint8_t  USB_DescriptorSetSection;

#ifdef __cplusplus
}
#endif

// end of header
