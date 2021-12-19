/*!
    \file    main.c
    \brief   EXTI trigger ADC regular and inserted channel demo

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

uint16_t adc_value[4];
uint16_t inserted_data[4];

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);
void dma_config(void);
void adc_config(void);
void exti_config(void);

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
    /* GPIO configuration */
    gpio_config();
    /* configure the EXTI peripheral */
    exti_config();
    /* systick configuration */
    systick_config();
    /* NVIC configuration */
    nvic_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM0);
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
    
    while(1){
        delay_1ms(1000);
        printf("\r\n ADC regular channel 0 data = %d \r\n", adc_value[0]);
        printf(" ADC regular channel 1 data = %d \r\n", adc_value[1]);
        printf(" ADC regular channel 2 data = %d \r\n", adc_value[2]);
        printf(" ADC regular channel 3 data = %d \r\n", adc_value[3]);
        printf("\r\n ADC inserted channel 0 data = %d \r\n", inserted_data[0]);
        printf(" ADC inserted channel 1 data = %d \r\n", inserted_data[1]);
        printf(" ADC inserted channel 2 data = %d \r\n", inserted_data[2]);
        printf(" ADC inserted channel 3 data = %d \r\n", inserted_data[3]);
        printf(" ***********************************\r\n");
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
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* config ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    adc_clock_config(ADC_ADCCK_HCLK_DIV6);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode, for ADC */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    /* config the GPIO as floating input mode, for EXTI */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_11|GPIO_PIN_15);
}

/**
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
  */
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(ADC_IRQn, 1, 1);
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
    dma_single_data_parameter.memory0_addr        = (uint32_t)(&adc_value);
    dma_single_data_parameter.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction           = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number              = 4;
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
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    /* ADC inserted channel length config */
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 4U);
    /* ADC inserted channel config */
    adc_inserted_channel_config(0, ADC_CHANNEL_0, ADC_SAMPLETIME_15);
    adc_inserted_channel_config(1, ADC_CHANNEL_1, ADC_SAMPLETIME_15);
    adc_inserted_channel_config(2, ADC_CHANNEL_2, ADC_SAMPLETIME_15);
    adc_inserted_channel_config(3, ADC_CHANNEL_3, ADC_SAMPLETIME_15);
    /* ADC inserted channel trigger config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_EXTI_15);
    /* ADC inserted channel external trigger enable */
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_RISING);
    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOIC);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC_INT_EOIC);
    
    /* ADC regular channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 4U);
    /* ADC regular channel config */
    adc_regular_channel_config(0, ADC_CHANNEL_4, ADC_SAMPLETIME_15);
    adc_regular_channel_config(1, ADC_CHANNEL_5, ADC_SAMPLETIME_15);
    adc_regular_channel_config(2, ADC_CHANNEL_6, ADC_SAMPLETIME_15);
    adc_regular_channel_config(3, ADC_CHANNEL_7, ADC_SAMPLETIME_15);
    /* ADC regular channel trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_EXTI_11);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_RISING);
    /* ADC regular channel discontinuous mode */
    adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 1);
    /* ADC DMA function enable */
    adc_dma_request_after_last_enable();
    adc_dma_mode_enable();

    /* enable ADC interface */
    adc_enable();
    /* wait for ADC stability */
    delay_1ms(1);
}

/*!
    \brief      configure the EXTI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exti_config(void)
{
    /* connect EXTI line to GPIO pin for inserted group */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN15);
    /* configure EXTI line for inserted group */
    exti_init(EXTI_15, EXTI_EVENT, EXTI_TRIG_RISING);
    exti_flag_clear(EXTI_15);
    
    /* connect EXTI line to GPIO pin for inserted group */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN11);
    /* configure EXTI line for inserted group */
    exti_init(EXTI_11, EXTI_EVENT, EXTI_TRIG_RISING);
    exti_flag_clear(EXTI_11);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
