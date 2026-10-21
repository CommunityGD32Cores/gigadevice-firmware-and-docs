/*!
    \file    usb_iap_core.c
    \brief   IAP driver

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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
#include "usb_iap_core.h"
#include "flash_operation.h"
#include <string.h>

#define USBD_VID                     0x28E9U
#define USBD_PID                     0x028BU

/* Note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
usb_desc_dev iap_dev_desc = {
    .header =
    {
        .bLength          = USB_DEV_DESC_LEN,
        .bDescriptorType  = USB_DESCTYPE_DEV
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
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configure descriptor */
usb_hid_desc_config_set iap_config_desc = {
    .config =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_config),
            .bDescriptorType = USB_DESCTYPE_CONFIG
        },
        .wTotalLength         = USB_DESC_LEN_IAP_CONFIG_SET,
        .bNumInterfaces       = 0x01U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x80U,
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
        .wDescriptorLength    = USB_DESC_LEN_IAP_REPORT
    },

    .hid_epin =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_ep),
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress     = IAP_IN_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = IAP_IN_PACKET,
        .bInterval            = 0x01U
    },

    .hid_epout =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_ep),
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress     = IAP_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = IAP_OUT_PACKET,
        .bInterval            = 0x01U
    }
};

/* USB language ID Descriptor */
usb_desc_LANGID usbd_language_id_desc = {
    .header =
    {
        .bLength         = sizeof(usb_desc_LANGID),
        .bDescriptorType = USB_DESCTYPE_STR
    },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(10U),
        .bDescriptorType = USB_DESCTYPE_STR
    },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static usb_desc_str product_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(12U),
        .bDescriptorType = USB_DESCTYPE_STR
    },
    .unicode_string = {'G', 'D', '3', '2', '-', 'U', 'S', 'B', '_', 'I', 'A', 'P'}
};

/* USB serial string */
static usb_desc_str serial_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(2U),
        .bDescriptorType = USB_DESCTYPE_STR
    }
};

/* USB string descriptor set */
uint8_t *usbd_iap_strings[] = {
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc iap_desc = {
    .dev_desc    = (uint8_t *)&iap_dev_desc,
    .config_desc = (uint8_t *)&iap_config_desc,
    .strings     = usbd_iap_strings
};

/* local function prototypes ('static') */
static uint8_t iap_init(usb_dev *udev, uint8_t config_index);
static uint8_t iap_deinit(usb_dev *udev, uint8_t config_index);
static uint8_t iap_req_handler(usb_dev *udev, usb_req *req);
static void    iap_data_out(usb_dev *udev, uint8_t ep_num);

/* IAP requests management functions */
static void iap_req_erase(usb_dev *udev);
static void iap_req_download(usb_dev *udev);
static void iap_req_read_optionbyte(usb_dev *udev);
static void iap_req_write_optionbyte(usb_dev *udev);
static void iap_req_leave(usb_dev *udev);
static void iap_address_send(usb_dev *udev);
static void iap_req_upload(usb_dev *udev);
static void iap_check_rdp(usb_dev *udev);

usb_class iap_class = {
    .init            = iap_init,
    .deinit          = iap_deinit,
    .req_process     = iap_req_handler,
    .data_out        = iap_data_out
};

/* USB custom HID device report descriptor */
const uint8_t iap_report_desc[USB_DESC_LEN_IAP_REPORT] = {
    0x05, 0x01,     /* USAGE_PAGE (Generic Desktop)   */
    0x09, 0x00,     /* USAGE (Custom Device)          */
    0xa1, 0x01,     /* COLLECTION (Application)       */

    /* IAP command and data */
    0x85, 0x01,     /* REPORT_ID (0x01)               */
    0x09, 0x01,     /* USAGE (IAP command)            */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)            */
    0x25, 0xff,     /* LOGICAL_MAXIMUM (255)          */
    0x75, 0x08,     /* REPORT_SIZE (8)                */
    0x95, REPORT_OUT_COUNT,     /* REPORT_COUNT (63)  */
    0x91, 0x82,     /* OUTPUT (Data,Var,Abs,Vol)      */

    /* device status and option byte */  
    0x85, 0x02,     /* REPORT_ID (0x02)               */
    0x09, 0x02,     /* USAGE (Status and option byte) */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0)            */
    0x25, 0xff,     /* LOGICAL_MAXIMUM (255)          */
    0x75, 0x08,     /* REPORT_SIZE (8)                */
    0x95, REPORT_IN_COUNT,     /* REPORT_COUNT (16)   */
    0x81, 0x82,     /* INPUT (Data,Var,Abs,Vol)       */

    0xc0            /* END_COLLECTION                 */
};

