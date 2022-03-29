/*!
    \file    dual_cdc_acm_core.c
    \brief   CDC ACM driver

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
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

#include "usbd_transc.h"
#include "dual_cdc_acm_core.h"

#define USBD_VID                          0x28E9U
#define USBD_PID                          0x028BU

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_desc_dev dual_cdc_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = 0xEFU,
    .bDeviceSubClass       = 0x02U,
    .bDeviceProtocol       = 0x01U,
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
usb_dual_cdc_desc_config_set dual_cdc_config_desc = 
{
    .config = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_config), 
             .bDescriptorType = USB_DESCTYPE_CONFIG,
         },
        .wTotalLength         = sizeof(usb_dual_cdc_desc_config_set),
        .bNumInterfaces       = 0x04U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x80U,
        .bMaxPower            = 0x32U
    },

    .cdc_iad0 = 
    {
         .header = 
         {
             .bLength         = sizeof(usb_desc_IAD), 
             .bDescriptorType = 0x0BU
         },
         .bFirstInterface     = 0x00U,
         .bInterfaceCount     = 0x02U,
         .bFunctionClass      = 0x02U,
         .bFunctionSubClass   = 0x02U,
         .bFunctionProtocol   = 0x01U,
         .iFunction           = 0x00U
    },
    
    .cdc_loopback_interface0 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x01U,
        .bInterfaceClass      = 0x02U,
        .bInterfaceSubClass   = 0x02U,
        .bInterfaceProtocol   = 0x01U,
        .iInterface           = 0x00U
    },

    .cdc_loopback_header0 = 
    {
        .header =
         {
            .bLength         = sizeof(usb_desc_header_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x00U,
        .bcdCDC              = 0x0110U
    },

    .cdc_loopback_call_managment0 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_call_managment_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x01U,
        .bmCapabilities      = 0x00U,
        .bDataInterface      = 0x01U
    },

    .cdc_loopback_acm0 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_acm_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x02U,
        .bmCapabilities      = 0x02U,
    },

    .cdc_loopback_union0 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_union_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x06U,
        .bMasterInterface    = 0x00U,
        .bSlaveInterface0    = 0x01U,
    },

    .cdc_loopback_cmd_endpoint0 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress    = CDC0_CMD_EP,
        .bmAttributes        = 0x03U,
        .wMaxPacketSize      = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval           = 0x0AU
    },

    .cdc_loopback_data_interface0 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_itf), 
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber    = 0x01U,
        .bAlternateSetting   = 0x00U,
        .bNumEndpoints       = 0x02U,
        .bInterfaceClass     = 0x0AU,
        .bInterfaceSubClass  = 0x00U,
        .bInterfaceProtocol  = 0x00U,
        .iInterface          = 0x00U
    },

    .cdc_loopback_out_endpoint0 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CDC0_OUT_EP,
        .bmAttributes         = 0x02U,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    },

    .cdc_loopback_in_endpoint0 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CDC0_IN_EP,
        .bmAttributes         = 0x02U,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    },
    
    .cdc_iad1 = 
    {
         .header = 
         {
             .bLength         = sizeof(usb_desc_IAD), 
             .bDescriptorType = 0x0BU 
         },
         .bFirstInterface     = 0x02U,
         .bInterfaceCount     = 0x02U,
         .bFunctionClass      = 0x02U,
         .bFunctionSubClass   = 0x02U,
         .bFunctionProtocol   = 0x01U,
         .iFunction           = 0x00U
    },

    .cdc_loopback_interface1 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x02U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x01U,
        .bInterfaceClass      = 0x02U,
        .bInterfaceSubClass   = 0x02U,
        .bInterfaceProtocol   = 0x01U,
        .iInterface           = 0x00U
    },

    .cdc_loopback_header1 = 
    {
        .header =
         {
            .bLength         = sizeof(usb_desc_header_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x00U,
        .bcdCDC              = 0x0110U
    },

    .cdc_loopback_call_managment1 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_call_managment_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x01U,
        .bmCapabilities      = 0x00U,
        .bDataInterface      = 0x03U
    },

    .cdc_loopback_acm1 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_acm_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x02U,
        .bmCapabilities      = 0x02U,
    },

    .cdc_loopback_union1 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_union_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x06U,
        .bMasterInterface    = 0x02U,
        .bSlaveInterface0    = 0x03U,
    },

    .cdc_loopback_cmd_endpoint1 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress    = CDC1_CMD_EP,
        .bmAttributes        = 0x03U,
        .wMaxPacketSize      = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval           = 0x0AU
    },

    .cdc_loopback_data_interface1 = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_itf), 
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber    = 0x03U,
        .bAlternateSetting   = 0x00U,
        .bNumEndpoints       = 0x02U,
        .bInterfaceClass     = 0x0AU,
        .bInterfaceSubClass  = 0x00U,
        .bInterfaceProtocol  = 0x00U,
        .iInterface          = 0x00U
    },

    .cdc_loopback_out_endpoint1 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CDC1_OUT_EP,
        .bmAttributes         = 0x02U,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    },

    .cdc_loopback_in_endpoint1 = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CDC1_IN_EP,
        .bmAttributes         = 0x02U,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    }
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
         .bLength         = USB_STRING_LEN(13U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-', 'D', 'U', 'A', 'L', '_', 'C', 'D', 'C'}
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
static uint8_t* usbd_dual_cdc_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc dual_cdc_desc = {
    .dev_desc    = (uint8_t *)&dual_cdc_dev_desc,
    .config_desc = (uint8_t *)&dual_cdc_config_desc,
    .strings     = usbd_dual_cdc_strings
};

/* local function prototypes ('static') */
static uint8_t dual_cdc_init         (usb_dev *udev, uint8_t config_index);
static uint8_t dual_cdc_deinit       (usb_dev *udev, uint8_t config_index);
static uint8_t dual_cdc_req_handler  (usb_dev *udev, usb_req *req);
static uint8_t dual_cdc_ctlx_out     (usb_dev *udev);
static void dual_cdc_data_in         (usb_dev *udev, uint8_t ep_num);
static void dual_cdc_data_out        (usb_dev *udev, uint8_t ep_num);

