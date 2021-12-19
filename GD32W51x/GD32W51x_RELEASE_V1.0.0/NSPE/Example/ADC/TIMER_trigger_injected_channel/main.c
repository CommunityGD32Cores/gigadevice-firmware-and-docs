/*!
    \file    main.c
    \brief   TIMER trigger injected channel of ADC demo

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

void rcu_config(void);
void nvic_config(void);
void gpio_config(void);
void timer_config(void);
void adc_config(void);

uint16_t inserted_data[4];

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
    /* configure COM port */
    gd_eval_com_init(EVAL_COM0);
    /* TIMER configuration */
    timer_config();
    /* ADC configuration */
    adc_config();
    /* enable TIMER2 */
    timer_enable(TIMER2);
  
    while(1){
        delay_1ms(1000);
        printf("\r\nADC inserted channel 0 data = %d \r\n", inserted_data[0]);
        printf("ADC inserted channel 1 data = %d \r\n", inserted_data[1]);
        printf("ADC inserted channel 2 data = %d \r\n", inserted_data[2]);
        printf("ADC inserted channel 3 data = %d \r\n", inserted_data[3]);
        printf("***********************************\r\n");
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
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER2);
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
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 1799U;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999U;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0U;
    timer_init(TIMER2, &timer_initpara);

    /* CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER2, TIMER_CH_3, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_3, 3999U);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_3, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
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

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_RISING);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_T2_CH3);
    
    /* ADC channel length config */
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 4U);

    /* ADC inserted channel config */
    adc_inserted_channel_config(0U, ADC_CHANNEL_0, ADC_SAMPLETIME_56);
    adc_inserted_channel_config(1U, ADC_CHANNEL_1, ADC_SAMPLETIME_56);
    adc_inserted_channel_config(2U, ADC_CHANNEL_2, ADC_SAMPLETIME_56);
    adc_inserted_channel_config(3U, ADC_CHANNEL_3, ADC_SAMPLETIME_56);

    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOIC);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC_INT_EOIC);
    
    /* enable ADC interface */
    adc_enable();
    /* wait for ADC stability */
    delay_1ms(1);
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
    nvic_irq_enable(ADC_IRQn, 0, 0);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
