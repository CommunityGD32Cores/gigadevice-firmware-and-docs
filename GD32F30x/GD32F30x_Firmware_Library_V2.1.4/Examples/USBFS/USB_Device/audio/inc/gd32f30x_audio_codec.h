/*!
    \file    gd32f30x_usb_audio_codec.h
    \brief   This file contains all the functions prototypes for the audio codec
             low layer driver.

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
    \version 2022-06-10, V3.1.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#ifndef GD32F30X_AUDIO_CODEC_H
#define GD32F30X_AUDIO_CODEC_H

#include "usbd_conf.h"

/* select the interrupt preemption priority and sub-priority for DMA interrupt */
#define AD_IRQ_PREPRIO               2
#define AD_IRQ_SUBRIO                0

/* uncomment the defines below to select if the master clock mode should be enabled or not */
#define IIS_MCLK_ENABLED
//#define IIS_MCLK_DISABLED

/* hardware configuration defines parameters */
/* I2S peripheral configuration defines (data and control interface of the audio codec) */
#define AD_I2S                       SPI1
#define AD_I2S_CLK                   RCU_SPI1
#define AD_I2S_ADDRESS               (SPI1+0x0C)
#define AD_I2S_IRQ                   SPI1_IRQn
#define AD_I2S_WS_PIN                GPIO_PIN_12
#define AD_I2S_SCK_PIN               GPIO_PIN_13
#define AD_I2S_SD_PIN                GPIO_PIN_15
#define AD_I2S_MCK_PIN               GPIO_PIN_6
#define AD_I2S_GPIO_CLK              RCU_GPIOB
#define AD_I2S_MCK_CLK               RCU_GPIOC
#define AD_I2S_GPIO                  GPIOB
#define AD_I2S_MCK_GPIO              GPIOC

/* I2S DMA Stream definitions */
#define AD_DMA_CLOCK                 RCU_DMA0
#define AD_DMA_CHANNEL               DMA_CH4
#define AD_DMA_IRQ                   DMA0_Channel4_IRQn
#define AD_DMA_FLAG_TC               DMA_FLAG_FTF
#define AD_DMA_FLAG_HT               DMA_FLAG_HTF
#define AD_DMA_FLAG_TE               DMA_FLAG_ERR
#define AD_DMA_FLAG_GL               DMA_INT_FLAG_G
#define AD_DMA_FLAG_ALL              DMA_INT_FLAG_G
#define AD_DMA                       DMA0

#define AD_DMA_IRQHandler            DMA0_Channel4_IRQHandler

/* mask for the bit en of the i2s cfgr register */
#define I2S_ENABLE_MASK              (0x0400)

/* audio state */
typedef enum _audio_status {
    AD_OK = 0,
    AD_FAIL,
} audio_status;

typedef enum _audio_ctl {
    AD_PAUSE = 0,
    AD_RESUME,
} audio_ctl;

/* audio machine states */
typedef enum _audio_state {
    AD_STATE_INACTIVE = 0,
    AD_STATE_ACTIVE,
    AD_STATE_PLAYING,
    AD_STATE_PAUSED,
    AD_STATE_STOPPED,
    AD_STATE_ERROR,
} audio_state_enum;

/* audio commands enumeration */
typedef enum
{
    AD_CMD_PLAY = 1U,
    AD_CMD_PAUSE,
    AD_CMD_STOP,
}audio_cmd_enum;

/* function declarations */
/* initializes the audio codec audio interface (i2s) */
void codec_audio_interface_init(uint32_t audio_freq);
/* deinitialize the audio codec audio interface */
void codec_audio_interface_deinit(void);
/* initializes IOs used by the audio codec */
void codec_gpio_init(void);
/* deinitialize IOs used by the audio codec interface */
void codec_gpio_deinit(void);
/* initializes dma to prepare for audio data transfer */
void codec_i2s_dma_init(void);
/* restore default state of the used media */
void codec_i2s_dma_deinit(void);
/* starts playing audio stream from the audio media */
void audio_play(uint32_t addr, uint32_t size);
/* pauses or resumes the audio stream playing from the media */
void audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size);
/* stops audio stream playing on the used media */
void audio_stop(void);

#endif /* GD32F10X_AUDIO_CODEC_H */
