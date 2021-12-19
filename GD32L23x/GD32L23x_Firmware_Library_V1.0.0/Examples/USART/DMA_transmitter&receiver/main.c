/*!
    \file    main.c
    \brief   USART DMA transmitter receiver

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32l23x.h"
#include <stdio.h>

uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";
#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART1_TDATA_ADDRESS      (&USART_TDATA(USART1))
#define USART1_RDATA_ADDRESS      (&USART_RDATA(USART1))

void com_usart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* initialize the com */
    com_usart_init();

    printf("\n\ra usart dma function test example!\n\r");

    /* initialize DMA channel 0 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART1_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)txbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr  = (uint32_t)USART1_TDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART1, USART_DENT_ENABLE);
    /* wait DMA Channel transfer complete */
    while(RESET == dma_flag_get(DMA_CH0, DMA_FLAG_FTF)) {
    }
    while(1) {
        /* initialize DMA channel 1 */
        dma_deinit(DMA_CH1);
        dma_struct_para_init(&dma_init_struct);
        dma_init_struct.request      = DMA_REQUEST_USART1_RX;
        dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr  = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number       = ARRAYNUM(rxbuffer);
        dma_init_struct.periph_addr  = (uint32_t)USART1_RDATA_ADDRESS;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(DMA_CH1, &dma_init_struct);

        /* configure DMA mode */
        dma_circulation_disable(DMA_CH1);
        dma_memory_to_memory_disable(DMA_CH1);
        /* disable the DMAMUX_MUXCH1 synchronization mode */
        dmamux_synchronization_disable(DMAMUX_MUXCH1);

        /* enable DMA channel 1 */
        dma_channel_enable(DMA_CH1);
        /* USART DMA enable for reception */
        usart_dma_receive_config(USART1, USART_DENR_ENABLE);
        /* wait DMA channel 1 transfer complete */
        while(RESET == dma_flag_get(DMA_CH1, DMA_FLAG_FTF));
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/*!
    \brief      initialize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USART TX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);
    /* connect port to USART RX */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* USART configure */
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    usart_enable(USART1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}
