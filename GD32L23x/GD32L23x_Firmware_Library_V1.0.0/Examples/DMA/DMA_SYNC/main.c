/*!
    \file    main.c
    \brief   DMA request synchronization example

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
#include "gd32l233r_eval.h"
#include <string.h>

#define USART1_DATA_ADDRESS       (uint32_t)&USART_TDATA(USART1)
#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))

uint8_t welcome[] = "Hi, this is an example: DMA_SYNC !\n";
__IO uint32_t g_transfer_complete = 0;

void rcu_config(void);
void led_config(void);
void nvic_config(void);
void dma_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    usart_dma_transmit_config(EVAL_COM, USART_DENT_ENABLE);
    /* key for DMA request synchronization configuration */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    /* LED1 configuration */
    led_config();
    /* interrupt configuration */
    nvic_config();
    /* configure DMA and DMAMUX channel */
    dma_config();

    /* wait for DMA transfer complete */
    while(1U != g_transfer_complete) {
    }

    /* success */
    gd_eval_led_on(LED1);

    while(1) {
    }
}

/*!
    \brief      system clocks configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
}

/*!
    \brief      interrupt configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA_Channel0_IRQn, 2);
    nvic_irq_enable(DMAMUX_IRQn, 2);
}

/*!
    \brief      LED1 configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
}

/*!
    \brief      configure DMA and DMAMUX channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    dmamux_sync_parameter_struct dmamux_sync_init_struct ;

    /* DMA channel configuration */
    /* initialize DMA channel 0 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART1_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)welcome;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(welcome);
    dma_init_struct.periph_addr  = USART1_DATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH0, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);

    /* DMAMUX channel configuration */
    /* initialize DMA request multiplexer channel 0 with synchronization mode */
    dmamux_sync_init_struct.sync_id        = DMAMUX_SYNC_EXTI0;
    dmamux_sync_init_struct.sync_polarity  = DMAMUX_SYNC_RISING;
    dmamux_sync_init_struct.request_number = 4;
    dmamux_synchronization_init(DMAMUX_MUXCH0, &dmamux_sync_init_struct);
    dmamux_synchronization_enable(DMAMUX_MUXCH0);

    /* enable DMAMUX interrupt */
    dmamux_interrupt_enable(DMAMUX_INT_MUXCH0_SO);

    /* enable DMA interrupt */
    dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
}
