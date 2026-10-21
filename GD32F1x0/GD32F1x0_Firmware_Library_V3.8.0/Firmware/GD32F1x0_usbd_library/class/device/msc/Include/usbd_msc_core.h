/*!
    \file    usbd_msc_core.h
    \brief   the header file of USB MSC device class core functions

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

#ifndef USBD_MSC_CORE_H
#define USBD_MSC_CORE_H

#include "usbd_core.h"

/* mass storage device class code */
#define USB_CLASS_MSC                     0x08U

/* mass storage subclass code */
#define USB_MSC_SUBCLASS_RBC              0x01U              /*!< MSC RBC subclass */
#define USB_MSC_SUBCLASS_ATAPI            0x02U              /*!< MSC ATAPI subclass */
#define USB_MSC_SUBCLASS_UFI              0x04U              /*!< MSC UFI subclass */
#define USB_MSC_SUBCLASS_SCSI             0x06U              /*!< MSC SCSI subclass */
#define USB_MSC_SUBCLASS_LOCKABLE         0x07U              /*!< MSC LOCKABLE subclass */
#define USB_MSC_SUBCLASS_IEEE1667         0x08U              /*!< MSC IEEE1667 subclass */

/* mass storage interface class control protocol codes */
#define USB_MSC_PROTOCOL_CBI              0x00U              /*!< MSC CBI protocol */
#define USB_MSC_PROTOCOL_CBI_ALT          0x01U              /*!< MSC CBI ALT protocol */
#define USB_MSC_PROTOCOL_BBB              0x50U              /*!< MSC BBB protocol */

/* mass storage request codes */
#define USB_MSC_REQ_CODES_ADSC            0x00U              /*!< MSC ADSC request */
#define USB_MSC_REQ_CODES_GET             0xFCU              /*!< MSC GET request */
#define USB_MSC_REQ_CODES_PUT             0xFDU              /*!< MSC PUT request */
#define USB_MSC_REQ_CODES_GML             0xFEU              /*!< MSC GML request */
#define USB_MSC_REQ_CODES_BOMSR           0xFFU              /*!< MSC BOMSR request */

/* mass storage class-specific request codes */
#define BBB_GET_MAX_LUN                   0xFEU              /*!< MSC BBB get maximum LUN */
#define BBB_RESET                         0xFFU              /*!< MSC BBB reset */

#define USB_MSC_CONFIG_DESC_SIZE          32U                                   /*!< MSC configuration descriptor size */

#define MSC_EPIN_SIZE                     MSC_DATA_PACKET_SIZE                  /*!< MSC endpoint IN size */
#define MSC_EPOUT_SIZE                    MSC_DATA_PACKET_SIZE                  /*!< MSC endpoint OUT size */

/* USB configuration descriptor structure */
typedef struct {
    usb_desc_config         config;                                             /*!< configuration descriptor */
    usb_desc_itf            msc_itf;                                            /*!< interface descriptor */
    usb_desc_ep             msc_epin;                                           /*!< endpoint IN descriptor */
    usb_desc_ep             msc_epout;                                          /*!< endpoint OUT descriptor */
} usb_desc_config_set;

extern usb_desc msc_desc;
extern usb_class msc_class;

#endif /* USBD_MSC_CORE_H */
