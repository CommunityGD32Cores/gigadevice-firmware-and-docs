/*!
    \file    gd32e103v_usb_audio_codec.h
    \brief   This file contains all the functions prototypes for the audio codec
             low layer driver.

    \version 2020-08-05, V2.0.0, firmware for GD32E10x
    \version 2020-12-31, V2.1.0, firmware for GD32E10x
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

#ifndef GD32E103V_AUDIO_CODEC_H
#define GD32E103V_AUDIO_CODEC_H

#include "usbd_conf.h"

/* select the interrupt preemption priority and sub-priority for DMA interrupt */
#define EVAL_AUDIO_IRQ_PREPRIO          2
#define EVAL_AUDIO_IRQ_SUBRIO           0

/* optional configuration defines parameters */

/* Uncomment defines below to select standard for audio communication between codec and I2S peripheral */
//#define I2S_STANDARD_PHILLIPS
#define I2S_STANDARD_MSB
//#define I2S_STANDARD_LSB

/* Uncomment the defines below to select if the Master clock mode should be enabled or not */
#define CODEC_MCLK_ENABLED
//#define CODEC_MCLK_DISABLED

/* codec audio standards */
#ifdef I2S_STANDARD_PHILLIPS
    #define CODEC_STANDARD                 0x04
    #define I2S_STANDARD                   I2S_STD_PHILLIPS
#elif defined(I2S_STANDARD_MSB)
    #define CODEC_STANDARD                 0x00
    #define I2S_STANDARD                   I2S_STD_MSB
#elif defined(I2S_STANDARD_LSB)
    #define CODEC_STANDARD                 0x08
    #define I2S_STANDARD                   I2S_STD_LSB
#else 
    #error "Error: No audio communication standard selected !"
#endif /* I2S_STANDARD */

/* Hardware Configuration defines parameters */
/* I2S peripheral configuration defines (data and control interface of the audio codec) */
#define CODEC_I2S                      SPI1
#define CODEC_I2S_CLK                  RCU_SPI1
#define CODEC_I2S_ADDRESS              (SPI1+0x0C)
#define CODEC_I2S_IRQ                  SPI1_IRQn
#define CODEC_I2S_WS_PIN               GPIO_PIN_12
#define CODEC_I2S_SCK_PIN              GPIO_PIN_13
#define CODEC_I2S_SD_PIN               GPIO_PIN_15
#define CODEC_I2S_MCK_PIN              GPIO_PIN_6
#define CODEC_I2S_GPIO_CLK             RCU_GPIOB
#define CODEC_I2S_MCK_CLK              RCU_GPIOC
#define CODEC_I2S_GPIO                 GPIOB
#define CODEC_I2S_MCK_GPIO             GPIOC

/* I2S DMA Stream definitions */
#define AUDIO_MAL_DMA_CLOCK            RCU_DMA0
#define AUDIO_MAL_DMA_CHANNEL          DMA_CH4
#define AUDIO_MAL_DMA_IRQ              DMA0_Channel4_IRQn
#define AUDIO_MAL_DMA_FLAG_TC          DMA_FLAG_FTF
#define AUDIO_MAL_DMA_FLAG_HT          DMA_FLAG_HTF
#define AUDIO_MAL_DMA_FLAG_TE          DMA_FLAG_ERR
#define AUDIO_MAL_DMA_FLAG_GL          DMA_INT_FLAG_G
#define AUDIO_MAL_DMA_FLAG_ALL         DMA_INT_FLAG_G
#define AUDIO_MAL_DMA_PERIPH_DATA_SIZE DMA_PERIPHERALDATASIZE_HALFWORD
#define AUDIO_MAL_DMA_MEM_DATA_SIZE    DMA_MEMORYDATASIZE_HALFWORD
#define AUDIO_MAL_DMA                  DMA0
#define Audio_MAL_IRQHandler           DMA0_Channel4_IRQHandler

/* mute commands */
#define AUDIO_MUTE                      0x01U
#define AUDIO_UNMUTE                    0x00U

/* functions return value */
#define AUDIO_OK                        0x00U
#define AUDIO_FAIL                      0xFFU

#define AUDIO_PAUSE                     0U
#define AUDIO_RESUME                    1U

/* audio machine states */
#define AUDIO_STATE_INACTIVE            0x00U
#define AUDIO_STATE_ACTIVE              0x01U
#define AUDIO_STATE_PLAYING             0x02U
#define AUDIO_STATE_PAUSED              0x03U
#define AUDIO_STATE_STOPPED             0x04U
#define AUDIO_STATE_ERROR               0x05U

/* mask for the bit en of the i2s cfgr register */
#define I2S_ENABLE_MASK                 (0x0400)

/* audio commands enumeration */
typedef enum
{
    AUDIO_CMD_PLAY = 1U,
    AUDIO_CMD_PAUSE,
    AUDIO_CMD_STOP,
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
void audio_play(uint32_t addr, uint32_t size);
void audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size);
void audio_stop(void);

#endif /* GD32E103V_AUDIO_CODEC_H */
