/*!
    \file  cdc_acm_core.h
    \brief the header file of IAP driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef CDC_ACM_CORE_H
#define CDC_ACM_CORE_H

#include "usbd_std.h"

#define TPDU_EXCHANGE                  0x01
#define SHORT_APDU_EXCHANGE            0x02
#define EXTENDED_APDU_EXCHANGE         0x04
#define CHARACTER_EXCHANGE             0x00

#define EXCHANGE_LEVEL_FEATURE         TPDU_EXCHANGE

#define REQUEST_ABORT                  0x01
#define REQUEST_GET_CLOCK_FREQUENCIES  0x02
#define REQUEST_GET_DATA_RATES         0x03

#define SMARTCARD_SIZ_CONFIG_DESC      93

#define CCID_BULK_EPIN_SIZE            64 
#define CCID_BULK_EPOUT_SIZE           64  
#define CCID_INTR_EPIN_SIZE            8
#define CCID_EP0_BUFF_SIZ              64

#define USB_DESCTYPE_CS_INTERFACE               0x24
#define USB_CCID_CONFIG_DESC_SIZE               93
#define CCID_DESC_SIZE                         0x8A

#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define NO_CMD                                  0xFF

#pragma pack(1)

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: header function descriptor */
    uint16_t  bcdCDC;                     /*!< bcdCDC: low byte of spec release number (CDC1.10) */
} usb_descriptor_header_function_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype:  call management function descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D0 is reset, D1 is ignored */
    uint8_t  bDataInterface;              /*!< bDataInterface: 1 interface used for call management */
} usb_descriptor_call_managment_function_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: abstract control management desc */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D1 */
} usb_descriptor_acm_function_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: union func desc */
    uint8_t  bMasterInterface;            /*!< bMasterInterface: communication class interface */
    uint8_t  bSlaveInterface0;            /*!< bSlaveInterface0: data class interface */
} usb_descriptor_union_function_struct;

#pragma pack() 

typedef struct
{
    usb_descriptor_configuration_struct               config;
    usb_descriptor_interface_struct                   ccid_interface;
    uint8_t ccid_class[54];                  
    usb_descriptor_endpoint_struct                    ccid_bulk_in_endpoint;
	  usb_descriptor_endpoint_struct                    ccid_bulk_out_endpoint;
   	usb_descriptor_endpoint_struct                    ccid_intr_endpoint;
} usb_descriptor_configuration_set_struct;

extern void* const usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descriptor;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the CCID T0 device */
usbd_status_enum ccid_init(void *pudev, uint8_t config_index);
/* de-initialize the CCID T0 device */
usbd_status_enum ccid_deinit(void *pudev, uint8_t config_index);
/* handle the CCID T0 class-specific requests */
usbd_status_enum ccid_req_handler(void *pudev, usb_device_req_struct *req);
/* handle CCID T0 data */
usbd_status_enum ccid_data_handler(void *pudev, usbd_dir_enum rx_tx, uint8_t ep_id);

#endif  /* CCID_CORE_H */
