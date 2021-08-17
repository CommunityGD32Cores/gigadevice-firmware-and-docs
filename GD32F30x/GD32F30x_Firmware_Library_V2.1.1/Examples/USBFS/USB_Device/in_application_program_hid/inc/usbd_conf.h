/*!
    \file    usbd_conf.h
    \brief   the header file of USB device configuration

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

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                       1U
#define USBD_ITF_MAX_NUM                       1U
#define USB_STR_DESC_MAX_SIZE                  64U

#define USBD_IAP_INTERFACE                     0U

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

//#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

#define USB_STRING_COUNT                       4U

#define IAP_IN_EP                              EP1_IN
#define IAP_OUT_EP                             EP1_OUT

#define IAP_IN_PACKET                          24U
#define IAP_OUT_PACKET                         64U
#define TRANSFER_SIZE                          62U

/* maximum number of supported memory media (Flash, RAM or EEPROM and so on) */
#define MAX_USED_MEMORY_MEDIA                  1U

/* MCU page size */
#define PAGE_SIZE                              2048U
#define OPT_BYTE_ADDR1                         0x1FFFF800U

#define REPORT_IN_COUNT                        0x17U
#define REPORT_OUT_COUNT                       ((TRANSFER_SIZE) + 1U)

/* memory address from where user application will be loaded, which represents 
   the DFU code protected against write and erase operations.*/
#define APP_LOADED_ADDR                        0x08008000U

/* make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)               (uint8_t)(((addr >= 0x08000000U) && \
                                                  (addr < (APP_LOADED_ADDR)))? 1U : 0U)

#endif /* __USBD_CONF_H */
