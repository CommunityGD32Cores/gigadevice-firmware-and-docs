/*!
    \file    usbd_hw.h
    \brief   usbd hardware configure file

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

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32f30x.h"
#include "gd32f303e_eval.h"

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define CUSTOM_HID_INTERFACE               0U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
//#define USB_DEVICE_LOW_PWR_MODE_SUPPORT

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint count used by the CDC ACM device */
#define CUSTOMHID_IN_EP                    EP_IN(1U)
#define CUSTOMHID_OUT_EP                   EP_OUT(1U)

#define CUSTOMHID_IN_PACKET                2U
#define CUSTOMHID_OUT_PACKET               2U

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0xA0U)

/* CDC data Tx buffer address offset */
#define HID_TX_ADDR                        (0x140U)

/* CDC data Rx buffer address offset */
#define HID_RX_ADDR                        (0x100U)

/* endpoint count used by the CDC ACM device */
#define EP_COUNT                           (4U)

#define USB_STRING_COUNT                   4U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

#define USB_PULLUP                         GPIOG
#define USB_PULLUP_PIN                     GPIO_PIN_8
#define RCC_AHBPeriph_GPIO_PULLUP          RCU_GPIOG

#endif /* __USBD_CONF_H */
