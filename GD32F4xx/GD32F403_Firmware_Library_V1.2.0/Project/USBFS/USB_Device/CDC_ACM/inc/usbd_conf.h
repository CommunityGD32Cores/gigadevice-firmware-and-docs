/*!
    \file  usbd_conf.h
    \brief USBFS device-mode configuration header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U
#define USB_STR_DESC_MAX_SIZE              64U

/* USB feature -- Self Powered */
#define USBD_SELF_POWERED

#define USB_STRING_COUNT                   4

/* endpoint count used by the CDC ACM device */
#define CDC_ACM_CMD_EP                     EP2_IN
#define CDC_ACM_DATA_IN_EP                 EP1_IN
#define CDC_ACM_DATA_OUT_EP                EP3_OUT

#define CDC_ACM_CMD_PACKET_SIZE            8

#define CDC_ACM_DATA_PACKET_SIZE           64

#endif /* USBD_CONF_H */
