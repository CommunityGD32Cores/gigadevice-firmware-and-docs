/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

#include "gd32f1x0.h"
#include "gd32f1x0r_eval.h"
#include "gd32f1x0_audio_codec.h"

/* USBD endpoint double buffer function enable */
#define USBD_DOUBLE_BUFFER_ENABLE

#define USBD_CFG_MAX_NUM                1U
#define USBD_ITF_MAX_NUM                3U

#define USBD_AD_INTERFACE               0U

/* endpoint count used by the audio device */
#define EP_COUNT                        4U

/* USB string count */
#define USB_STRING_COUNT                4U

#define AD_OUT_EP                       EP_OUT(1)
#define AD_IN_EP                        EP_IN(2)
#define AD_FEEDBACK_IN_EP               EP_IN(3)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                     0x20U
#define EP0_TX_ADDR                     0x60U

/* ISO endpoint, Rx/Tx buffers address offset */
#define AD_TX_BUF_ADDR                  0x00E000A0U
#define AD_RX_BUF_ADDR                  0x01700120U
#define AD_FEEDBACK_TX_BUF_ADDR         0x01D001C0U

/* micro-phone parameter */
#define USBD_MIC_FREQ                   USBD_AD_FREQ_8K
#define MIC_IN_BIT_RESOLUTION           16U
#define MIC_IN_CHANNEL_NBR              2U /* Mono = 1, Stereo = 2 */
#define MIC_IN_PACKET                   (uint32_t)(((USBD_MIC_FREQ * \
                                                    (MIC_IN_BIT_RESOLUTION / 8U) * \
                                                     MIC_IN_CHANNEL_NBR) / 1000U))

/* speaker parameter */
#define USBD_SPEAKER_FREQ               USBD_AD_FREQ_16K
#define SPEAKER_OUT_BIT_RESOLUTION      16U
#define SPEAKER_OUT_CHANNEL_NBR         2U /* Mono = 1, Stereo = 2 */
#define SPEAKER_OUT_PACKET              (uint32_t)(((USBD_SPEAKER_FREQ * \
                                                    (SPEAKER_OUT_BIT_RESOLUTION / 8U) * \
                                                     SPEAKER_OUT_CHANNEL_NBR) / 1000U))

/* feedback parameter */
#define FEEDBACK_FREQ_OFFSET            (USBD_SPEAKER_FREQ / 100U)
#define FEEDBACK_IN_PACKET              3U
#define FEEDBACK_IN_INTERVAL            5U

#define SPEAKER_OUT_MAX_PACKET          (SPEAKER_OUT_PACKET + 16U)

/* Audio frequency in Hz */
#define USBD_AD_FREQ_48K                48000U
#define USBD_AD_FREQ_44K                44100U
#define USBD_AD_FREQ_22K                22000U
#define USBD_AD_FREQ_16K                16000U
#define USBD_AD_FREQ_8K                 8000U

#define DEFAULT_VOLUME                  65U    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                   0x0000U

/*!
    \brief      get the calculate value of i2s sample frequency
    \param[in]  set_freq: setting sample frequence
    \param[out] none
    \retval     i2s sample frequency
    \note       Users need to calculate the actual sampling frequency 
                of I2S module at different frequency points by themselves.
*/
__STATIC_INLINE uint32_t I2S_ACTUAL_SAM_FREQ(uint32_t set_freq)
{
    uint32_t actual_freq;

    switch(set_freq) {
    case USBD_AD_FREQ_48K:
        actual_freq = 46875U;
        break;

    case USBD_AD_FREQ_44K:
        actual_freq = 46875U;
        break;

    case USBD_AD_FREQ_22K:
        actual_freq = 21634U;
        break;

    case USBD_AD_FREQ_16K:
        actual_freq = 15625U;
        break;

    case USBD_AD_FREQ_8K:
        actual_freq = 8035;
        break;

    default:
        actual_freq = set_freq;
        break;
    }

    return actual_freq;
}

#define USB_PULLUP                      GPIOC
#define USB_PULLUP_PIN                  GPIO_PIN_2
#define RCU_PULLUP                      RCU_GPIOC

#endif /* USBD_CONF_H */
