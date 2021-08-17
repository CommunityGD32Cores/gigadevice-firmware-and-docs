/*!
    \file  usbd_conf.h
    \brief the header file of USBHS device-mode configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM             1
#define USBD_ITF_MAX_NUM             1
#define USB_STR_DESC_MAX_SIZE        64

/* USB feature -- Self Powered */
//#define USBD_SELF_POWERED

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

//#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

/* Maximum number of supported media (Flash) */
#define MAX_USED_MEMORY_MEDIA        1

#define USB_STRING_COUNT             6

/* DFU maximum data packet size */
#define TRANSFER_SIZE                2048

/* memory address from where user application will be loaded, which represents 
   the dfu code protected against write and erase operations.*/
#define APP_LOADED_ADDR              0x08004000

/* Make sure the corresponding memory where the DFU code should not be loaded
   cannot be erased or overwritten by DFU application. */
#define IS_PROTECTED_AREA(addr)  (uint8_t)(((addr >= 0x08000000) && (addr < (APP_LOADED_ADDR)))? 1 : 0)

/* DFU endpoint define */
#define DFU_IN_EP                    EP0_IN
#define DFU_OUT_EP                   EP0_OUT


#endif /* USBD_CONF_H */
