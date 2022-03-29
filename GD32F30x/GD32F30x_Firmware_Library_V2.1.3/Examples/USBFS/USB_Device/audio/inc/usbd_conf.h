/*!
    \file    usbd_conf.h
    \brief   the header file of USB device configuration

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

#include "usb_conf.h"

/* USB configure exported defines */ 
#define USBD_CFG_MAX_NUM                    1U
#define USBD_ITF_MAX_NUM                    2U

#define USBD_AUDIO_INTERFACE                0U

#define USB_STR_DESC_MAX_SIZE               255U

/* Audio class layer parameter */ 
#define AUDIO_OUT_EP                        EP_OUT(2)
#define AUDIO_IN_EP                         EP_IN(1)

#define USB_STRING_COUNT                    4U

/* micro-phone parameter */
#define USBD_MIC_FREQ                       USBD_AUDIO_FREQ_8K
#define MIC_IN_BIT_RESOLUTION               16U
#define MIC_IN_CHANNEL_NBR                  2U /* Mono = 1, Stereo = 2 */
#define MIC_IN_PACKET                       (uint32_t)(((USBD_MIC_FREQ * \
                                                        (MIC_IN_BIT_RESOLUTION/8U) *\
                                                         MIC_IN_CHANNEL_NBR) /1000U)) 

/* speaker parameter */
#define USBD_SPEAKER_FREQ                   USBD_AUDIO_FREQ_16K
#define SPEAKER_OUT_BIT_RESOLUTION          16U
#define SPEAKER_OUT_CHANNEL_NBR             2U/* Mono = 1, Stereo = 2 */
#define SPEAKER_OUT_PACKET                  (uint32_t)(((USBD_SPEAKER_FREQ * \
                                                        (SPEAKER_OUT_BIT_RESOLUTION/8U) *\
                                                         SPEAKER_OUT_CHANNEL_NBR) /1000U)) 

/* audio frequency in Hz */
#define USBD_AUDIO_FREQ_48K                 48000U
#define USBD_AUDIO_FREQ_44K                 44100U
#define USBD_AUDIO_FREQ_22K                 22000U
#define USBD_AUDIO_FREQ_16K                 16000U
#define USBD_AUDIO_FREQ_8K                  8000U

#define DEFAULT_VOLUME                      65U    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                      To get accurate volume variations, it is possible to use a logarithmic
                                                      coversion table to convert from percentage to logarithmic law.
                                                      In order to keep this example code simple, this conversion is not used.*/

#endif /* __USBD_CONF_H */