usb_class dual_cdc_class = {
    .req_cmd       = 0xFFU,

    .init          = dual_cdc_init,
    .deinit        = dual_cdc_deinit,
    .req_process   = dual_cdc_req_handler,
    .ctlx_out      = dual_cdc_ctlx_out,
    .data_in       = dual_cdc_data_in,
    .data_out      = dual_cdc_data_out
};

/*!
    \brief      to receive CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void dual_cdc_data_receive(usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

    cdc->packet_receive = 0U;
    cdc->pre_packet_send = 0U;

    usbd_ep_recev(udev, CDC0_OUT_EP, (uint8_t*)(cdc->data), USB_CDC_RX_LEN);
    usbd_ep_recev(udev, CDC1_OUT_EP, (uint8_t*)(cdc->data), USB_CDC_RX_LEN);
}

/*!
    \brief      send CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void dual_cdc_data_send (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];
    uint32_t data_len = cdc->receive_length;

    if ((0U != data_len) && (1U == cdc->packet_sent)) {
        cdc->packet_sent = 0U;
        usbd_ep_send(udev, CDC0_IN_EP, (uint8_t*)(cdc->data), (uint16_t)data_len);
        usbd_ep_send(udev, CDC1_IN_EP, (uint8_t*)(cdc->data), (uint16_t)data_len);
        cdc->receive_length = 0U;
    }
}

/*!
    \brief      check cdc acm is ready for data transfer
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     0 if cdc is ready, 5 else
*/
uint8_t dual_cdc_check_ready(usb_dev *udev)
{
    if (NULL != udev->class_data[0]) {
        usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

        if ((1U == cdc->packet_receive) && (1U == cdc->pre_packet_send)) {
            return 0U;
        }
    }

    return 5U;
}

