/*!
    \file    main.c
    \brief   ADC1 resolution
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "gd32c10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32c10x_eval.h"


#define BOARD_ADC_CHANNEL ADC_CHANNEL_13
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN  GPIO_PIN_3

uint16_t adc_value;

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);
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
    /* NVIC configuration */
    nvic_config();
    /* ADC configuration */
    adc_config();
    /* configures COM port */
    gd_eval_com_init(EVAL_COM1); 
    /* enable ADC regular channel group software trigger */
    adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);
  
    while(1){
        /* wait end of group conversion */
        adc_flag_clear(ADC1, ADC_FLAG_EOC);
        while(SET != adc_flag_get(ADC1, ADC_FLAG_EOC)){
        }
        /* printf the data of regular channel */
        adc_value = ADC_RDATA(ADC1);        
        printf("6B: 0x%x\r\n", adc_value);
        delay_1ms(500);
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
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC1 clock */
    rcu_periph_clock_enable(RCU_ADC1);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
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
    gpio_init(ADC_GPIO_PORT, GPIO_MODE_AIN, GPIO_OSPEED_MAX, ADC_GPIO_PIN);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(ADC0_1_IRQn, 0, 0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC1);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC contineous function enable */
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    /* ADC resolusion 6B */
    adc_resolution_config(ADC1, ADC_RESOLUTION_6B); 
    
    /* ADC channel length config */
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1);
    /* ADC regular channel config */
    adc_regular_channel_config(ADC1, 0, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC1);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM1,USART_FLAG_TBE));
    return ch;
}
