/*!
    \file    main.c
    \brief   ADC analog watchdog 1

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include "systick.h"
#include <stdio.h>
#include "gd32e507z_eval.h"

#define BOARD_ADC_CHANNEL       ADC_CHANNEL_10
#define ADC_GPIO_PORT_RCU       RCU_GPIOC
#define ADC_GPIO_PORT           GPIOC
#define ADC_GPIO_PIN            GPIO_PIN_0
#define ADC_AWD1_2_CHANNEL      ADC_AWD1_2_SELECTION_CHANNEL_10

#define ADC_WATCHDOG1_HT        0xA0
#define ADC_WATCHDOG1_LT        0x40

uint16_t adc_value;
uint16_t data;

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

    while(1){
        delay_1ms(1000);
        adc_value = adc_regular_data_read(ADC1);

        if(1 == data){
            /* turn off LED2 */
            if((adc_value < ((uint16_t)(ADC_WATCHDOG1_HT << 4))) && (adc_value > ((uint16_t)(ADC_WATCHDOG1_LT << 4)))){
                data = 0;
                gd_eval_led_off(LED2);
            }
        }
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
    rcu_periph_clock_enable(ADC_GPIO_PORT_RCU);
    /* enable ADC1 clock */
    rcu_periph_clock_enable(RCU_ADC1);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure led GPIO */
    gd_eval_led_init(LED2);
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
    /* ADC continuous function enable */
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC1, ADC_SCAN_MODE, DISABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC channel length config */
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC1, 0, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);
    adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC analog watchdog threshold config */
    adc_watchdog1_threshold_config(ADC1, ADC_WATCHDOG1_LT, ADC_WATCHDOG1_HT);
    /* ADC analog watchdog single channel config */
    adc_watchdog1_channel_config(ADC1, ADC_AWD1_2_CHANNEL, ENABLE);
    /* ADC interrupt config */
    adc_interrupt_enable(ADC1, ADC_INT_WDE1);

    /* enable ADC interface */
    adc_enable(ADC1);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);
}
