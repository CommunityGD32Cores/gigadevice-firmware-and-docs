/*!
    \file    main.c
    \brief   TIMER1 dma burst demo

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

#define TIMER1_DMATB            ((uint32_t)0x04000004C)
uint16_t buffer[8] = {100, 200, 300, 400, 500, 600, 700, 800};

void gpio_config(void);
void timer_config(void);
void dma_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gpio_config();
    dma_config();
    timer_config();
    while(1);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /*configure PA0(TIMER1 CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_0);

    /*configure PA9(TIMER1 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_1);

    /*configure PB10(TIMER1 CH2) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_10);

    /*configure PB11(TIMER1 CH3) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_11);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* TIMER1 DMA transfer example -------------------------------------------------
    TIMER1_CLK = 64MHz, prescaler = 63
    TIMER1 counter clock = TIMER1_CLK/64 = 1MHz.

    The objective is to configure TIMER1 channel 0~3(PA0 PA1 PB10 PB11) to generate PWM signal.
    capture compare register 0~3 are to be updated twice per circle.On the first update
    DMA request, data1 is transferred to CH0CV, data2 is transferred to CH1CV, data3 is
    transferred to CH2CV, data4 is transferred to CH3CV and duty cycle(10%, 20%, 30%, 40%).
    On the second update DMA request, data5 is transferred to CH0CV, data6 is transferred
    to CH1CV, data7 is transferred to CH2CV, data8 is transferred to CH3CV and duty cycle
    (50%, 60%, 70%, 80%).
    -----------------------------------------------------------------------------*/
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler        = 63;
    timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period           = 999;
    timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate    = TIMER_CCX_ENABLE;
    timer_ocinitpara.ocpolarity     = TIMER_OC_POLARITY_HIGH;
    timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocinitpara);
    timer_channel_output_config(TIMER1, TIMER_CH_3, &timer_ocinitpara);

    /* CH0 configuration in PWM0 mode */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, buffer[0]);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* CH1 configuration in PWM0 mode */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, buffer[0]);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* CH2 configuration in PWM0 mode */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, buffer[0]);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* CH3 configuration in PWM0 mode */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_3, buffer[0]);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* TIMER1 update DMA request enable */
    timer_dma_transfer_config(TIMER1, TIMER_DMACFG_DMATA_CH0CV, TIMER_DMACFG_DMATC_4TRANSFER);
    timer_dma_enable(TIMER1, TIMER_DMA_UPD);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);

    /* enable a TIMER */
    timer_enable(TIMER1);
}

/*!
    \brief      configure different DMA channels
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    /* ADC DMA_channel configuration */
    dma_deinit(DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)TIMER1_DMATB;
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&buffer);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_data_parameter.number       = 8;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_data_parameter.request      = DMA_REQUEST_TIMER1_UP;
    dma_init(DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA_CH0);
}
