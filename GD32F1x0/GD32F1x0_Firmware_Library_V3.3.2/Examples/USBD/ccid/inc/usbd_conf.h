/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2020-07-23, V3.0.0, firmware for GD32F1x0
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

#include "gd32f1x0.h"
#include "gd32f1x0r_eval.h"

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define USBD_CCID_INTERFACE                0U

/* smartcard use only one protocol: T = 0 or T = 1 */
#define SC_PROTOCOL_T0
//#define SC_PROTOCOL_T1

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
//#define USB_DEVICE_LOW_PWR_MODE_SUPPORT

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint count used by the CCID device */
#define CCID_BULK_OUT_EP                   EP_OUT(1)
#define CCID_BULK_IN_EP                    EP_IN(2)
#define CCID_INT_IN_EP                     EP_IN(3)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)

/* CCID data Tx buffer address offset */
#define BULK_TX_ADDR                       (0x140U)

/* CCID data Rx buffer address offset */
#define BULK_RX_ADDR                       (0x100U)

/* CCID command Tx buffer address offset */
#define INT_TX_ADDR                        (0xC0U)

/* endpoint maximum packet length */
#define CCID_DATA_PACKET_SIZE              64U
#define CCID_NOTIFY_PACKET_SIZE            8U

/* endpoint count used by the CCID device */
#define EP_COUNT                           4U

/* string count used by the CCID device */
#define USB_STRING_COUNT                   6U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

#define USB_PULLUP                         GPIOC
#define USB_PULLUP_PIN                     GPIO_PIN_2
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOC

#endif/* __USBD_CONF_H */