/*!
    \brief      initialize the HID device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t iap_init(usb_dev *udev, uint8_t config_index)
{
    static usbd_iap_handler iap_handler;

    /* initialize TX endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, INT_TX_ADDR, &(iap_config_desc.hid_epin));

    /* initialize RX endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, INT_RX_ADDR, &(iap_config_desc.hid_epout));

    /* unlock the internal flash */
    fmc_unlock();

    memset((void *)&iap_handler, 0, sizeof(usbd_iap_handler));

    /* prepare receive Data */
    usbd_ep_recev(udev, IAP_OUT_EP, iap_handler.report_buf, IAP_OUT_PACKET);

    udev->ep_transc[EP_ID(IAP_OUT_EP)][TRANSC_OUT] = iap_class.data_out;

    iap_handler.base_address = APP_LOADED_ADDR;

    udev->class_data[USBD_IAP_INTERFACE] = (void *)&iap_handler;

    return USBD_OK;
}

/*!
    \brief      de-initialize the HID device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t iap_deinit(usb_dev *udev, uint8_t config_index)
{
    /* deinitialize HID endpoints */
    usbd_ep_deinit(udev, IAP_IN_EP);
    usbd_ep_deinit(udev, IAP_OUT_EP);

    /* lock the internal flash */
    fmc_lock();

    return USBD_OK;
}

/*!
    \brief      handle the HID class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t iap_req_handler(usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    switch(req->bRequest) {
    case GET_REPORT:
        /* no use for this driver */
        break;

    case GET_IDLE:
        usb_transc_config(&udev->transc_in[0], (uint8_t *)&iap->idlestate, 1U, 0U);

        status = REQ_SUPP;
        break;

    case GET_PROTOCOL:
        usb_transc_config(&udev->transc_in[0], (uint8_t *)&iap->protocol, 1U, 0U);

        status = REQ_SUPP;
        break;

    case SET_REPORT:
        iap->reportID = (uint8_t)(req->wValue);

        usb_transc_config(&udev->transc_out[0], iap->report_buf, req->wLength, 0U);

        status = REQ_SUPP;
        break;

    case SET_IDLE:
        iap->idlestate = (uint8_t)(req->wValue >> 8);

        status = REQ_SUPP;
        break;

    case SET_PROTOCOL:
        iap->protocol = (uint8_t)(req->wValue);

        status = REQ_SUPP;
        break;

    case USB_GET_DESCRIPTOR:
        if(USB_DESCTYPE_REPORT == (req->wValue >> 8)) {
            usb_transc_config(&udev->transc_in[0], \
                              (uint8_t *)iap_report_desc, \
                              USB_MIN(USB_DESC_LEN_IAP_REPORT, req->wLength), \
                              0U);

            return REQ_SUPP;
        }
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      handle data out stage
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
static void iap_data_out(usb_dev *udev, uint8_t ep_num)
{
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    if (IAP_HOST_ID == iap->report_buf[0]){
        switch (iap->report_buf[1]){
        case IAP_DOWNLOAD:
            iap_req_download(udev);
            break;

        case IAP_ERASE:
            iap_req_erase(udev);
            break;

        case IAP_READ_OPTION_BYTE:
            iap_req_read_optionbyte(udev);
            break;

        case IAP_LEAVE:
            iap_req_leave(udev);
            break;

        case IAP_GETBIN_ADDRESS:
            iap_address_send(udev);
            break;

        case IAP_WRITE_OPTION_BYTE:
            iap_req_write_optionbyte(udev);
            break;

        case IAP_UPLOAD:
            iap_req_upload(udev);
            break;

        case IAP_CHECK_RDP:
            iap_check_rdp(udev);
            break;

        default:
            break;
        }
    }

    usbd_ep_recev(udev, IAP_OUT_EP, iap->report_buf, IAP_OUT_PACKET);
}

