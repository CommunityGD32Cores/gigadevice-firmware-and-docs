/*!
    \file  cdc_acm_core.h
    \brief the header file of CDC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef CDC_ACM_CORE_H
#define CDC_ACM_CORE_H

#include "usbd_std.h"

#define USB_DESCTYPE_CS_INTERFACE               0x24
#define USB_CDC_ACM_CONFIG_DESC_SIZE            0x43

#define CDC_ACM_DESC_SIZE                       0x3A

#define CDC_ACM_DESC_TYPE                       0x21

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
    uint32_t dwDTERate;                   /*!< data terminal rate */
    uint8_t  bCharFormat;                 /*!< stop bits */
    uint8_t  bParityType;                 /*!< parity */
    uint8_t  bDataBits;                   /*!< data bits */
}line_coding_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t   bDescriptorSubtype;         /*!< bDescriptorSubtype: header function descriptor */
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

typedef struct
{
    usb_descriptor_configuration_struct               config;
    usb_descriptor_interface_struct                   cdc_loopback_interface;
    usb_descriptor_header_function_struct             cdc_loopback_header;
    usb_descriptor_call_managment_function_struct     cdc_loopback_call_managment;
    usb_descriptor_acm_function_struct                cdc_loopback_acm;
    usb_descriptor_union_function_struct              cdc_loopback_union;
    usb_descriptor_endpoint_struct                    cdc_loopback_cmd_endpoint;
    usb_descriptor_interface_struct                   cdc_loopback_data_interface;
    usb_descriptor_endpoint_struct                    cdc_loopback_out_endpoint;
    usb_descriptor_endpoint_struct                    cdc_loopback_in_endpoint;
} usb_descriptor_configuration_set_struct;

#pragma pack()

extern uint8_t* usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descriptor;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the CDC ACM device */
uint8_t cdc_acm_init(void *pudev, uint8_t config_index);
/* de-initialize the CDC ACM device */
uint8_t cdc_acm_deinit(void *pudev, uint8_t config_index);
/* handle the CDC ACM class-specific requests */
uint8_t cdc_acm_req_handler(void *pudev, usb_device_req_struct *req);
/* handle CDC ACM data */
uint8_t cdc_acm_data_handler(void *pudev, usb_dir_enum rx_tx, uint8_t ep_num);

/* receive CDC ACM data */
void cdc_acm_data_receive(void *pudev);
/* send CDC ACM data */
void cdc_acm_data_send(void *pudev, uint32_t data_len);
/* command data received on control endpoint */
usbd_status_enum cdc_acm_EP0_RxReady(void  *pudev);

#endif  /* CDC_ACM_CORE_H */
