/*!
    \file    main.c
    \brief   DMA transfer data with switch buffer mode

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#include "gd32w51x.h"
#include <stdio.h>
#include "gd32w515p_eval.h"

#define GPIOB_OCTL           0x40020414

static void led_config(void);
static void dma_config(void);
static void timer_config(void);

uint8_t buffer1 = 0xFF;
uint8_t buffer2 = 0x00;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    led_config();
    dma_config();
    timer_config();

    while(1);
}

/*!
    \brief      configure the LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void led_config()
{
    /* LED initialize */
    gd_eval_led_init(LED1);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_parameter;
    dma_single_data_para_struct_init(&dma_init_parameter);

    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_DMA0);

    /* DMA peripheral configure */
    dma_deinit(DMA0,DMA_CH7);

    dma_init_parameter.periph_addr = (uint32_t)(GPIOB_OCTL);
    dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)(&buffer1);
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_DISABLE;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number = 1;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH7, &dma_init_parameter);

    dma_channel_subperipheral_select(DMA0, DMA_CH7, DMA_SUBPERI3);

    dma_switch_buffer_mode_config(DMA0, DMA_CH7, (uint32_t)(&buffer2), DMA_MEMORY_1);
    dma_switch_buffer_mode_enable(DMA0, DMA_CH7, ENABLE);

    dma_channel_enable(DMA0, DMA_CH7);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void timer_config(void)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

    timer_deinit(TIMER1);
    timer_struct_para_init(&timer_initpara);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 1999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 49999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* TIMER0 update DMA request enable */
    timer_dma_enable(TIMER1,TIMER_DMA_UPD);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}
