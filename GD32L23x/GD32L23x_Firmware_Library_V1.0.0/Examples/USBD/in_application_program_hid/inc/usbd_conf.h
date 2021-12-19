/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32l23x.h"
#include "gd32l233r_eval.h"

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define USBD_IAP_INTERFACE                 0U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
/* #define USBD_LOWPWR_MODE_ENABLE */

/* endpoint count used by the custom HID device */
#define EP_COUNT                           (2U)

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

#define USB_STRING_COUNT                   4U

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)

#define IAP_IN_EP                          EP_IN(1U)
#define IAP_OUT_EP                         EP_OUT(1U)

#define IAP_IN_PACKET                      17U
#define IAP_OUT_PACKET                     62U

/* Tx buffer address offset */
#define INT_TX_ADDR                        (0x100U)

/* Rx buffer address offset */
#define INT_RX_ADDR                        (0x140U)

#define TRANSFER_SIZE                      60U    /* IAP maximum data packet size */

#define REPORT_IN_COUNT                    0x10U
#define REPORT_OUT_COUNT                   ((TRANSFER_SIZE) + 1U)

#define PAGE_SIZE                          4096U  /* MCU page size */

/* memory address from where user application will be loaded, which represents
   the DFU code protected against write and erase operations.*/
#define APP_LOADED_ADDR                    0x08008000U

/* maximum number of supported memory media (Flash, RAM or EEPROM and so on) */
#define MAX_USED_MEMORY_MEDIA              1

/* make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)            (uint8_t)((((addr) >= 0x08000000U) && ((addr) < (APP_LOADED_ADDR)))? 1U : 0U)

#endif /* __USBD_CONF_H */
