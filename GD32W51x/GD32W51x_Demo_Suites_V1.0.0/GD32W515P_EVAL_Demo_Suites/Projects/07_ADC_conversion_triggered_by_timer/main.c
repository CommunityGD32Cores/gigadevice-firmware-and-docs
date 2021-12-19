/*!
    \file    main.c
    \brief   ADC conversion triggered by TIMER demo

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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

uint16_t adc_value;

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void timer_config(void);
void adc_config(void);
void flash_led(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init test status led */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* SYSTICK configuration */
    systick_config();
    /* flash led for test */
    flash_led(3);
    /* RCU configuration */
    rcu_config();
    /* GPIO configuration */
    gpio_config();
    /* DMA configuration */
    dma_config();
    /* TIMER configuration */
    timer_config();
    /* ADC configuration */
    adc_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM0);
    
    /* infinite loop */
    while(1){
        delay_1ms(1000);
        printf("\r\n//*********************************//\r\n");
        printf("the ADC conversion result is 0x%04X \n", adc_value);
    }
}

/*!
    \brief      RCU configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    adc_clock_config(ADC_ADCCK_HCLK_DIV6);
}

/*!
    \brief      GPIO configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PA0(ADC channel0) as analog input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
}

/*!
    \brief      DMA configuration function
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
    dma_single_data_parameter.memory_inc          = DMA_MEMORY_INCREASE_DISABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction           = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number              = 1U;
    dma_single_data_parameter.priority            = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
    \brief      TIMER configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 5;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 399;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 100);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);
    timer_enable(TIMER0);
}

/*!
    \brief      ADC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);

    /* ADC regular channel config */
    adc_regular_channel_config(0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T0_CH0);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    /* enable ADC interface */
    adc_enable();
    /* wait for ADC stability */
    delay_1ms(1);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable();
    adc_dma_mode_enable();
    /* enable ADC software trigger */
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

/*!
    \brief      test status led
    \param[in]  times: the flash times of leds
    \param[out] none
    \retval     none
*/
void flash_led(int times)
{
    int i;
    
    for(i = 0;i < times;i ++){
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on led */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off led */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