/*!
    \brief      send IAP report
    \param[in]  udev: pointer to USB device instance
    \param[in]  report: pointer to HID report
    \param[in]  len: data length
    \param[out] none
    \retval     USB device operation status
*/
uint8_t iap_report_send(usb_dev *udev, uint8_t *report, uint16_t len)
{
    usbd_ep_send(udev, IAP_IN_EP, report, len);

    return USBD_OK;
}

/*!
    \brief      handle the IAP_DNLOAD request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_download (usb_dev *udev)
{
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    iap->dev_status[0] = IAP_DEVICE_ID;

    /* get the target address to download */
    iap->base_address  = iap->report_buf[2];
    iap->base_address |= (uint32_t)iap->report_buf[3] << 8U;
    iap->base_address |= (uint32_t)iap->report_buf[4] << 16U;
    iap->base_address |= (uint32_t)iap->report_buf[5] << 24U;
    /* program the target address */
    if(FMC_READY == iap_data_write(&iap->report_buf[6], iap->base_address, TRANSFER_SIZE)){
        iap->dev_status[1] = OPERATION_SUCCESS;
    }else{
        iap->dev_status[1] = OPERATION_FAIL;
    }
    iap_report_send (udev, iap->dev_status, IAP_IN_PACKET);

}
/*!
    \brief      handle the IAP_WRITE_OPTION_BYTE request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_write_optionbyte(usb_dev *udev)
{
    uint32_t option_byte_addr = 0;
    uint16_t option_byte_size = 0;
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];
    /* get option byte address address */
    option_byte_addr  = iap->report_buf[2];
    option_byte_addr |= (uint32_t)iap->report_buf[3] << 8U;
    option_byte_addr |= (uint32_t)iap->report_buf[4] << 16U;
    option_byte_addr |= (uint32_t)iap->report_buf[5] << 24U;
    /* get option byte address size */
    if(OPT_BYTE_ADDR == option_byte_addr){
       option_byte_size = OPT_BYTE_SIZE;
    }

    iap->dev_status[0] = IAP_DEVICE_ID;
    /* write option byte address data */
    if(FMC_READY == option_byte_write(option_byte_addr,&iap->report_buf[6],option_byte_size)){
        iap->dev_status[1] = OB_WRITE_SUCCESS;
    }else{
        iap->dev_status[1] = OPERATION_FAIL;
    }
    iap_report_send (udev, iap->dev_status, IAP_IN_PACKET);
}

/*!
    \brief      handle the IAP_ERASE request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_erase(usb_dev *udev)
{
    uint32_t addr = 0U;

    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    /* get base address to erase */
    iap->base_address  = iap->report_buf[2];
    iap->base_address |= (uint32_t)iap->report_buf[3] << 8;
    iap->base_address |= (uint32_t)iap->report_buf[4] << 16;
    iap->base_address |= (uint32_t)iap->report_buf[5] << 24;

    /* get file length */
    iap->file_length = iap->report_buf[6];
    iap->file_length |= (uint32_t)iap->report_buf[7] << 8;
    iap->file_length |= (uint32_t)iap->report_buf[8] << 16;
    iap->file_length |= (uint32_t)iap->report_buf[9] << 24;


    /* check if the address is in protected area */
    if(IS_PROTECTED_AREA(iap->base_address)) {
        return;
    }

    addr = iap->base_address;
    iap->dev_status[0] = IAP_DEVICE_ID;

    /* unlock the flash program erase controller */
    fmc_unlock();

    if(FMC_READY == flash_erase(addr, iap->file_length)){
        iap->dev_status[1] = OPERATION_SUCCESS;
    }else{
        iap->dev_status[1] = OPERATION_FAIL;
    }

    fmc_lock();

    usbd_ep_send(udev, IAP_IN_EP, iap->dev_status, IAP_IN_PACKET);
}