/*!
    \brief      initialize the CDC ACM device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t dual_cdc_init (usb_dev *udev, uint8_t config_index)
{
    static usb_cdc_handler cdc_handler;

    /* initialize the data Tx/Rx endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, BULK0_TX_ADDR, &(dual_cdc_config_desc.cdc_loopback_in_endpoint0));
    usbd_ep_init(udev, EP_BUF_SNG, BULK0_RX_ADDR, &(dual_cdc_config_desc.cdc_loopback_out_endpoint0));

    usbd_ep_init(udev, EP_BUF_SNG, BULK1_TX_ADDR, &(dual_cdc_config_desc.cdc_loopback_in_endpoint1));
    usbd_ep_init(udev, EP_BUF_SNG, BULK1_RX_ADDR, &(dual_cdc_config_desc.cdc_loopback_out_endpoint1));

    /* initialize the command Tx endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, INT0_TX_ADDR, &(dual_cdc_config_desc.cdc_loopback_cmd_endpoint0));
    usbd_ep_init(udev, EP_BUF_SNG, INT1_TX_ADDR, &(dual_cdc_config_desc.cdc_loopback_cmd_endpoint1));

    udev->ep_transc[EP_ID(CDC0_IN_EP)][TRANSC_IN] = dual_cdc_class.data_in;
    udev->ep_transc[CDC0_OUT_EP][TRANSC_OUT] = dual_cdc_class.data_out;

    udev->ep_transc[EP_ID(CDC1_IN_EP)][TRANSC_IN] = dual_cdc_class.data_in;
    udev->ep_transc[CDC1_OUT_EP][TRANSC_OUT] = dual_cdc_class.data_out;

    /* initialize cdc handler structure */
    cdc_handler.packet_receive = 0U;
    cdc_handler.packet_sent = 1U;
    cdc_handler.pre_packet_send = 1U; 
    cdc_handler.receive_length = 0U;

    cdc_handler.line_coding = (acm_line){
        .dwDTERate   = 115200U,
        .bCharFormat = 0U,
        .bParityType = 0U,
        .bDataBits   = 0x08U
    };

    udev->class_data[0] = (void *)&cdc_handler;

    return USBD_OK;
}

/*!
    \brief      de-initialize the CDC ACM device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t dual_cdc_deinit (usb_dev *udev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_deinit(udev, CDC0_IN_EP);
    usbd_ep_deinit(udev, CDC0_OUT_EP);

    usbd_ep_deinit(udev, CDC1_IN_EP);
    usbd_ep_deinit(udev, CDC1_OUT_EP);

    /* deinitialize the command Tx endpoint */
    usbd_ep_deinit(udev, CDC0_CMD_EP);
    usbd_ep_deinit(udev, CDC1_CMD_EP);

    return USBD_OK;
}

/*!
    \brief      command data received on control endpoint
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t dual_cdc_ctlx_out (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

    if (NO_CMD != udev->class_core->req_cmd) {
        cdc->packet_receive = 1U;
        cdc->pre_packet_send = 1U;

        udev->class_core->req_cmd = NO_CMD;
    }

    return USBD_OK;
}

/*!
    \brief      handle CDC ACM data in DATA IN transaction
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     USB device operation status
*/
static void dual_cdc_data_in (usb_dev *udev, uint8_t ep_num)
{
    usb_transc *transc = &udev->transc_in[ep_num];
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

    if (transc->xfer_count == transc->max_len) {
        usbd_ep_send(udev, EP_ID(ep_num), NULL, 0U);
    } else {
        cdc->packet_sent = 1U;
        cdc->pre_packet_send = 1U;
    }
}

/*!
    \brief      handle CDC ACM data in DATA OUT transaction
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     USB device operation status
*/
static void dual_cdc_data_out (usb_dev *udev, uint8_t ep_num)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

    cdc->packet_receive = 1U;

    cdc->receive_length = udev->transc_out[ep_num].xfer_count;
}

/*!
    \brief      handle the CDC ACM class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t dual_cdc_req_handler (usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP, noti_buf[10] = {0U};
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->class_data[0];

    acm_notification *notif = (void *)noti_buf;

    switch (req->bRequest) {
    case SEND_ENCAPSULATED_COMMAND:
        break;

    case GET_ENCAPSULATED_RESPONSE:
        break;

    case SET_COMM_FEATURE:
        break;

    case GET_COMM_FEATURE:
        break;

    case CLEAR_COMM_FEATURE:
        break;

    case SET_LINE_CODING:
        /* set the value of the current command to be processed */
        udev->class_core->req_cmd = req->bRequest;

        usb_transc_config(&udev->transc_out[0U], (uint8_t *)&cdc->line_coding, req->wLength, 0U);

        status = REQ_SUPP;
        break;

    case GET_LINE_CODING:
        usb_transc_config(&udev->transc_in[0U], (uint8_t *)&cdc->line_coding, 7U, 0U);

        status = REQ_SUPP;
        break;

    case SET_CONTROL_LINE_STATE:
        notif->bmRequestType = 0xA1U;
        notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
        notif->wIndex = 0U;
        notif->wValue = 0U;
        notif->wLength = 2U;
        noti_buf[8] = (uint8_t)req->wValue & 3U;
        noti_buf[9] = 0U;

        status = REQ_SUPP;
        break;

    case SEND_BREAK:
        break;

    default:
        break;
    }

    return status;
}
