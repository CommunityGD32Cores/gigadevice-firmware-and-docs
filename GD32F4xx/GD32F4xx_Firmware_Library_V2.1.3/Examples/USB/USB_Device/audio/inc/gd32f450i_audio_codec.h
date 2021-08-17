/*!
    \file    gd32f450i_audio_codec.h
    \brief   This file contains all the functions prototypes for the audio codec
             low layer driver.

    \version 2020-08-01, V3.0.0, firmware for GD32F4xx
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

#ifndef __GD32F450I_AUDIO_CODEC_H
#define __GD32F450I_AUDIO_CODEC_H

#include "usb_conf.h"

/* CONFIGURATION: audio codec driver configuration parameters */
//#define AUDIO_USE_MACROS

/* For the DMA modes select the interrupt that will be used */
/* #define AUDIO_MAL_DMA_IT_TC_EN */  /* Uncomment this line to enable DMA Transfer Complete interrupt */
/* #define AUDIO_MAL_DMA_IT_HT_EN */  /* Uncomment this line to enable DMA Half Transfer Complete interrupt */
/* #define AUDIO_MAL_DMA_IT_TE_EN */  /* Uncomment this line to enable DMA Transfer Error interrupt */

/* #define USE_DMA_PAUSE_FEATURE *//* Uncomment this line to enable the use of DMA Pause Feature
                                 When this define is enabled, the Pause function will
                                 just pause/resume the DMA without need to reinitialize the
                                 DMA structure. In this case, the buffer pointer should remain
                                 available after resume. */

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define EVAL_AUDIO_IRQ_PREPRIO           0   /* Select the preemption priority level(0 is the highest) */
#define EVAL_AUDIO_IRQ_SUBRIO            0   /* Select the sub-priority level (0 is the highest) */

/* OPTIONAL Configuration defines parameters */

/* Uncomment defines below to select standard for audio communication between codec and I2S peripheral */
//#define I2S_STANDARD_PHILLIPS
//#define I2S_STANDARD_MSB
#define I2S_STANDARD_LSB

/* Uncomment the defines below to select if the Master clock mode should be enabled or not */
#define CODEC_MCLK_ENABLED
/* #define CODEC_MCLK_DISABLED */

/* Uncomment this line to enable verifying data sent to codec after each write operation */
/* #define VERIFY_WRITTENDATA */

/* Hardware Configuration defines parameters */

/* I2S peripheral configuration defines (data and control interface of the audio codec) */
#define CODEC_I2S                      SPI1
#define CODEC_I2S_CLK                  RCU_SPI1
#define CODEC_I2S_ADDRESS              (SPI1+0x0C)
#define CODEC_I2S_IRQ                  SPI1_IRQn

#define CODEC_I2S_WS_PIN               GPIO_PIN_0
#define CODEC_I2S_SCK_PIN              GPIO_PIN_1
#define CODEC_I2S_SD_PIN               GPIO_PIN_1
#define CODEC_I2S_MCK_PIN              GPIO_PIN_6

#define CODEC_I2S_WS_CLK               RCU_GPIOI
#define CODEC_I2S_SCK_CLK              RCU_GPIOI
#define CODEC_I2S_SD_CLK               RCU_GPIOC
#define CODEC_I2S_MCK_CLK              RCU_GPIOA

#define CODEC_I2S_WS_GPIO              GPIOI
#define CODEC_I2S_SCK_GPIO             GPIOI
#define CODEC_I2S_SD_GPIO              GPIOC
#define CODEC_I2S_MCK_GPIO             GPIOA

