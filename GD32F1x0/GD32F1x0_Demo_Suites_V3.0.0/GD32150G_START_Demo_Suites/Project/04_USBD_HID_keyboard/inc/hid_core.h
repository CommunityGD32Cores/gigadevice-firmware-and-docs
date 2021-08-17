/*!
    \file  hid_core.h
    \brief definitions for HID core

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef HID_CORE_H
#define HID_CORE_H

#include "usbd_std.h"

#define USB_HID_CONFIG_DESC_SIZE         0x29

#define HID_DESC_TYPE                    0x21
#define USB_HID_DESC_SIZE                0x09
#define USB_HID_REPORT_DESC_SIZE         0x3D
#define HID_REPORT_DESCTYPE              0x22

#define GET_REPORT                       0x01
#define GET_IDLE                         0x02
#define GET_PROTOCOL                     0x03
#define SET_REPORT                       0x09
#define SET_IDLE                         0x0A
#define SET_PROTOCOL                     0x0B

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
    usb_descriptor_configuration_struct Config;

    usb_descriptor_interface_struct            HID_Interface;
    usb_hid_descriptor_hid_struct              HID_VendorHID;
    usb_descriptor_endpoint_struct             HID_ReportINEndpoint;
    usb_descriptor_endpoint_struct             HID_ReportOUTEndpoint;
} usb_descriptor_configuration_set_struct;

extern void* const usbd_strings[USB_STRING_COUNT];
extern const usb_descriptor_device_struct device_descripter;
extern const usb_descriptor_configuration_set_struct configuration_descriptor;

/* function declarations */
/* initialize the HID device */
usbd_status_enum hid_init (void *pudev, uint8_t config_index);
/* de-initialize the HID device */
usbd_status_enum hid_deinit (void *pudev, uint8_t config_index);
/* handle the HID class-specific requests */
usbd_status_enum hid_req_handler (void *pudev, usb_device_req_struct *req);
/* handle data stage */
usbd_status_enum hid_data_handler (void *pudev, usbd_dir_enum rx_tx, uint8_t ep_id);

/* send keyboard report */
uint8_t hid_report_send (usbd_core_handle_struct *pudev, uint8_t *report, uint16_t len);

#endif  /* HID_CORE_H */
