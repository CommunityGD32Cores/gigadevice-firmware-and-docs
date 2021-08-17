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

#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                1
#define USB_STR_DESC_MAX_SIZE           64

/* USB feature -- Self Powered */
#define USBD_SELF_POWERED

#define USB_STRING_COUNT                4

/* class layer parameter */
#define PRINTER_IN_EP                   EP1_IN
#define PRINTER_OUT_EP                  EP1_OUT


#ifdef USE_USBHS  
    #ifdef USE_ULPI_PHY
        #define PRINTER_DATA_PACKET_SIZE    512
    #else
        #define PRINTER_DATA_PACKET_SIZE    64
    #endif
#else  /* USE_USBFS */
    #define PRINTER_DATA_PACKET_SIZE        64
#endif

#define PRINTER_IN_PACKET               PRINTER_DATA_PACKET_SIZE
#define PRINTER_OUT_PACKET              PRINTER_DATA_PACKET_SIZE

#endif /* USBD_CONF_H */
