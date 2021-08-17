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

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
/* #define USB_DEVICE_LOW_PWR_MODE_SUPPORT */

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint count used by the CDC ACM device */
#define CDC_ACM_CMD_EP                     EP2_IN
#define CDC_ACM_DATA_IN_EP                 EP1_IN
#define CDC_ACM_DATA_OUT_EP                EP3_OUT

#define CDC_ACM_CMD_PACKET_SIZE            8
#define CDC_ACM_DATA_PACKET_SIZE           64

/* endpoint count used by the CDC ACM device */
#define EP_COUNT                           (4U)

#define USB_STRING_COUNT                   4

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BUFFER_ADDRESS                     (0x0000U)

#define USB_PULLUP                         GPIOD
#define USB_PULLUP_PIN                     GPIO_PIN_13
#define RCC_AHBPeriph_GPIO_PULLUP          RCU_GPIOD

#endif /* USBD_CONF_H */
