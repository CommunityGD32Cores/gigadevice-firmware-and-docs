/*!
    \file    main.c
    \brief   TIMER trigger inserted channel of ADC

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

uint16_t inserted_data[4];

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);
void shrtimer_config(void);
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
    /* NVIC configuration */
    nvic_config();
    /* systick configuration */
    systick_config();  
    /* GPIO configuration */
    gpio_config();
    /* SHRTIMER configuration */
    shrtimer_config();
    /* ADC configuration */
    adc_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM0);

    while(1){
        delay_1ms(1000);

        printf("\r\n //*********************************//");
        printf("\r\n ADC0 inserted channel 0 data = %d \r\n", inserted_data[0]);
        printf("\r\n ADC0 inserted channel 1 data = %d \r\n", inserted_data[1]);
        printf("\r\n ADC0 inserted channel 2 data = %d \r\n", inserted_data[2]);
        printf("\r\n ADC0 inserted channel 3 data = %d \r\n", inserted_data[3]);
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
    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
    /* config SHRTIMER clock */
    rcu_periph_clock_enable(RCU_SHRTIMER);
    rcu_shrtimer_clock_config(RCU_SHRTIMERSRC_CKSYS);
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
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


/*!
    \brief      configure the shrtimer peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void shrtimer_config(void)
{
    shrtimer_baseinit_parameter_struct baseinit_para;
    shrtimer_timerinit_parameter_struct timerinit_para;
    shrtimer_timercfg_parameter_struct timercfg_para;
    shrtimer_adctrigcfg_parameter_struct adctrigcfg_para;

    /* periodic DLL calibration */
    shrtimer_dll_calibration_start(SHRTIMER0, SHRTIMER_CALIBRATION_16384_PERIOD);
    while(RESET == shrtimer_common_flag_get(SHRTIMER0, SHRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    shrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period            = 179;
    baseinit_para.prescaler         = SHRTIMER_PRESCALER_DIV1;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode      = SHRTIMER_COUNTER_MODE_CONTINOUS;
    shrtimer_timers_base_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &baseinit_para);

    /* initialize Slave_TIMER0 to work in waveform mode */
    shrtimer_timerinit_struct_para_init(&timerinit_para);
    timerinit_para.cnt_bunch         = SHRTIMER_TIMERBUNCHNMODE_MAINTAINCLOCK;
    timerinit_para.dac_trigger       = SHRTIMER_DAC_TRIGGER_NONE;
    timerinit_para.half_mode         = SHRTIMER_HALFMODE_DISABLED;
    timerinit_para.repetition_update = SHRTIMER_UPDATEONREPETITION_DISABLED;
    timerinit_para.reset_sync        = SHRTIMER_SYNCRESET_DISABLED;
    timerinit_para.shadow            = SHRTIMER_SHADOW_DISABLED;
    timerinit_para.start_sync        = SHRTIMER_SYNISTART_DISABLED;
    timerinit_para.update_selection  = SHRTIMER_MT_ST_UPDATE_SELECTION_INDEPENDENT;
    shrtimer_timers_waveform_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &timerinit_para);

    /* configure the general behavior of a Slave_TIMER0 which work in waveform mode */
    shrtimer_timercfg_struct_para_init(&timercfg_para);
    timercfg_para.balanced_mode   = SHRTIMER_STXBALANCEDMODE_DISABLED;
    timercfg_para.cnt_reset       = SHRTIMER_STXCNT_RESET_NONE;
    timercfg_para.deadtime_enable = SHRTIMER_STXDEADTIME_DISABLED;
    timercfg_para.delayed_idle    = SHRTIMER_STXDELAYED_IDLE_DISABLED;
    timercfg_para.fault_enable    = SHRTIMER_STXFAULTENABLE_NONE;
    timercfg_para.fault_protect   = SHRTIMER_STXFAULT_PROTECT_READWRITE;
    timercfg_para.reset_update    = SHRTIMER_STXUPDATEONRESET_DISABLED;
    timercfg_para.update_source   = SHRTIMER_STXUPDATETRIGGER_NONE;
    shrtimer_slavetimer_waveform_config(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &timercfg_para);

    /* configure the trigger source to ADC and the update source */
    shrtimer_adctrigcfg_struct_para_init(&adctrigcfg_para);
    adctrigcfg_para.update_source = SHRTIMER_ADCTRGI_UPDATE_ST0;
    adctrigcfg_para.trigger       = SHRTIMER_ADCTRGI13_EVENT_ST0PER;
    shrtimer_adc_trigger_config(SHRTIMER0, SHRTIMER_ADCTRIG_1, &adctrigcfg_para);

    /* enable a counter */
    shrtimer_timers_counter_enable(SHRTIMER0, SHRTIMER_ST0_COUNTER);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC special function config */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 4);
    /* ADC inserted channel config */
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
    adc_inserted_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_SHRTIMER_ADCTRG1);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);

    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC0, ADC_INT_EOIC);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
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
    nvic_irq_enable(ADC0_1_IRQn, 0, 0);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
