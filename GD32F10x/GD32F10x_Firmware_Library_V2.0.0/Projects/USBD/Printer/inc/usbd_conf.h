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

#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                1

/* class layer parameter */
#define PRINTER_IN_EP                   EP1_IN
#define PRINTER_OUT_EP                  EP1_OUT
#define PRINTER_DATA_PACKET_SIZE        64
#define PRINTER_IN_PACKET               PRINTER_DATA_PACKET_SIZE
#define PRINTER_OUT_PACKET              PRINTER_DATA_PACKET_SIZE

/* USB user string supported */
#define USB_SUPPORT_USER_STRING_DESC

/* endpoint count used by the Printer device */
#define EP_COUNT                        (2)

#define USB_STRING_COUNT                4

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BUFFER_ADDRESS                  (0x0000)

#define USB_PULLUP                      GPIOD
#define USB_PULLUP_PIN                  GPIO_PIN_13
#define RCC_AHBPeriph_GPIO_PULLUP       RCU_GPIOD

#endif /* USBD_CONF_H */