/*!
    \brief      handle the IAP_OPTION_BYTE request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_read_optionbyte(usb_dev *udev)
{
    uint8_t i = 0U;
    uint32_t option_size = 0U;
    uint32_t temp;
    uint32_t option_address = 0;

    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];
    /* read option address address */
    option_address = iap->report_buf[2] + (iap->report_buf[3] << 8) +  (iap->report_buf[4] << 16) + (iap->report_buf[5] << 24);

    iap->option_byte[0] = IAP_DEVICE_ID;

    if(OPT_BYTE_ADDR == option_address){
        option_size = OPT_BYTE_SIZE;
    }
    /* read option address content */
    for(i = 0U; i < (option_size / 4U); i++){
        temp =  *(uint32_t *)option_address;
        iap->option_byte[4*i + 5] = temp >> 24;
        iap->option_byte[4*i + 4] = temp >> 16;
        iap->option_byte[4*i + 3] = temp >> 8;
        iap->option_byte[4*i + 2] = temp;
        option_address = option_address + 4U;
    }
    iap->option_byte[1] = OPERATION_SUCCESS;

    iap_report_send(udev, iap->option_byte, IAP_IN_PACKET);
}

/*!
    \brief      handle the IAP_LEAVE request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_leave(usb_dev *udev)
{
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    /* get base address to erase */
    iap->base_address  = iap->report_buf[2];
    iap->base_address |= (uint32_t)iap->report_buf[3] << 8;
    iap->base_address |= (uint32_t)iap->report_buf[4] << 16;
    iap->base_address |= (uint32_t)iap->report_buf[5] << 24;

    iap->dev_status[0] = IAP_DEVICE_ID;
    iap->dev_status[1] = LEAVE_FINISH;

    usbd_ep_send(udev, IAP_IN_EP, iap->dev_status, IAP_IN_PACKET);

    usbd_disconnect(udev);
    /* the interrupt of target application to jump is recommended to prior to the USBFS interrupt */
    rcu_deinit();
    jump_to_execute(iap->base_address);
}

/*!
    \brief      handle the IAP_SEND_ADDRESS request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_address_send(usb_dev *udev)
{
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    iap->bin_addr[0] = IAP_DEVICE_ID;
    /* get app boundary address */
    iap->bin_addr[1] = (uint8_t)(APP_LOADED_ADDR);
    iap->bin_addr[2] = (uint8_t)(APP_LOADED_ADDR >> 8);
    iap->bin_addr[3] = (uint8_t)(APP_LOADED_ADDR >> 16);
    iap->bin_addr[4] = (uint8_t)(APP_LOADED_ADDR >> 24);

    iap_report_send(udev, iap->bin_addr, IAP_IN_PACKET);
}

/*!
    \brief      handle the IAP_UPLOAD request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_req_upload(usb_dev *udev)
{
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];
    uint32_t bin_flash_addr = APP_LOADED_ADDR;
    uint16_t packet_valid_length = 0U;
    uint16_t i = 0U;

    iap->bin_addr[0] = IAP_DEVICE_ID;
    /* get target flash address */
    bin_flash_addr  = iap->report_buf[2];
    bin_flash_addr |= (uint32_t)iap->report_buf[3] << 8;
    bin_flash_addr |= (uint32_t)iap->report_buf[4] << 16;
    bin_flash_addr |= (uint32_t)iap->report_buf[5] << 24;
    /* get current packet valid length */
    packet_valid_length = iap->report_buf[6];
    packet_valid_length |= iap->report_buf[7]<< 8;
    /* get target flash address content */
    for(i= 0U; i< packet_valid_length; i++) {
        iap->bin_addr[i+1] = REG8(bin_flash_addr+i);
    }

    iap_report_send(udev, iap->bin_addr, IAP_IN_PACKET);
}

/*!
    \brief      handle the IAP_CHECK_RDP request
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
static void iap_check_rdp(usb_dev *udev)
{
    uint8_t mode;
    usbd_iap_handler *iap = (usbd_iap_handler *)udev->class_data[USBD_IAP_INTERFACE];

    /* check whether the SPC bit of FMC module is normal state */
    if(0xA5 != REG8(OPT_BYTE_ADDR)){
        mode = IS_RDP_MODE;
    }else{
        mode = IS_NORMAL_MODE;
    }

    iap->bin_addr[0] = IAP_DEVICE_ID;
    iap->bin_addr[1] = mode;

    iap_report_send(udev, iap->bin_addr, IAP_IN_PACKET);
}
