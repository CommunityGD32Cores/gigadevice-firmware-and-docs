/*!
    \file  usbd_conf.h
    \brief the header file of USBHS device-mode configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

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

#define CUSTOMHID_IN_EP                    EP1_IN
#define CUSTOMHID_OUT_EP                   EP1_OUT

#define CUSTOMHID_IN_PACKET                2
#define CUSTOMHID_OUT_PACKET               2

#endif /* USBD_CONF_H */
