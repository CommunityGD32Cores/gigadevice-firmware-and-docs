/*!
    \file    main.c
    \brief   I2S master use DMA

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32w51x.h"
#include "gd32w515p_eval.h"

#define arraysize  10

uint8_t i2s1_send_array[arraysize] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void spi_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* peripheral clock enable */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* DMA config */
    dma_config();
    /* SPI config */
    spi_config();

    /* SPI enable */
    i2s_enable(SPI1);
    /* DMA channel enable */
    dma_channel_enable(DMA0,DMA_CH4);
    /* SPI DMA enable */
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

    /* wait DMA transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH4, DMA_FLAG_FTF));

    while(1){
    }
}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* cofigure i2s clock */
    rcu_hxtal_plli2s_enable();
    rcu_plli2s_config(48, RCU_PLLI2SSRC_DIV8, RCU_PLLI2S_DIV2);
    rcu_osci_on(RCU_PLLI2S_CK);
    while(0 == (RCU_CTL & RCU_CTL_PLLI2SSTB));
    rcu_i2s_clock_config(RCU_I2SSRC_PLLI2S);

    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable SPI1/I2S1 clock */
    rcu_periph_clock_enable(RCU_SPI1);
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* I2S1 GPIO config: I2S1_WS/PB12, I2S1_CK/PB13, I2S_SD/PB15 */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    dma_single_data_para_struct_init(&dma_init_struct);
    
    /* SPI1 transmit dma config: DMA0,DMA_CH4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.periph_addr         = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.memory0_addr        = (uint32_t)i2s1_send_array;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.priority            = DMA_PRIORITY_LOW;
    dma_init_struct.number              = arraysize;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_mode_init(DMA0, DMA_CH4, &dma_init_struct);
    /* connect DMA0_CH4 to SPI1 TX */
    dma_channel_subperipheral_select(DMA0, DMA_CH4, DMA_SUBPERI0);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    /* resEt SPI1/I2S1 */
    spi_i2s_deinit(SPI1);
    /* configure I2S1 */
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_HIGH);
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_8K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
}
