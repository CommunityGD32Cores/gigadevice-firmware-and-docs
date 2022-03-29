/*!
    \file    hid_printer_wrapper.c
    \brief   this file calls to the separate HID and printer class layer handlers

    \version 2020-07-17, V3.0.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "hid_printer_wrapper.h"
#include "usbd_lld_int.h"

#define USBD_VID                          0x28E9
#define USBD_PID                          0x325A

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_desc_dev hid_printer_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV
     },
    .bcdUSB                = 0x0200,
    .bDeviceClass          = 0x00,
    .bDeviceSubClass       = 0x00,
    .bDeviceProtocol       = 0x00,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

/* USB device configuration descriptor */
usb_hid_printer_desc_config_set_struct hid_printer_config_desc = 
{
    .config = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_config), 
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength        = sizeof(usb_hid_printer_desc_config_set_struct),
        .bNumInterfaces      = 0x02,
        .bConfigurationValue = 0x01,
        .iConfiguration      = 0x00,
        .bmAttributes        = 0x80,
        .bMaxPower           = 0x32
    },

    .hid_interface = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00,
        .bAlternateSetting    = 0x00,
        .bNumEndpoints        = 0x02,
        .bInterfaceClass      = 0x03,
        .bInterfaceSubClass   = 0x00,
        .bInterfaceProtocol   = 0x00,
        .iInterface           = 0x00
    },

    .hid_vendorhid = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_hid),
             .bDescriptorType = USB_DESCTYPE_HID 
         },
        .bcdHID               = 0x0111,
        .bCountryCode         = 0x00,
        .bNumDescriptors      = 0x01,
        .bDescriptorType      = USB_DESCTYPE_REPORT,
        .wDescriptorLength    = DESC_LEN_REPORT,
    },

    .hid_ep_report_in = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CUSTOMHID_IN_EP,
        .bmAttributes         = 0x03,
        .wMaxPacketSize       = CUSTOMHID_IN_PACKET,
        .bInterval            = 0x20
    },

    .hid_ep_report_out = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress     = CUSTOMHID_OUT_EP,
        .bmAttributes         = 0x03,
        .wMaxPacketSize       = CUSTOMHID_OUT_PACKET,
        .bInterval            = 0x20
    },
  
    .printer_interface = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x01,
        .bAlternateSetting    = 0x00,
        .bNumEndpoints        = 0x02,
        .bInterfaceClass      = 0x07,
        .bInterfaceSubClass   = 0x01,
        .bInterfaceProtocol   = 0x02,
        .iInterface           = 0x00
    },

    .printer_ep_data_in = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = PRINTER_IN_EP,
        .bmAttributes         = 0x02,
        .wMaxPacketSize       = PRINTER_IN_PACKET,
        .bInterval            = 0x00
    },

    .printer_ep_data_out = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = PRINTER_OUT_EP,
        .bmAttributes         = 0x02,
        .wMaxPacketSize       = PRINTER_OUT_PACKET,
        .bInterval            = 0x00
    }
};

/* USB language ID Descriptor */
static const usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(10U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(16U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-', 'H', 'I', 'D', '_', 'P', 'R', 'I', 'N', 'T', 'E', 'R'}
};

/* USBD serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

/* USB string descriptor set */
static uint8_t* usbd_hid_printer_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc hid_printer_desc = {
    .dev_desc    = (uint8_t *)&hid_printer_dev_desc,
    .config_desc = (uint8_t *)&hid_printer_config_desc,
    .strings     = usbd_hid_printer_strings
};

static uint8_t hid_printer_init         (usb_dev *udev, uint8_t config_index);
static uint8_t hid_printer_deinit       (usb_dev *udev, uint8_t config_index);
static uint8_t hid_printer_req_handler  (usb_dev *udev, usb_req *req);

static void hid_printer_data_out        (usb_dev *udev, uint8_t ep_num);
static void hid_printer_data_in         (usb_dev *udev, uint8_t ep_num);

usb_class hid_printer_class = {
    .req_cmd       = 0xFFU,

    .init          = hid_printer_init,
    .deinit        = hid_printer_deinit,
    .req_process   = hid_printer_req_handler,
    .data_in       = hid_printer_data_in,
    .data_out      = hid_printer_data_out
};

/*!
    \brief      initialize the HID/printer device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_init (usb_dev *udev, uint8_t config_index)
{
    /* HID initialization */
    custom_hid_class.init(udev, config_index);

    /* printer initialization */
    printer_class.init(udev, config_index);

    return USBD_OK;
}

/*!
    \brief      deinitialize the HID/printer device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_deinit (usb_dev *udev, uint8_t config_index)
{
    /* HID deinitialize */
    custom_hid_class.deinit(udev, config_index);

    /* printer deinitialize */
    printer_class.deinit(udev, config_index);

    return USBD_OK;
}

/*!
    \brief      handle HID/printer class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_req_handler (usb_dev *udev, usb_req *req)
{
    if ((req->wIndex & 0xFF) == CUSTOM_HID_INTERFACE) {
        return custom_hid_class.req_process(udev, req);
    } else {
        return printer_class.req_process(udev, req);
    }
}

/*!
    \brief      handle data stage
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static void hid_printer_data_in (usb_dev *udev, uint8_t ep_num)
{
    if((CUSTOMHID_IN_EP & 0x7FU) == ep_num) {
        custom_hid_class.data_in(udev, ep_num);
    } else if ((PRINTER_IN_EP & 0x7FU) == ep_num) {
        printer_class.data_in(udev, ep_num);
    } else {
        /* no operation */
    }
}

/*!
    \brief      handle data stage
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static void  hid_printer_data_out (usb_dev *udev, uint8_t ep_num)
{
    if((CUSTOMHID_OUT_EP & 0x7FU) == ep_num) {
        custom_hid_class.data_out(udev, ep_num);
    } else if ((PRINTER_OUT_EP & 0x7FU) == ep_num) {
        printer_class.data_out(udev, ep_num);
    } else {
        /* no operation */
    }
}
