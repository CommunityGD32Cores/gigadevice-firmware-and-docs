/*!
    \file  usbd_conf.h
    \brief usb device driver basic configuration
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"

#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                1

/* class layer parameter */
#define AUDIO_TOTAL_IF_NUM              0x02
#define AUDIO_OUT_EP                    EP1_OUT
#define AUDIO_IN_EP                     EP2_IN

/* Audio frequency in Hz */
#define USBD_AUDIO_FREQ_48K             48000
#define USBD_AUDIO_FREQ_44K             44100
#define USBD_AUDIO_FREQ_16K             16000
#define USBD_AUDIO_FREQ_22K             22000

#define DEFAULT_VOLUME                  65    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

/* USB user string supported */
#define USB_SUPPORT_USER_STRING_DESC

/* endpoint count used by the Printer device */
#define EP_COUNT                        (2)

#define USB_STRING_COUNT                4

/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BUFFER_ADDRESS                  (0x0000)

#define USB_PULLUP                      GPIOC
#define USB_PULLUP_PIN                  GPIO_PIN_2
#define RCC_AHBPeriph_GPIO_PULLUP       RCU_GPIOC

#endif /* USBD_CONF_H */
