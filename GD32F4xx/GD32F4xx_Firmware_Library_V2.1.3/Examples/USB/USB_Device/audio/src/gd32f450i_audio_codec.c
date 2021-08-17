/*!
    \file    gd32f450i_audio_codec.c
    \brief   this file includes the low layer driver for CS43L22 Audio Codec

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

#include "gd32f450i_audio_codec.h"

/* mask for the bit en of the i2s cfgr register */
#define I2S_ENABLE_MASK                    (0x0400)

/* delay for the codec to be correctly reset */
#define CODEC_RESET_DELAY                  (0x4FFF)

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

/* the 7 bits codec address (sent through i2c interface) */
#define CODEC_ADDRESS                      0x94  /* b00100111 */

/* this structure is declared global because it is handled by two different functions */
static dma_single_data_parameter_struct dma_initstructure;

uint32_t audio_totalsize = 0xFFFF; /* the total size of the audio file */
uint32_t audio_remsize   = 0xFFFF; /* the remaining data in audio file */
uint16_t *current_pos;             /* the current position of audio pointer */
uint32_t i2s_audiofreq = 0;

__IO uint32_t codec_timeout = CODEC_LONG_TIMEOUT;

/* These PLL parameters are valid when the f(VCO clock) = 1Mhz */
const uint32_t I2SFreq[8] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000};
const uint32_t I2SPLLN[8] = {256, 429, 213, 429, 426, 271, 258, 344};
const uint32_t I2SPLLR[8] = {5, 4, 2, 4, 4, 6, 3, 1};

/* low layer codec functions */
static void codec_audio_interface_init (uint32_t audio_freq);
static void codec_audio_interface_deinit (void);
static void codec_gpio_init (void);
static void codec_gpio_deinit (void);

/*!
    \brief      configure the audio peripherals
    \param[in]  output_device: audio output device
      \arg        OUTPUT_DEVICE_SPEAKER
      \arg        OUTPUT_DEVICE_HEADPHONE
      \arg        OUTPUT_DEVICE_BOTH
      \arg        OUTPUT_DEVICE_AUTO
    \param[in]  volume: initial volume level (from 0 (mute) to 100 (max))
    \param[in]  audio_freq: Audio frequency used to play the audio stream
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t eval_audio_init(uint16_t output_device, uint8_t volume, uint32_t audio_freq)
{
    /* perform low layer codec initialization */
    if (codec_init(output_device, VOLUME_CONVERT(volume), audio_freq) != 0) {
        return 1;
    } else {
        /* i2s data transfer preparation:
           prepare the media to be used for the audio transfer from memory to i2s peripheral */
        audio_mal_init();

        /* return 0 when all operations are OK */
        return 0;
    }
}

/*!
    \brief      de-initialize all the resources used by the codec (those initialized 
                by eval_audio_init() function) EXCEPT the I2C resources since they are 
                used by the IOExpander as well (and eventually other modules)
    \param[in]  none
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t eval_audio_deinit(void)
{
    /* deinitialize the Media layer */
    audio_mal_deinit();

    /* de-initialize codec */
    codec_deinit();

    return 0;
}

/*!
    \brief      starts playing audio stream from a data buffer for a determined size
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  size: number of audio data bytes
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t eval_audio_play(uint16_t* pbuffer, uint32_t size)
{
    /* set the total number of data to be played (count in half-word) */
    audio_totalsize = size;

    /* update the media layer and enable it for play */  
    audio_mal_play((uint32_t)pbuffer, (uint32_t)(DMA_MAX(audio_totalsize)));

    /* update the remaining number of data to be played */
    audio_remsize = (size) - DMA_MAX(audio_totalsize);

    /* update the current audio pointer position */
    current_pos = pbuffer + DMA_MAX(audio_totalsize);

    return 0;
}

/*!
    \brief      pauses or resumes the audio file stream. in case of using DMA, the DMA
                pause feature is used. in all cases the i2s peripheral is disabled
    \warning    when calling EVAL_AUDIO_PauseResume() function for pause, only
                this function should be called for resume (use of EVAL_AUDIO_Play() 
                function for resume could lead to unexpected behavior).
    \param[in]  cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
                from 0) to resume
    \param[in]  addr: address from/at which the audio stream should resume/pause
    \param[in]  size: number of data to be configured for next resume
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t eval_audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size)
{
    /* call the media layer pause/resume function */
    audio_mal_pause_resume(cmd, addr, size);

    /* return 0 if all operations are OK */
    return 0;
}

