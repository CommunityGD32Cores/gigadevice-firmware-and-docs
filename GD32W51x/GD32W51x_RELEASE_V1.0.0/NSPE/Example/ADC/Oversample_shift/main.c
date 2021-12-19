/*!
    \file    main.c
    \brief   ADC oversample shift example demo

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

#define BOARD_ADC_CHANNEL               ADC_CHANNEL_0
#define ADC_GPIO_PORT                   GPIOA
#define ADC_GPIO_PIN                    GPIO_PIN_0

uint16_t adc_value = 0U;

void rcu_config(void);
void gpio_config(void);
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
    /* ADC configuration */
    adc_config();
    /* configures COM port */
    gd_eval_com_init(EVAL_COM0);
    
    /* enable ADC software trigger for regular channel */
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

    while(1){
        adc_flag_clear(ADC_FLAG_EOC);
        while(SET != adc_flag_get(ADC_FLAG_EOC)){
        }
        
        adc_value = ADC_RDATA;
        printf("\r\n*ADC oversample shift example demo*\r\n");
        printf("16 times sample, 4 bits shift: 0x%X\r\n", adc_value);
        delay_1ms(1000U);
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
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
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
    /* config the GPIO as analog mode */
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC contineous function disable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);
    /* ADC inserted channel config */
    adc_regular_channel_config(0U, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_56);

    /* 16 times sample, 4 bits shift */
    adc_oversample_mode_config(ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, ADC_OVERSAMPLING_RATIO_MUL16);
    adc_oversample_mode_enable();
    /* enable ADC interface */
    adc_enable();
    /* wait for ADC stability */
    delay_1ms(1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
