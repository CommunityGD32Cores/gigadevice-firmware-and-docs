/*!
    \file    main.c
    \brief   ADC discontinuous mode for regular channel demo

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
#include "systick.h"
#include <stdio.h>
#include "gd32w515p_eval.h"

uint16_t adc_value[16];

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);

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
    /* systick configuration */
    systick_config();
    /* GPIO configuration */
    gpio_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();

    while(1){
        /* delay 1s */
        delay_1ms(1000U);
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    adc_clock_config(ADC_ADCCK_HCLK_DIV6);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */ 
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);
    
    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr         = (uint32_t)(&ADC_RDATA);
    dma_single_data_parameter.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr        = (uint32_t)(adc_value);
    dma_single_data_parameter.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction           = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number              = 16U;
    dma_single_data_parameter.priority            = DMA_PRIORITY_HIGH;
    
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);
    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 8U);
    /* ADC discontinuous mode */
    adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 3U);

    /* ADC regular channel config */
    adc_regular_channel_config(0U, ADC_CHANNEL_0, ADC_SAMPLETIME_56);
    adc_regular_channel_config(1U, ADC_CHANNEL_1, ADC_SAMPLETIME_56);
    adc_regular_channel_config(2U, ADC_CHANNEL_2, ADC_SAMPLETIME_56);
    adc_regular_channel_config(3U, ADC_CHANNEL_3, ADC_SAMPLETIME_56);
    adc_regular_channel_config(4U, ADC_CHANNEL_4, ADC_SAMPLETIME_56);
    adc_regular_channel_config(5U, ADC_CHANNEL_5, ADC_SAMPLETIME_56);
    adc_regular_channel_config(6U, ADC_CHANNEL_6, ADC_SAMPLETIME_56);
    adc_regular_channel_config(7U, ADC_CHANNEL_7, ADC_SAMPLETIME_56);

    /* ADC external trigger config */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);

    /* ADC DMA function enable */
    adc_dma_mode_enable();
}