/*!
    \brief      stops audio playing and power down the audio codec
    \param[in]  option: can be
      \arg        CODEC_PDWN_SW for software power off (by writing registers)
                  then no need to reconfigure the codec after power on
      \arg        CODEC_PDWN_HW completely shut down the codec (physically). Then need 
                  to reconfigure the codec after power on
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t eval_audio_stop (uint32_t option)
{
    /* call media layer stop function */
    audio_mal_stop();

    /* update the remaining data number */
    audio_remsize = audio_totalsize;    

    /* return 0 when all operations are correctly done */
    return 0;
}

/*!
    \brief      audio change buffer
    \param[in]  pdata: pointer to data buffer'
    \param[in]  size: the size of buffer
    \param[out] none
    \retval     none
*/
void eval_audio_change_buffer (uint16_t *pdata, uint16_t size)
{
    audio_mal_play((uint32_t)pdata, size);
}

/*!
    \brief      set audio frequency
    \param[in]  audio_freq: audio frequency used to play the audio stream
    \param[out] none
    \retval     none
*/
void eval_audio_frequency_set(uint32_t audio_freq)
{
    uint8_t index = 0, freqindex = 0xFF;

    for (index = 0; index < 8; index++) {
        if (I2SFreq[index] == audio_freq) {
            freqindex = index;
        }
    }

    if (freqindex != 0xFF) {
        rcu_plli2s_config(I2SPLLN[freqindex], I2SPLLR[freqindex]);

        rcu_i2s_clock_config(RCU_I2SSRC_PLLI2S);
    }

    codec_audio_interface_init(audio_freq);
}

/* CS4344 Audio Codec Control Functions */

/*!
    \brief      initializes the audio codec and all related interfaces (control 
                interface: i2c and audio interface: i2s)
    \param[in]  output_device:
      \arg        OUTPUT_DEVICE_SPEAKER
      \arg        OUTPUT_DEVICE_HEADPHONE
      \arg        OUTPUT_DEVICE_BOTH
      \arg        OUTPUT_DEVICE_AUTO
    \param[in]  volume: initial volume level (from 0 (mute) to 100 (max))
    \param[in]  audiofreq: audio frequency used to play the audio stream
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t codec_init(uint16_t output_device, uint8_t volume, uint32_t audio_freq)
{
    uint8_t index = 0, freqindex = 0xFF;

    for (index = 0; index < 8; index++) {
        if (I2SFreq[index] == audio_freq) {
            freqindex = index;
        }
    }

    if (freqindex != 0xFF) {
        rcu_plli2s_config(I2SPLLN[freqindex], I2SPLLR[freqindex]);

        rcu_i2s_clock_config(RCU_I2SSRC_PLLI2S);
    }

    /* configure the codec related IOs */
    codec_gpio_init();

    /* configure the i2s peripheral */
    codec_audio_interface_init(audio_freq);

    /* return communication control value */
    return 0;
}

/*!
    \brief      restore the audio codec state to default state and free all used resources
    \param[in]  none
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
uint32_t codec_deinit(void)
{
    uint32_t counter = 0;

    /* deinitialize all use GPIOs */
    codec_gpio_deinit();

    /* deinitialize the codec audio interface (i2s) */
    codec_audio_interface_deinit();

    /* return communication control value */
    return counter;
}

/*!
    \brief      initializes the audio codec audio interface (i2s)
    \note       this function assumes that the i2s input clock (through pll_r in 
                devices reva/z and through dedicated plli2s_r in devices revb/y)
                is already configured and ready to be used
    \param[in]  audio_freq: audio frequency to be configured for the i2s peripheral
    \param[out] none
    \retval     none
*/
static void codec_audio_interface_init(uint32_t audio_freq)
{
    i2s_audiofreq = audio_freq;
    
    /* enable the CODEC_I2S peripheral clock */
    rcu_periph_clock_enable(CODEC_I2S_CLK);

    /* CODEC_I2S peripheral configuration */
    spi_i2s_deinit(CODEC_I2S);

    i2s_psc_config(CODEC_I2S, audio_freq, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);

    i2s_init(CODEC_I2S, I2S_MODE_MASTERTX, I2S_STANDARD, I2S_CKPL_LOW);

    /* enable the I2S DMA TX request */
    spi_dma_enable(CODEC_I2S, SPI_DMA_TRANSMIT);
}

/*!
    \brief      restores the audio codec audio interface to its default state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void codec_audio_interface_deinit(void)
{
    /* disable the codec_i2s peripheral (in case it hasn't already been disabled) */
    i2s_disable(CODEC_I2S);

    /* deinitialize the codec_i2s peripheral */
    spi_i2s_deinit(CODEC_I2S);

    /* disable the codec_i2s peripheral clock */
    rcu_periph_clock_enable(CODEC_I2S_CLK);
}

