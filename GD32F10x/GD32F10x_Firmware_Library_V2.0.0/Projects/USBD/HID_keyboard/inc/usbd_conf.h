/*!
    \file  usbd_conf.h
    \brief usb device driver basic configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "gd32f10x.h"
#include "gd32f10x_eval.h"

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
#define USBD_LOWPWR_MODE_ENABLE

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* link power mode support */
/* #define LPM_ENABLED */

#define USBD_CFG_MAX_NUM             1
#define USBD_ITF_MAX_NUM             1

/* endpoint count used by the HID device */
#define EP_COUNT                     (2)

#define USB_STRING_COUNT             4

#define HID_IN_EP                    EP1_IN
#define HID_OUT_EP                   EP1_OUT

#define HID_IN_PACKET                8
#define HID_OUT_PACKET               8

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BUFFER_ADDRESS               (0x0000)

#endif /* USBD_CONF_H */
