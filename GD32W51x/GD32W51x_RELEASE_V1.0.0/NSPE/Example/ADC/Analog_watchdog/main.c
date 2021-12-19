/*!
    \file    main.c
    \brief   ADC analog watchdog demo

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

#define BOARD_ADC_CHANNEL              ADC_CHANNEL_0
#define ADC_GPIO_PORT                  GPIOA
#define ADC_GPIO_PIN                   GPIO_PIN_0

#define ADC_WATCHDOG_HT                0x0A00U
#define ADC_WATCHDOG_LT                0x0400U

uint8_t data;
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
   /* configure COM port */
    gd_eval_com_init(EVAL_COM0);
    /* NVIC configuration */
    nvic_config();
    /* ADC configuration */
    adc_config();

    while(1){
        delay_1ms(1000);
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        while(SET != adc_flag_get(ADC_FLAG_EOC)){
        }
        adc_flag_clear(ADC_FLAG_EOC);
        adc_value = adc_regular_data_read();
        
        printf("\r\n //*********************************//");
        printf("\r\n ADC watchdog low threshold: 0x%04X", ADC_WATCHDOG_LT);
        printf("\r\n ADC watchdog hgih threshold: 0x%04X", ADC_WATCHDOG_HT);
        printf("\r\n ADC regular channel data = 0x%04X \r\n", adc_value);

        if(1U == data){
            /* turn off LED2 */
            if((adc_value < ADC_WATCHDOG_HT) && (adc_value > ADC_WATCHDOG_LT)){
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
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_HCLK_DIV5);
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
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);
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
    nvic_irq_enable(ADC_IRQn, 0U, 0U);
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
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);
 
    /* ADC regular channel config */
    adc_regular_channel_config(0U, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_56);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    
    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);

    /* ADC analog watchdog threshold config */
    adc_watchdog_threshold_config(ADC_WATCHDOG_LT, ADC_WATCHDOG_HT);
    /* ADC analog watchdog single channel config */
    adc_watchdog_single_channel_enable(BOARD_ADC_CHANNEL);
    /* ADC interrupt config */
    adc_interrupt_enable(ADC_INT_WDE);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
