/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

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

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32f10x.h"
#include "gd32f10x_eval.h"

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
#define USBD_LOWPWR_MODE_ENABLE

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* link power mode support */
/* #define LPM_ENABLED */

#define USBD_CFG_MAX_NUM                   1
#define USBD_ITF_MAX_NUM                   1

#define USBD_HID_INTERFACE                 0

/* endpoint count used by the HID device */
#define EP_COUNT                           (2)

#define USB_STRING_COUNT                   4

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_TX_ADDR                        0x20
#define EP0_RX_ADDR                        0x40

#define INT_TX_ADDR                        0x50
#define INT_RX_ADDR                        0x60

#define HID_IN_EP                          EP_IN(1)

#define HID_IN_PACKET                      8

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000)

#define USB_PULLUP                         GPIOD
#define USB_PULLUP_PIN                     GPIO_PIN_13
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOD

#endif /* __USBD_CONF_H */
