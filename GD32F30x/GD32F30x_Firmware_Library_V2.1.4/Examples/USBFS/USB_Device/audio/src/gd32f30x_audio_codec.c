/*!
    \file    gd32f30x_audio_codec.c
    \brief   This file includes the low layer driver for CS4344 Audio Codec

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

#include "gd32f30x_audio_codec.h"
#include "audio_out_itf.h"
#include "audio_core.h"

uint32_t i2s_audiofreq = 0;
dma_parameter_struct dma_initstructure;

/*!
    \brief      initializes the audio codec audio interface (i2s)
    \note       this function assumes that the i2s input clock (through pll_r in 
                devices reva/z and through dedicated plli2s_r in devices revb/y)
                is already configured and ready to be used
    \param[in]  audio_freq: audio frequency to be configured for the i2s peripheral
    \param[out] none
    \retval     none
*/
void codec_audio_interface_init(uint32_t audio_freq)
{
    i2s_audiofreq = audio_freq;

    /* enable the AD_I2S peripheral clock */
    rcu_periph_clock_enable(AD_I2S_CLK);

    /* AD_I2S peripheral configuration */
    spi_i2s_deinit(AD_I2S);

    /* initialize the I2S peripheral with the structure above */
    i2s_psc_config(AD_I2S, audio_freq, I2S_FRAMEFORMAT_DT16B_CH16B, 
#ifdef IIS_MCLK_ENABLED
    I2S_MCKOUT_ENABLE
#elif defined(IIS_MCLK_DISABLED)
    I2S_MCKOUT_DISABLE
#endif
    );

    i2s_init(AD_I2S, I2S_MODE_MASTERTX, I2S_STD_MSB, I2S_CKPL_HIGH);

    /* enable the I2S DMA TX request */
    spi_dma_enable(AD_I2S, SPI_DMA_TRANSMIT);
}

/*!
    \brief      deinitialize the audio codec audio interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_audio_interface_deinit(void)
{
    /* disable the codec_i2s peripheral */
    i2s_disable(AD_I2S);

    /* deinitialize the codec_i2s peripheral */
    spi_i2s_deinit(AD_I2S);

    /* disable the codec_i2s peripheral clock */
    rcu_periph_clock_disable(AD_I2S_CLK);
}

/*!
    \brief      initializes IOs used by the audio codec
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_gpio_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(AD_I2S_GPIO_CLK);
    rcu_periph_clock_enable(AD_I2S_MCK_CLK);

    /* AD_I2S pins configuraiton: WS, SCK and SD pins */
    gpio_init(AD_I2S_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, AD_I2S_WS_PIN);
    gpio_init(AD_I2S_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, AD_I2S_SCK_PIN);
    gpio_init(AD_I2S_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, AD_I2S_SD_PIN);

#ifdef IIS_MCLK_ENABLED
    /* codec_i2s pins configuraiton: mck pin */
    gpio_init(AD_I2S_MCK_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, AD_I2S_MCK_PIN);
#endif /* IIS_MCLK_ENABLED */
}

/*!
    \brief      deinitialize IOs used by the audio codec interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_gpio_deinit(void)
{
    /* deinitialize all the GPIOs used by the driver */
    gpio_init(AD_I2S_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, AD_I2S_WS_PIN );
    gpio_init(AD_I2S_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, AD_I2S_SCK_PIN);
    gpio_init(AD_I2S_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, AD_I2S_SD_PIN);

#ifdef IIS_MCLK_ENABLED
    /* AD_I2S pins deinitialization: MCK pin */
    gpio_init(AD_I2S_MCK_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, AD_I2S_MCK_PIN);
#endif /* IIS_MCLK_ENABLED */
}

/*!
    \brief      initializes dma to prepare for audio data transfer
                from Media to the I2S peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_i2s_dma_init(void)
{
    /* enable the DMA clock */
    rcu_periph_clock_enable(AD_DMA_CLOCK);

    /* configure the DMA Stream */
    dma_deinit(AD_DMA, AD_DMA_CHANNEL);

    /* set the parameters to be configured */
    dma_initstructure.periph_addr = AD_I2S_ADDRESS;
    dma_initstructure.memory_addr = (uint32_t)0;
    dma_initstructure.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_initstructure.number = (uint32_t)0xFFFF;
    dma_initstructure.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initstructure.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initstructure.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_initstructure.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_circulation_disable(AD_DMA, AD_DMA_CHANNEL);
    dma_initstructure.priority = DMA_PRIORITY_HIGH;

    dma_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);

    /* enable the selected dma interrupts */
    dma_interrupt_enable(AD_DMA, AD_DMA_CHANNEL, DMA_INT_FTF);

    /* I2S DMA IRQ channel configuration */
    nvic_irq_enable(AD_DMA_IRQ, AD_IRQ_PREPRIO, AD_IRQ_SUBRIO);
}

/*!
    \brief      restore default state of the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void codec_i2s_dma_deinit(void)
{
    /* deinitialize the NVIC interrupt for the I2S DMA Stream */
    nvic_irq_disable(AD_DMA_IRQ);

    /* disable the DMA channel before the deinitialization */
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

    /* deinitialize the DMA channel */
    dma_deinit(AD_DMA, AD_DMA_CHANNEL);
}