/*!
    \brief      initializes IOs used by the audio codec (on the control and audio interfaces)
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void codec_gpio_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(CODEC_I2S_WS_CLK);
    rcu_periph_clock_enable(CODEC_I2S_SCK_CLK);
    rcu_periph_clock_enable(CODEC_I2S_SD_CLK);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* CODEC_I2S pins configuration: WS, SCK and SD pins */

    gpio_mode_set(CODEC_I2S_WS_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, CODEC_I2S_WS_PIN);
    gpio_output_options_set(CODEC_I2S_WS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CODEC_I2S_WS_PIN);
    gpio_af_set(CODEC_I2S_WS_GPIO, GPIO_AF_5, CODEC_I2S_WS_PIN);


    gpio_mode_set(CODEC_I2S_SCK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, CODEC_I2S_SCK_PIN);
    gpio_output_options_set(CODEC_I2S_SCK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CODEC_I2S_SCK_PIN);
    gpio_af_set(CODEC_I2S_SCK_GPIO, GPIO_AF_5, CODEC_I2S_SCK_PIN);


    gpio_mode_set(CODEC_I2S_SD_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, CODEC_I2S_SD_PIN);
    gpio_output_options_set(CODEC_I2S_SD_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CODEC_I2S_SD_PIN);
    gpio_af_set(CODEC_I2S_SD_GPIO, GPIO_AF_7, CODEC_I2S_SD_PIN);

#ifdef CODEC_MCLK_ENABLED
    /* enable GPIO clock */
    rcu_periph_clock_enable(CODEC_I2S_MCK_CLK);

    /* codec_i2s pins configuration: MCK pin */
    gpio_mode_set(CODEC_I2S_MCK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, CODEC_I2S_MCK_PIN);
    gpio_output_options_set(CODEC_I2S_MCK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CODEC_I2S_MCK_PIN);
    gpio_af_set(CODEC_I2S_MCK_GPIO, GPIO_AF_6, CODEC_I2S_MCK_PIN);
#endif /* CODEC_MCLK_ENABLED */
}

/*!
    \brief      restores the IOs used by the audio codec interface to their default state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void codec_gpio_deinit(void)
{
    /* deinitialize all the GPIOs used by the driver (EXCEPT the I2C IOs since 
       they are used by the IOExpander as well) */
    gpio_mode_set(CODEC_I2S_SD_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, CODEC_I2S_SD_PIN);
    gpio_mode_set(CODEC_I2S_WS_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, CODEC_I2S_WS_PIN);
    gpio_mode_set(CODEC_I2S_SCK_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, CODEC_I2S_SCK_PIN);

#ifdef CODEC_MCLK_ENABLED
    /* CODEC_I2S pins deinitialization: MCK pin */
    gpio_mode_set(CODEC_I2S_MCK_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, CODEC_I2S_MCK_PIN);
#endif /* CODEC_MCLK_ENABLED */
}

/* Audio MAL Interface Control Functions */

/*!
    \brief      initializes and prepares the Media to perform audio data transfer 
                from Media to the I2S peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_mal_init(void)  
{
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
    NVIC_InitTypeDef NVIC_InitStructure;
#endif

    /* enable the DMA clock */
    rcu_periph_clock_enable(AUDIO_MAL_DMA_CLOCK);

    /* configure the DMA Stream */
    dma_channel_enable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);
    dma_deinit(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* Set the parameters to be configured */
    dma_initstructure.periph_addr = CODEC_I2S_ADDRESS;
    dma_initstructure.memory0_addr = (uint32_t)0;/* this field will be configured in play function */
    dma_initstructure.direction = DMA_MEMORY_TO_PERIPH;
    dma_initstructure.number = (uint32_t)0xFFFE;/* this field will be configured in play function */
    dma_initstructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initstructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initstructure.periph_memory_width = AUDIO_MAL_DMA_PERIPH_DATA_SIZE;
    dma_initstructure.circular_mode = DMA_CIRCULAR_MODE_DISABLE;

    dma_initstructure.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, &dma_initstructure);
    dma_channel_subperipheral_select(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_SUBPERI0);

    /* enable the selected DMA interrupts (selected in "gd32f450i_audio_codec.h" defines) */
#ifdef AUDIO_MAL_DMA_IT_TC_EN
    dma_interrupt_enable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INT_FTF);
#endif /* AUDIO_MAL_DMA_IT_TC_EN */

#ifdef AUDIO_MAL_DMA_IT_HT_EN
    DMA_ITConfig(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INT_HTF);
#endif /* AUDIO_MAL_DMA_IT_HT_EN */

