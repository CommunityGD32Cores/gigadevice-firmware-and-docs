/*!
    \file    main.c
    \brief   I2S master transmit using DMA

    \version 2023-12-31, V3.5.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#include "gd32f1x0.h"
#include "gd32f1x0r_eval.h"

#define ARRAYSIZE  10

uint8_t i2s0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void i2s_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable peripheral clock */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure DMA */
    dma_config();
    /* configure I2S */
    i2s_config();

    /* enable DMA channel */
    dma_channel_enable(DMA_CH2);
    /* enable SPI DMA */
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    /* enable SPI */
    i2s_enable(SPI0);

    /* wait DMA transmit completed */
    while(!dma_flag_get(DMA_CH2, DMA_FLAG_FTF)) {
    }

    while(1) {
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
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable I2S0 clock */
    rcu_periph_clock_enable(RCU_SPI0);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure I2S0 GPIO: I2S0_WS/PA4, I2S0_CK/PA5, I2S0_SD/PA7 */
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct  dma_init_struct;
    dma_struct_para_init(&dma_init_struct);

    /* configure I2S0 transmit DMA: DMA_CH2 */
    dma_deinit(DMA_CH2);
    dma_init_struct.periph_addr         = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr         = (uint32_t)i2s0_send_array;
    dma_init_struct.direction           = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.periph_width        = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.memory_width        = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.priority            = DMA_PRIORITY_LOW;
    dma_init_struct.number              = ARRAYSIZE;
    dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA_CH2, &dma_init_struct);
}

/*!
    \brief      configure the I2S peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2s_config(void)
{
    /* reset I2S0 */
    spi_i2s_deinit(SPI0);
    /* configure I2S0 */
    i2s_init(SPI0, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_HIGH);
    i2s_psc_config(SPI0, I2S_AUDIOSAMPLE_8K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
}
