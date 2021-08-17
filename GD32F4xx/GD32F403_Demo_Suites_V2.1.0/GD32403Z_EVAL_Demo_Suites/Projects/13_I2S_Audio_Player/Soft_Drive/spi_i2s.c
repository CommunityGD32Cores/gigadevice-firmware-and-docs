/*!
    \file  spi_i2s.c
    \brief SPI I2S configuration file

    \version 2017-02-10, V1.0.0, demo for GD32F403
    \version 2018-10-10, V1.1.0, demo for GD32F403
    \version 2018-12-25, V2.0.0, demo for GD32F403
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "spi_i2s.h" 
#include "gd32f403.h"

/*!
    \brief      configure the I2S1 and SPI0 GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_i2s_gpio_config(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* configure I2S1 pins: PB12(I2S_WS), PB13(I2S_CK), PB15(I2S_DIN) */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15);

    /* PC6(I2S_MCK) */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* configure pcm1770 mode control port: PA5(MCLK), PA7(MDIN) */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_7);

    /* PA4(MSEL) */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    
    /* configure PA4 as pcm1770 mode control chip select pin */
    gpio_bit_set(GPIOA, GPIO_PIN_4);
}

/*!
    \brief      configure the I2S and SPI0
    \param[in]  standard: I2S standard
      \arg        I2S_STD_PHILLIPS: I2S phillips standard
      \arg        I2S_STD_MSB: I2S MSB standard
      \arg        I2S_STD_LSB: I2S LSB standard
      \arg        I2S_STD_PCMSHORT: I2S PCM short standard
      \arg        I2S_STD_PCMLONG: I2S PCM long standard
     \param[in]  mclk_output: I2S master clock output
      \arg        I2S_MCKOUT_ENABLE: I2S master clock output enable
      \arg        I2S_MCKOUT_DISABLE: I2S master clock output disable
     \param[in]  audio_freq: I2S audio sample rate
      \arg        I2S_AUDIOSAMPLE_8K: audio sample rate is 8KHz
      \arg        I2S_AUDIOSAMPLE_11K: audio sample rate is 11KHz
      \arg        I2S_AUDIOSAMPLE_16K: audio sample rate is 16KHz
      \arg        I2S_AUDIOSAMPLE_22K: audio sample rate is 22KHz
      \arg        I2S_AUDIOSAMPLE_32K: audio sample rate is 32KHz
      \arg        I2S_AUDIOSAMPLE_44K: audio sample rate is 44KHz
      \arg        I2S_AUDIOSAMPLE_48K: audio sample rate is 48KHz
      \arg        I2S_AUDIOSAMPLE_96K: audio sample rate is 96KHz
      \arg        I2S_AUDIOSAMPLE_192K: audio sample rate is 192KHz
    \param[out] none
    \retval     none
*/
void spi_i2s_config(uint16_t standard, uint16_t mclk_output, uint16_t audio_freq)
{
    spi_parameter_struct spi_init_struct;

    /* enable SPI clock */
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);

    {
        /* deinitialize SPI1 peripheral */
        spi_i2s_deinit(SPI1);
        /* I2S1 peripheral configuration */
        i2s_init(SPI1, I2S_MODE_MASTERTX, standard, I2S_CKPL_HIGH);
        i2s_psc_config(SPI1, audio_freq, I2S_FRAMEFORMAT_DT16B_CH16B, mclk_output);

        /* disable the I2S1 TBE interrupt */ 
        spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TBE);
        /* enable the SPI1 peripheral */
        i2s_enable(SPI1);
    }
    
    /* pcm1770 mode control via SPI0 interface */ 
    {
        /* deinitialize SPI0 peripheral */
        spi_i2s_deinit(SPI0);
        /* SPI0 parameter config */
        spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
        spi_init_struct.device_mode          = SPI_MASTER;
        spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
        spi_init_struct.nss                  = SPI_NSS_SOFT;
        spi_init_struct.prescale             = SPI_PSC_32;
        spi_init_struct.endian               = SPI_ENDIAN_MSB;
        spi_init(SPI0, &spi_init_struct);

        /* enable the SPI0 peripheral */
        spi_enable(SPI0);
    }
}
