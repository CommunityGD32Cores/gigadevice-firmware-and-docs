/*!
    \file  hid_core.h
    \brief header file for the hid_core.c file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef HID_CORE_H
#define HID_CORE_H

#include  "usbd_std.h"

#define USB_HID_CONFIG_DESC_SIZE        0x22
#define USB_HID_DESC_SIZE               0x09
#define USB_HID_REPORT_DESC_SIZE        0x29

#define HID_DESC_TYPE                   0x21
#define HID_REPORT_DESCTYPE             0x22

#define GET_REPORT                      0x01
#define GET_IDLE                        0x02
#define GET_PROTOCOL                    0x03
#define SET_REPORT                      0x09
#define SET_IDLE                        0x0A
#define SET_PROTOCOL                    0x0B

#pragma pack(1)

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< regular descriptor header containing the descriptor's type and length */

    uint16_t bcdHID;                      /*!< BCD encoded version that the HID descriptor and device complies to */
    uint8_t  bCountryCode;                /*!< country code of the localized device, or zero if universal */
    uint8_t  bNumDescriptors;             /*!< total number of HID report descriptors for the interface */
    uint8_t  bDescriptorType;             /*!< type of HID report */
    uint16_t wDescriptorLength;           /*!< length of the associated HID report descriptor, in bytes */
} usb_hid_descriptor_hid_struct;

#pragma pack()

typedef struct
{
    usb_descriptor_configuration_struct        config;

    usb_descriptor_interface_struct            hid_interface;
    usb_hid_descriptor_hid_struct              hid_vendor;
    usb_descriptor_endpoint_struct             hid_in_endpoint;
} usb_descriptor_configuration_set_struct;

extern uint8_t* usbd_strings[USB_STRING_COUNT];
extern usb_descriptor_device_struct device_descripter;
extern usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the HID device */
uint8_t usbd_hid_init (void *pudev, uint8_t config_index);
/* de-initialize the HID device */
uint8_t usbd_hid_deinit (void *pudev, uint8_t config_index);
/* handle the HID class-specific requests */
uint8_t usbd_hid_classreq_handle (void *pudev, usb_device_req_struct *req);
/* handle data stage */
uint8_t usbd_hid_data_handler (void *pudev, usb_dir_enum rx_tx, uint8_t ep_id);

/* send keyboard report */
uint8_t usbd_hid_report_send (usb_core_handle_struct *pudev, uint8_t *report, uint16_t Len);

#endif  /* HID_CORE_H */