#ifdef AUDIO_MAL_DMA_IT_TE_EN
    DMA_ITConfig(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INT_FTF | DMA_INT_HTF | DMA_INT_ERR);
#endif /* AUDIO_MAL_DMA_IT_TE_EN */

    /* enable the I2S DMA request */
    spi_dma_enable(CODEC_I2S, SPI_DMA_TRANSMIT);

#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
    /* I2S DMA IRQ channel configuration */
    nvic_irq_enable(AUDIO_MAL_DMA_IRQ, EVAL_AUDIO_IRQ_PREPRIO, EVAL_AUDIO_IRQ_SUBRIO);
#endif 
}

/*!
    \brief      restore default state of the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_mal_deinit(void)  
{
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
    NVIC_InitTypeDef NVIC_InitStructure;  

    /* deinitialize the NVIC interrupt for the I2S DMA Stream */
    NVIC_InitStructure.NVIC_IRQChannel = AUDIO_MAL_DMA_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EVAL_AUDIO_IRQ_PREPRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EVAL_AUDIO_IRQ_SUBRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif 

    /* disable the DMA channel before the deinitialize */
    dma_channel_disable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* deinitialize the DMA channel */
    dma_deinit(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* The DMA clock is not disabled, since it can be used by other streams */
}

/*!
    \brief      starts playing audio stream from the audio media
    \param[in]  addr: pointer to the audio stream buffer
    \param[in]  size: number of data in the audio stream buffer
    \param[out] none
    \retval     none
*/
void audio_mal_play(uint32_t addr, uint32_t size)
{
    /* disable the I2S DMA Stream*/
    dma_channel_disable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* clear the Interrupt flag */
    dma_interrupt_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTC_FTFIFC);
    dma_interrupt_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTC_HTFIFC);
    dma_interrupt_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTC_TAEIFC);
    dma_interrupt_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTC_SDEIFC);
    dma_interrupt_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTC_FEEIFC);

    /* configure the buffer address and size */
    dma_initstructure.memory0_addr = (uint32_t)addr;
    dma_initstructure.number = (uint32_t)(size);

    /* configure the DMA Stream with the new parameters */
    dma_single_data_mode_init(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, &dma_initstructure);

    dma_circulation_enable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* enable the I2S DMA Stream*/
    dma_channel_enable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* if the i2s peripheral is still not enabled, enable it */
    if ((SPI_I2SCTL(CODEC_I2S) & I2S_ENABLE_MASK) == 0) {
        i2s_enable(CODEC_I2S);
    }
}

/*!
    \brief      pauses or resumes the audio stream playing from the media
    \param[in]  cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
                from 0) to resume
    \param[in]  addr: address from/at which the audio stream should resume/pause
    \param[in]  size: number of data to be configured for next resume
    \param[out] none
    \retval     none
*/
void audio_mal_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size)
{
    /* pause the audio file playing */
    if (cmd == AUDIO_PAUSE) {
        /* stop the current DMA request by resetting the I2S cell */
        codec_audio_interface_deinit();

        /* re-configure the I2S interface for the next resume operation */
        codec_audio_interface_init(i2s_audiofreq);

        /* disable the DMA Stream */
        dma_channel_disable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);
            
        /* clear the Interrupt flag */
        dma_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTF_FTFIF | DMA_INTF_HTFIF | DMA_INTF_SDEIF | DMA_INTF_TAEIF);
    } else {
        /* configure the buffer address and size */
        dma_initstructure.memory0_addr = (uint32_t)addr;
        dma_initstructure.number = (uint32_t)(size * 2);

        /* configure the DMA Stream with the new parameters */
        dma_single_data_mode_init(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, &dma_initstructure);

        /* enable the I2S DMA Stream*/
        dma_channel_enable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

        /* if the I2S peripheral is still not enabled, enable it */
        if ((SPI_I2SCTL(CODEC_I2S) & I2S_ENABLE_MASK) == 0) {
            i2s_enable(CODEC_I2S);
        }
    }
}

/*!
    \brief      stops audio stream playing on the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_mal_stop(void)
{
    /* stop the Transfer on the I2S side: Stop and disable the DMA stream */
    dma_channel_disable(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL);

    /* clear all the DMA flags for the next transfer */
    dma_flag_clear(AUDIO_MAL_DMA, AUDIO_MAL_DMA_CHANNEL, DMA_INTF_FTFIF | DMA_INTF_HTFIF | DMA_INTF_SDEIF | DMA_INTF_TAEIF);

    /* stop the current DMA request by resetting the I2S cell */
    codec_audio_interface_deinit();

    /* re-configure the I2S interface for the next play operation */
    codec_audio_interface_init(i2s_audiofreq);
}
