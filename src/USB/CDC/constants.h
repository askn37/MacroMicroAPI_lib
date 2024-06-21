/**
 * @file USB/CDC/callback.cpp
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

#include "../CDC.h"
#include "define.h"

/* CDC-ACM general specs */
#define USB_ENDPOINTS_MAX     3     /* fixed value */
#define USB_BULK_INTERVAL     0     /* As large as possible */
#define USB_INTR_INTERVAL     5     /* As large as possible */

#define USB_SETUP_PK_SIZE     64    /* This length cannot be changed. */
#define USB_DATA_PK_SIZE      64    /* 64 is the maximum allowed by USB 2.0. */
#define USB_INTR_PK_SIZE      16    /* 16 is enough. */

#define USB_BULK_RECV_MAX     USB_DATA_PK_SIZE  /* Cannot be changed. */
#define USB_BULK_SEND_MAX     64                /* It can be expanded up to 1023. */

#define USB_DATA_BUFFER_SIZE  (USB_BULK_SEND_MAX + USB_BULK_RECV_MAX + USB_INTR_PK_SIZE - 8)

#define USB_EP_SIZE_gc(x)  ((x <= 8 ) ? USB_BUFSIZE_DEFAULT_BUF8_gc :\
                            (x <= 16) ? USB_BUFSIZE_DEFAULT_BUF16_gc:\
                            (x <= 32) ? USB_BUFSIZE_DEFAULT_BUF32_gc:\
                                        USB_BUFSIZE_DEFAULT_BUF64_gc)

/* EPFIFO number that identifies the EP */
#define USB_EP_SETUP_OUT  (0x00)
#define USB_EP_SETUP_IN   (0x08)
/* #define USB_EP_INTR_OUT   (0x10) */  /* notused */
#define USB_EP_INTR_IN    (0x18)
#define USB_EP_BULK_OUT   (0x20)
#define USB_EP_BULK_IN    (0x28)

/* Shortcut Macros */

#define USB_EP(EPFIFO) (*(USB_EP_t *)(((uint16_t)&get_endpointer_ptr()->EP) + (EPFIFO)))
#define USB_EP_STATUS_CLR(EPFIFO) _SFR_MEM8(&USB0_STATUS0_OUTCLR + ((EPFIFO) >> 2))
#define USB_EP_STATUS_SET(EPFIFO) _SFR_MEM8(&USB0_STATUS0_OUTSET + ((EPFIFO) >> 2))

#define USB_EP_REQ USB_EP(USB_EP_SETUP_OUT)
#define USB_EP_RES USB_EP(USB_EP_SETUP_IN)

#define USB_SETUP_DATA  (*(Setup_Packet*)&get_workmem_ptr()->setup)
#define USB_HEADER_DATA (*(Descriptor_Header*)&get_workmem_ptr()->header)
#define USB_DATA_BUFFER (*(uint8_t*)&get_workmem_ptr()->header)

#define USB_EP_INTR USB_EP(USB_EP_INTR_IN)
#define USB_EP_RECV USB_EP(USB_EP_BULK_OUT)
#define USB_EP_SEND USB_EP(USB_EP_BULK_IN)

#define USB_INTR_BUFFER (get_workmem_ptr()->control)
#define USB_RECV_BUFFER (get_workmem_ptr()->recv)
#define USB_SEND_BUFFER (get_workmem_ptr()->send)

/* Area for storing the status of the USB interface. */
#define USBSTATE (*(Interface_State*)get_state())  /* *(USB_Interface_State*) */

namespace USB_NAMESPACE {

  typedef struct {
    register8_t   FIFO[USB_ENDPOINTS_MAX * 2];  /* FIFO Index Table */
    USB_EP_PAIR_t EP[USB_ENDPOINTS_MAX];        /* USB Device Controller EP */
    _WORDREGISTER(FRAMENUM);                    /* FRAMENUM count */
  } PACKED USB_EP_TABLE_t;

  union USB_WM_TABLE_t {
    struct {
      Setup_Packet      setup;  /* 8 bytes */
      Descriptor_Header header; /* 2 bytes  */
      uint8_t data[USB_DATA_BUFFER_SIZE - 2];
    };
    struct {
      uint8_t control[USB_INTR_PK_SIZE];
      uint8_t recv[USB_BULK_RECV_MAX];
      uint8_t send[USB_BULK_SEND_MAX];
    };
  };

  /*** Callback functions for them. ***/

  USB_EP_TABLE_t* get_endpointer_ptr (void);  /* 64 bytes */
  USB_WM_TABLE_t* get_workmem_ptr (void);     /* 136 bytes */

  /* Static descriptor getter */
  size_t cb_get_descriptor_data (uint16_t _index, uint8_t* _buffer);

  /* Endpoint Setup IN/OUT */
  void ep_setup_clear (void);
  void ep_setup_in_listen (void);
  void ep_setup_out_listen (void);
  void ep_setup_in_pending (void);
  void ep_setup_out_pending (void);
  void ep_setup_stalled (void);

  /* custom callback */
  void cb_request_setup_stalled (USB_EP_t* EP_REQ, USB_EP_t* EP_RES);
  bool cb_request_standard_other (USB_EP_t* EP_REQ, USB_EP_t* EP_RES);
  bool cb_request_class (USB_EP_t* EP_REQ, USB_EP_t* EP_RES);
  bool cb_request_class_other (USB_EP_t* EP_REQ, USB_EP_t* EP_RES);
  bool cb_request_vender (USB_EP_t* EP_REQ, USB_EP_t* EP_RES);

  void cb_endpoint_complete (USB_EP_t* EP, uint8_t EP_ID);
  void cb_event_class_sof (void);
  uint8_t cb_get_configuration (void);
  bool cb_set_configuration (uint8_t _config);
  bool cb_clear_feature (void);
  bool cb_set_feature (void);
  void cb_clear_state (void);

  uint8_t cb_get_interface (uint8_t _interface);
  bool cb_set_interface (uint8_t _interface, uint8_t _altsetting);

} /* USB_CDC */

#endif
// end of header
