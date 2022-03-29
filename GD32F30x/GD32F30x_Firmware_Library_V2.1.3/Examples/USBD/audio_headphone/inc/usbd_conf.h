/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

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

#include "gd32f30x.h"
#include "gd32f303e_eval.h"
#include "gd32f303c_audio_codec.h"

#define USBD_CFG_MAX_NUM                1U
#define USBD_ITF_MAX_NUM                2U

#define USBD_AUDIO_INTERFACE            0U

/* class layer parameter */
#define AUDIO_TOTAL_IF_NUM              0x02U

#define AUDIO_OUT_EP                    EP_OUT(1)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                     (0x20U)
#define EP0_TX_ADDR                     (0x60U)

#define AUDIO_BUF_ADDR                  (AUDIO_BUF0_ADDR | (AUDIO_BUF1_ADDR << 16U))

#define AUDIO_BUF0_ADDR                 (0xA0U)
#define AUDIO_BUF1_ADDR                 (0xF0U)

/* Audio frequency in Hz */
#define USBD_AUDIO_FREQ_48K             48000U
#define USBD_AUDIO_FREQ_44K             44100U
#define USBD_AUDIO_FREQ_16K             16000U
#define USBD_AUDIO_FREQ_22K             22000U

#define DEFAULT_VOLUME                  65U    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

/* USB user string supported */
#define USB_SUPPORT_USER_STRING_DESC

/* endpoint count used by the Printer device */
#define EP_COUNT                        (2U)

#define USB_STRING_COUNT                4U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                   (0x0000U)

#define USB_PULLUP                      GPIOG
#define USB_PULLUP_PIN                  GPIO_PIN_8
#define RCC_AHBPeriph_GPIO_PULLUP       RCU_GPIOG

#endif /* __USBD_CONF_H */
