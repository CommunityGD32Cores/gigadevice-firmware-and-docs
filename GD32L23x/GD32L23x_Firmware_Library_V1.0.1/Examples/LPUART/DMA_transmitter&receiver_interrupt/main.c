/*!
    \file    main.c
    \brief   transmit/receive data using DMA interrupt

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
#include "gd32l233r_eval.h"
#include "systick.h"

#define LPUART_RDATA_ADDRESS      (&LPUART_RDATA)
#define LPUART_TDATA_ADDRESS      (&LPUART_TDATA)
#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))

__IO FlagStatus g_transfer_complete = RESET;
uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rLPUART DMA interrupt receive and transmit example, please input 10 bytes:\n\r";

void com_lpuart_init(void);
void nvic_config(void);

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
    com_lpuart_init();
    /* configure DMA interrupt */
    nvic_config();

    /* initialize DMA channel 0 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_LPUART_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)txbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr  = (uint32_t)LPUART_TDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);
    /* LPUART DMA enable for transmission and reception */
    lpuart_dma_transmit_config(LPUART_DENT_ENABLE);
    /* enable DMA channel 0 transfer complete interrupt */
    dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);

    /* initialize DMA channel 1 */
    dma_deinit(DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_LPUART_RX;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)rxbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(rxbuffer);
    dma_init_struct.periph_addr  = (uint32_t)LPUART_RDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_disable(DMA_CH1);
    /* disable the DMAMUX_MUXCH1 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH1);
    /* LPUART DMA enable for reception */
    lpuart_dma_receive_config(LPUART_DENR_ENABLE);
    /* enable DMA channel 1 transfer complete interrupt */
    dma_interrupt_enable(DMA_CH1, DMA_INT_FTF);
    /* enable DMA channel 1 */
    dma_channel_enable(DMA_CH1);

    /* waiting for the transfer to complete*/
    while(RESET == g_transfer_complete) {
    }

    g_transfer_complete = RESET;

    /* waiting for the transfer to complete*/
    while(RESET == g_transfer_complete) {
    }

    printf("\n\r%s\n\r", rxbuffer);

    while(1) {
    }
}

/*!
    \brief      configure LPUART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_lpuart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable LPUART clock */
    rcu_periph_clock_enable(RCU_LPUART);

    /* connect port to LPUART TX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_2);
    /* connect port to LPUART RX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_3);

    /* configure LPUART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure LPUART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* LPUART configure */
    lpuart_deinit();
    lpuart_word_length_set(LPUART_WL_8BIT);
    lpuart_stop_bit_set(LPUART_STB_1BIT);
    lpuart_parity_config(LPUART_PM_NONE);
    lpuart_baudrate_set(115200U);
    lpuart_receive_config(LPUART_RECEIVE_ENABLE);
    lpuart_transmit_config(LPUART_TRANSMIT_ENABLE);

    lpuart_enable();
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA_Channel0_IRQn, 1);
    nvic_irq_enable(DMA_Channel1_IRQn, 0);
}

/* retarget the C library printf function to the LPUART */
int fputc(int ch, FILE *f)
{
    lpuart_data_transmit((uint8_t)ch);
    while(RESET == lpuart_flag_get(LPUART_FLAG_TBE));
    return ch;
}
