/*!
    \file    main.c
    \brief   DMA reload transfer number by EXTI interrupt in discontinuous mode demo

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
#include "main.h"

#define USART1_DATA_ADDRESS       (uint32_t)&USART_TDATA(USART1)

uint8_t welcome[50] = "Hi, this is an example: RAM_TO_USART by DMA !\n";
__IO FlagStatus g_transfer_complete = RESET;

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
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    /* USART DMA enable for transmission */
    usart_dma_transmit_config(EVAL_COM, USART_DENT_ENABLE);
    /* configure DMA interrupt */
    nvic_config();
    /* initialize LED1 */
    led_config();
    /* turn off LED1 */
    gd_eval_led_off(LED1);
    /* configure WAKEUP key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);

    /* configure DMA channel 0 */
    dma_config();

    while(1) {
    }
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
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA_Channel0_IRQn, 0U);
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

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
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);

    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
}
