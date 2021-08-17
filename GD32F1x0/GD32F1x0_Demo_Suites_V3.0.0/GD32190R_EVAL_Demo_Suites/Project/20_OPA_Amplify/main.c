/*!
    \file  main.c
    \brief OPA amplify

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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
#include "gd32f1x0_libopt.h"
#include "gd32f190r_eval.h"
#include "systick.h"
#include <stdio.h>

__IO uint16_t data;
float value;

void rcu_config(void);
void gpio_config(void);
void opa_config(void);
void adc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* RCU configuration */
    rcu_config();
    gd_eval_com_init(EVAL_COM2);
    /* SYSTICK configuration */	
    systick_config();
    gpio_config();
    /* OPA configuration */
    opa_config();
    /* ADC configuration */
    adc_config();
  
    while (1){
        delay_1ms(500);
        /* get the converted result, and convert it into fractional format */
        data = adc_regular_data_read();
        value = ((float)data*5.0/4095.0);
        /* output ADC value on Hyperterminal using printf function */      
        printf("\n\rADC_Value %5.3f\n\r",value);
    }
}

/*!
    \brief      configure RCU function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* ADCCLK = APB2/8 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV8);
    /* OPAMP clock enable */
    rcu_periph_clock_enable(RCU_OPAIVREF);
    /* GPIOA clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* GPIOC clock enable */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* ADC clock enable */
    rcu_periph_clock_enable(RCU_ADC);
}

/*!
    \brief      Configure GPIO function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

/*!
    \brief      Configure OPA function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void opa_config(void)
{
    opa_deinit();
    /* enable OPA switch */
    opa_switch_enable(OPA_S1OPA2);
    opa_switch_enable(OPA_S2OPA2);
    /* enable OPA */
    opa_enable(OPA2);
}

/*!
    \brief      Configure ADC function
    \param[in]  none
    \param[out] none
    \retval     none
*/ 
void adc_config(void)
{
    /* configure ADC */
    adc_special_function_config(ADC_SCAN_MODE,ENABLE);
    adc_special_function_config(ADC_CONTINUOUS_MODE,ENABLE);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_NONE);
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC_REGULAR_CHANNEL,1);
    adc_regular_channel_config(0,ADC_CHANNEL_13,ADC_SAMPLETIME_41POINT5);
    /* enable ADC interface */
    adc_enable();
    adc_calibration_enable();
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM2, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM2,USART_FLAG_TC));
    return ch;
}