/*!
    \brief      starts playing audio stream from the audio media
    \param[in]  addr: pointer to the audio stream buffer
    \param[in]  size: number of data in the audio stream buffer
    \param[out] none
    \retval     none
*/
void audio_play(uint32_t addr, uint32_t size)
{
    /* disable the I2S DMA Stream*/
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

    /* clear the Interrupt flag */
    dma_interrupt_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_INT_FLAG_FTF);

    /* configure the buffer address and size */
    dma_initstructure.memory_addr = (uint32_t)addr;
    dma_initstructure.number = (uint32_t)size;

    /* configure the DMA Stream with the new parameters */
    dma_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);

    /* enable the I2S DMA Stream*/
    dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);

    /* if the i2s peripheral is still not enabled, enable it */
    if ((SPI_I2SCTL(AD_I2S) & I2S_ENABLE_MASK) == 0) {
        i2s_enable(AD_I2S);
    }
}

/*!
    \brief      pauses or resumes the audio stream playing from the media
    \param[in]  cmd: AD_PAUSE (or 0) to pause, AD_RESUME (or any value different
                from 0) to resume
    \param[in]  addr: address from/at which the audio stream should resume/pause
    \param[in]  size: number of data to be configured for next resume
    \param[out] none
    \retval     none
*/
void audio_pause_resume(uint32_t cmd, uint32_t addr, uint32_t size)
{
    /* pause the audio file playing */
    if (cmd == AD_PAUSE) {
        /* stop the current DMA request by resetting the I2S peripherals */
        codec_audio_interface_deinit();

        /* re-configure the I2S interface for the next resume operation */
        codec_audio_interface_init(i2s_audiofreq);

        /* disable the DMA Stream */
        dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

        /* clear the Interrupt flag */
        dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_FLAG_ALL);
    } else {
        /* configure the buffer address and size */
        dma_initstructure.memory_addr = (uint32_t)addr;
        dma_initstructure.number = (uint32_t)size;

        /* configure the DMA Stream with the new parameters */
        dma_init(AD_DMA, AD_DMA_CHANNEL, &dma_initstructure);

        /* enable the I2S DMA Stream*/
        dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);

        /* if the I2S peripheral is still not enabled, enable it */
        if (0U == (SPI_I2SCTL(AD_I2S) & I2S_ENABLE_MASK)) {
            i2s_enable(AD_I2S);
        }
    }
}

/*!
    \brief      stops audio stream playing on the used media
    \param[in]  none
    \param[out] none
    \retval     none
*/
void audio_stop(void)
{
    /* stop the current DMA request by resetting the I2S cell */
    codec_audio_interface_deinit();

    /* re-configure the I2S interface for the next play operation */
    codec_audio_interface_init(i2s_audiofreq);

    /* stop the transfer on the I2S side: Stop and disable the DMA stream */
    dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

    /* clear all the DMA flags for the next transfer */
    dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_FLAG_ALL);
}

/*!
    \brief      this function handles main media layer interrupt
    \param[in]  none
    \param[out] none
    \retval     0 if correct communication, else wrong communication
*/
void AD_DMA_IRQHandler(void)
{
    uint16_t remain_size = 0;

    /* transfer complete interrupt */
    if(dma_flag_get(AD_DMA, AD_DMA_CHANNEL, AD_DMA_FLAG_TC) != RESET){
        /* increment to the next sub-buffer */
        audio_handler.isoc_out_rdptr += audio_handler.dam_tx_len;

        if (audio_handler.isoc_out_rdptr >= (audio_handler.isoc_out_buff + TOTAL_OUT_BUF_SIZE)) {
            /* roll back to the start of buffer */
            audio_handler.isoc_out_rdptr = audio_handler.isoc_out_buff;
        }

        if(audio_handler.isoc_out_wrptr >= audio_handler.isoc_out_rdptr){
            remain_size = audio_handler.isoc_out_wrptr - audio_handler.isoc_out_rdptr;
        }else{
            remain_size = TOTAL_OUT_BUF_SIZE + audio_handler.isoc_out_buff - audio_handler.isoc_out_rdptr;
        }

        /* check if the end of file has been reached */
        if(remain_size > 0){
            dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

            /* clear the Interrupt flag */
            dma_flag_clear(AD_DMA, AD_DMA_CHANNEL, AD_DMA_FLAG_TC);
            /* clear the Interrupt flag */
            dma_interrupt_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_INT_FLAG_ERR);

            DMA_CHMADDR(AD_DMA, AD_DMA_CHANNEL) = (uint32_t)audio_handler.isoc_out_rdptr;

            DMA_CHCNT(AD_DMA, AD_DMA_CHANNEL) = (uint32_t)((remain_size / 2) & DMA_CHANNEL_CNT_MASK);

            /* update the current DMA tx data length */
            audio_handler.dam_tx_len = (remain_size / 2) * 2;

            dma_channel_enable(AD_DMA, AD_DMA_CHANNEL);
        }else{
            /* disable the I2S DMA Stream*/
            dma_channel_disable(AD_DMA, AD_DMA_CHANNEL);

            /* clear the Interrupt flag */
            dma_interrupt_flag_clear(AD_DMA, AD_DMA_CHANNEL, DMA_INT_FLAG_FTF);

            /* clear flag */
            audio_handler.dam_tx_len = 0;
            audio_handler.play_flag = 0U;
        }
    }
}
