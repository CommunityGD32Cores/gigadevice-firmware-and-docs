/*!
    \file  usbd_conf.h
    \brief the header file of USBFS device-mode configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef USBFS_CONF_H
#define USBFS_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                   1
#define USBD_ITF_MAX_NUM                   1
#define USB_STR_DESC_MAX_SIZE              64

/* USB feature -- Self Powered */
#define USBD_SELF_POWERED

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

#define USB_STRING_COUNT                   4

#define IAP_IN_EP                          EP1_IN
#define IAP_OUT_EP                         EP1_OUT

#define IAP_IN_PACKET                      24

#define IAP_OUT_PACKET                     64
#define TRANSFER_SIZE                      62

/* maximum number of supported memory media (Flash, RAM or EEPROM and so on) */
#define MAX_USED_MEMORY_MEDIA              1

/* memory address from where user application will be loaded, which represents 
   the DFU code protected against write and erase operations.*/
#define APP_LOADED_ADDR                    0x08008000

/* make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)            (uint8_t)(((addr >= 0x08000000) && \
                                           (addr < (APP_LOADED_ADDR)))? 1 : 0)

#endif /* USBD_CONF_H */
