/*!
    \file    usbd_conf.h
    \brief   the header file of USBHS device-mode configuration

    \version 2020-08-04, V1.1.0, firmware for GD32VF103
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

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "usb_conf.h"

#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                2
#define USB_STR_DESC_MAX_SIZE           64

/* USB feature -- Self Powered */
//#define USBD_SELF_POWERED

/* USB user string supported */
/* #define USB_SUPPORT_USER_STRING_DESC */

//#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED

#define USBD_AUDIO_INTERFACE            0

#define USBD_AUDIO_FREQ                 16000  /* Audio frequency in Hz for GD32VF103 devices family when 25MHz HSE value
                                                  is used. */

/* Maximum number of supported media (Flash) */
#define MAX_USED_MEMORY_MEDIA           1

#define USB_STRING_COUNT                4

/* Audio endpoint define */
#define AUDIO_TOTAL_IF_NUM              0x02
#define AUDIO_OUT_EP                    EP1_OUT
#define AUDIO_IN_EP                     EP2_IN

#define USB_SERIAL_STRING_SIZE          0x06

/* micro-phone parameter */
#define USBD_MIC_FREQ                   USBD_AUDIO_FREQ_8K
#define MIC_IN_BIT_RESOLUTION           16
#define MIC_IN_CHANNEL_NBR              2 /* Mono = 1, Stereo = 2 */
#define MIC_IN_PACKET                   (uint32_t)(((USBD_MIC_FREQ * \
                                               (MIC_IN_BIT_RESOLUTION/8) *\
                                                MIC_IN_CHANNEL_NBR) /1000))

/* speaker parameter */
#define USBD_SPEAKER_FREQ               USBD_AUDIO_FREQ_16K
#define SPEAKER_OUT_BIT_RESOLUTION      16
#define SPEAKER_OUT_CHANNEL_NBR         2 /* Mono = 1, Stereo = 2 */
#define SPEAKER_OUT_PACKET              (uint32_t)(((USBD_SPEAKER_FREQ * \
                                                        (SPEAKER_OUT_BIT_RESOLUTION/8) *\
                                                         SPEAKER_OUT_CHANNEL_NBR) /1000))

/* audio frequency in Hz */
#define USBD_AUDIO_FREQ_48K             48000U
#define USBD_AUDIO_FREQ_44K             44100U
#define USBD_AUDIO_FREQ_22K             22000U
#define USBD_AUDIO_FREQ_16K             16000U
#define USBD_AUDIO_FREQ_8K              8000U

#define DEFAULT_VOLUME                  65    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

#endif /* USBD_CONF_H */
