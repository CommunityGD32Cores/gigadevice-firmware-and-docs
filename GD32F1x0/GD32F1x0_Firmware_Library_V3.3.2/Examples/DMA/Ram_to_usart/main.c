/*!
    \file    main.c
    \brief   transfer data from RAM to USART

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h" 
#include "gd32f1x0r_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))

uint8_t welcome[]="hi,this is a example: RAM_TO_USART by DMA !\n";
__IO uint32_t transfer;

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

    /* initialize LED */
    gd_eval_led_init (LED1);

    /*configure DMA interrupt*/
    nvic_config();

#ifdef GD32F130_150

    /* USART configure */
    gd_eval_com_init(EVAL_COM0);

    /* initialize DMA channel1 */
    dma_deinit(DMA_CH1);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)welcome;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(welcome);
    dma_init_struct.periph_addr = (uint32_t)(&USART_TDATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_disable(DMA_CH1);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART0,USART_DENT_ENABLE);

    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(DMA_CH1,DMA_CHXCTL_FTFIE);

    /* enable DMA channel1 */
    dma_channel_enable(DMA_CH1);

#elif defined GD32F170_190

    /* USART configure */
    gd_eval_com_init(EVAL_COM2);

    /* initialize DMA channel3 */
    dma_deinit(DMA_CH3);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)welcome;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(welcome);
    dma_init_struct.periph_addr = (uint32_t)(&USART_TDATA(USART1));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH3, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH3);
    dma_memory_to_memory_disable(DMA_CH3);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART1,USART_DENT_ENABLE);

    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(DMA_CH3,DMA_INT_FTF);

    /* enable DMA channel3 */
    dma_channel_enable(DMA_CH3);

#endif /* GD32F130_150 */

    /* waiting for the transfer to complete*/
    while(0 == transfer);

    /* light LED1 */
    gd_eval_led_on(LED1);

    while (1);
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
#ifdef GD32F130_150
    nvic_irq_enable(DMA_Channel1_2_IRQn,0,0);
#elif defined GD32F170_190
    nvic_irq_enable(DMA_Channel3_4_IRQn,0,0);
#endif
}
