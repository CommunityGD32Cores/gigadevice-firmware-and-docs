/*!
    \file    hid_printer_wrapper.c
    \brief   this file calls to the separate HID and printer class layer handlers

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#define USBD_VID                     0x28E9U
#define USBD_PID                     0x325AU

/* USB standard device descriptor */
usb_desc_dev hid_printer_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = 0x00U,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

/* USB device configuration descriptor */
usb_hid_printer_desc_config_set hid_printer_config_desc = 
{
    .config = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_config), 
             .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength         = sizeof(usb_hid_printer_desc_config_set),
        .bNumInterfaces       = 0x02U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0xA0U,
        .bMaxPower            = 0x32U
    },

    .hid_itf = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x02U,
        .bInterfaceClass      = USB_HID_CLASS,
        .bInterfaceSubClass   = 0x00U,
        .bInterfaceProtocol   = 0x00U,
        .iInterface           = 0x00U
    },

    .hid_vendor = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_hid),
             .bDescriptorType = USB_DESCTYPE_HID 
         },
        .bcdHID               = 0x0111U,
        .bCountryCode         = 0x00U,
        .bNumDescriptors      = 0x01U,
        .bDescriptorType      = USB_DESCTYPE_REPORT,
        .wDescriptorLength    = DESC_LEN_REPORT,
    },

    .hid_epin = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CUSTOMHID_IN_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = CUSTOMHID_IN_PACKET,
        .bInterval            = 0x20U
    },

    .hid_epout = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress     = CUSTOMHID_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = CUSTOMHID_OUT_PACKET,
        .bInterval            = 0x20U
    },

    .printer_itf = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x01U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x02U,
        .bInterfaceClass      = USB_CLASS_PRINTER,
        .bInterfaceSubClass   = USB_SUBCLASS_PRINTER,
        .bInterfaceProtocol   = PROTOCOL_BI_DIRECTIONAL_ITF,
        .iInterface           = 0x00U
    },

    .printer_epin = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = PRINTER_IN_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = PRINTER_IN_PACKET,
        .bInterval            = 0x00U
    },

    .printer_epout = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = PRINTER_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = PRINTER_OUT_PACKET,
        .bInterval            = 0x00U
    },
};

/* USB language ID Descriptor */
static usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .wLANGID              = ENG_LANGID
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
    .unicode_string = {'G', 'D', '3', '2', '-', 'U', 'S', 'B', '_','H', 'I','D', '_','P', 'r', 'i', 'n', 't', 'e', 'r'}
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

/* USB string descriptor */
static uint8_t* usbd_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc hid_printer_desc = {
    .dev_desc    = (uint8_t *)&hid_printer_dev_desc,
    .config_desc = (uint8_t *)&hid_printer_config_desc,
    .strings     = usbd_strings
};

static uint8_t hid_printer_init         (usb_dev *udev, uint8_t config_index);
static uint8_t hid_printer_deinit       (usb_dev *udev, uint8_t config_index);
static uint8_t hid_printer_req_handler  (usb_dev *udev, usb_req *req);
static void hid_printer_data_in         (usb_dev *udev, uint8_t ep_num);
static void hid_printer_data_out        (usb_dev *udev, uint8_t ep_num);

usb_class hid_printer_class = {
    .init          = hid_printer_init,
    .deinit        = hid_printer_deinit,
    .req_process   = hid_printer_req_handler,
    .data_in       = hid_printer_data_in,
    .data_out      = hid_printer_data_out
};

/*!
    \brief      initialize the printer device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_init (usb_dev *udev, uint8_t config_index)
{
    /* hid initialization */
    custom_hid_class.init(udev, config_index);

    /* printer initialization */
    printer_class.init(udev, config_index);

    return USBD_OK;
}

/*!
    \brief      de-initialize the printer device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_deinit (usb_dev *udev, uint8_t config_index)
{
    /* hid deinitialize */
    custom_hid_class.deinit(udev, config_index);

    /* printer deinitialize */
    printer_class.deinit(udev, config_index);

    return USBD_OK;
}

/*!
    \brief      handle the printer class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_printer_req_handler (usb_dev *udev, usb_req *req)
{
    if((req->wIndex & 0xFF) == HID_INTERFACE){
        return custom_hid_class.req_process(udev, req);
    }else{
        return printer_class.req_process(udev, req);
    }
}

/*!
    \brief      handle printer data
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
static void hid_printer_data_in (usb_dev *udev, uint8_t ep_num)
{
    if((CUSTOMHID_IN_EP & 0x7F) == ep_num){
        custom_hid_class.data_in(udev, ep_num);
    }else if((PRINTER_IN_EP & 0x7F) == ep_num){
        printer_class.data_in(udev, ep_num);
    }
}

/*!
    \brief      handle printer data
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
static void hid_printer_data_out (usb_dev *udev, uint8_t ep_num)
{
    if((CUSTOMHID_OUT_EP & 0x7F) == ep_num){
        custom_hid_class.data_out(udev, ep_num);
    }else if((PRINTER_OUT_EP & 0x7F) == ep_num){
        printer_class.data_out(udev, ep_num);
    }
}