/* I2S DMA stream definitions */
#define AUDIO_MAL_DMA                  DMA0
#define AUDIO_MAL_DMA_CLOCK            RCU_DMA0
#define AUDIO_MAL_DMA_CHANNEL          DMA_CH4
#define AUDIO_MAL_DMA_IRQ              DMA0_Channel4_IRQn
#define AUDIO_MAL_DMA_FLAG_TC          DMA0_FLAG_TC4
#define AUDIO_MAL_DMA_FLAG_HT          DMA0_FLAG_HT4
#define AUDIO_MAL_DMA_FLAG_TE          DMA0_FLAG_TE4
#define AUDIO_MAL_DMA_FLAG_GL          DMA_INT_FLAG_G
#define AUDIO_MAL_DMA_FLAG_ALL         DMA_INT_FLAG_G
#define AUDIO_MAL_DMA_PERIPH_DATA_SIZE DMA_PERIPH_WIDTH_16BIT
#define AUDIO_MAL_DMA_MEM_DATA_SIZE    DMA_MEMORY_WIDTH_16BIT
#define DMA_MAX_SZE                    0xFFFF

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define CODEC_FLAG_TIMEOUT             ((uint32_t)0x1000)
#define CODEC_LONG_TIMEOUT             ((uint32_t)(300 * CODEC_FLAG_TIMEOUT))

/* audio codec User defines */

/* codec output device */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* volume levels values */
#define DEFAULT_VOLMIN                0x00U
#define DEFAULT_VOLMAX                0xFFU
#define DEFAULT_VOLSTEP               0x04U

#define AUDIO_PAUSE                   0U
#define AUDIO_RESUME                  1U

/* codec power down modes */
#define CODEC_PDWN_HW                 1U
#define CODEC_PDWN_SW                 2U

/* mute commands */
#define AUDIO_MUTE_ON                 1U
#define AUDIO_MUTE_OFF                0U

#define VOLUME_CONVERT(x)    ((x > 100)? 100:((uint8_t)((x * 255) / 100)))
#define DMA_MAX(x)           (((x) <= DMA_MAX_SZE)? (x) : DMA_MAX_SZE)

/* function declarations */
/* generic functions */
/* configure the audio peripherals */
uint32_t eval_audio_init (uint16_t output_device, uint8_t volume, uint32_t audio_freq);
/* de-initializes all the resources used by the codec */
uint32_t eval_audio_deinit (void);
/* starts playing audio stream from a data buffer for a determined size */
uint32_t eval_audio_play (uint16_t* pbuffer, uint32_t size);
/* eval_audio_pause_resume */
uint32_t eval_audio_pause_resume (uint32_t cmd, uint32_t addr, uint32_t size);
/* eval_audio_stop */
uint32_t eval_audio_stop (uint32_t option);

void eval_audio_change_buffer (uint16_t *pdata, uint16_t size);

void eval_audio_frequency_set (uint32_t audio_freq);

/* audio codec functions */

/* high layer codec functions */
/* initializes the audio codec and all related interfaces */
uint32_t codec_init (uint16_t output_device, uint8_t volume, uint32_t audio_freq);
/* restore the audio codec state to default state and free all used resources */
uint32_t codec_deinit (void);
/* pauses and resumes playing on the audio codec */
uint32_t codec_pause_resume (uint32_t cmd);

/* MAL (media access layer) functions */
/* initializes and prepares the Media to perform audio data transfer from Media to the I2S peripheral */
void audio_mal_init (void);
/* restore default state of the used media */
void audio_mal_deinit (void);
/* starts playing audio stream from the audio media */
void audio_mal_play (uint32_t addr, uint32_t size);
/* pauses or resumes the audio stream playing from the media */
void audio_mal_pause_resume  (uint32_t cmd, uint32_t addr, uint32_t size);
/* stops audio stream playing on the used media */
void audio_mal_stop (void);

/* user callbacks: user has to implement these functions in his code if they are needed. */

/* This function is called when the requested data has been completely transferred.
   In Normal mode (when  the define AUDIO_MAL_MODE_NORMAL is enabled) this function
   is called at the end of the whole audio file.
   In circular mode (when  the define AUDIO_MAL_MODE_CIRCULAR is enabled) this 
   function is called at the end of the current buffer transmission. */
void eval_audio_transfercomplete_callback(uint32_t pbuffer, uint32_t size);

/* This function is called when half of the requested buffer has been transferred 
   This callback is useful in Circular mode only (when AUDIO_MAL_MODE_CIRCULAR 
   define is enabled)*/
void eval_audio_halftransfer_callback(uint32_t pbuffer, uint32_t size);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void eval_audio_error_callback(void* pdata);

#endif /* __GD32F450I_AUDIO_CODEC_H */
