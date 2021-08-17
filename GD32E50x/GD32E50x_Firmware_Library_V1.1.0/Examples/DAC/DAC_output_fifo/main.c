/*!
    \file    main.c
    \brief   DAC output fifo demo

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include "systick.h"
#include "gd32e507z_eval.h"

void rcu_config(void);
void gpio_config(void);
void nvic_config(void);
void adc_config(void);
void dac_config(void);
void timer6_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_config();
    systick_config();
    gpio_config();
    nvic_config();
    adc_config();
    dac_config();
    timer6_config();

    while (1){
    }
}

/*!
    \brief      configure the RCU of peripherals
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the clock of peripherals */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
    rcu_periph_clock_enable(RCU_DAC);
}

/*!
    \brief      configure the related GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3|GPIO_PIN_4);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(ADC0_1_IRQn, 0U, 0U);
    nvic_irq_enable(TIMER5_DAC_IRQn, 0U, 1U);
}

/*!
    \brief      configure the ADC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC continous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_55POINT5);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* ADC interrupt enable */
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
    adc_interrupt_enable(ADC0, ADC_INT_EOC);

    /* enable ADC software trigger */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

/*!
    \brief      configure the DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    dac_deinit();
    /* configure the DAC_OUT0 */
    dac_trigger_source_config(DAC_OUT_0, DAC_TRIGGER_T6_TRGO);
    dac_trigger_enable(DAC_OUT_0);
    dac_wave_mode_config(DAC_OUT_0, DAC_WAVE_DISABLE);

    /* enable DAC output FIFO */
    dac_output_fifo_enable(DAC_OUT_0);

    dac_interrupt_flag_clear(DAC_OUT_0, DAC_INT_FLAG_FIFOOVR0 | DAC_INT_FLAG_FIFOUDR0);
    dac_interrupt_enable(DAC_OUT_0, DAC_INT_FIFOOVRIE0);
    dac_interrupt_enable(DAC_OUT_0, DAC_INT_FIFOUDRIE0);

    /* enable DAC_OUT0 */
    dac_enable(DAC_OUT_0);
}

/*!
    \brief      configure the TIMER6
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer6_config(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER6);

    timer_deinit(TIMER6);

    /* TIMER configuration */
    timer_initpara.prescaler         = 1799;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER6, &timer_initpara);
    
    /* TIMER master mode output trigger source: Update event */
    timer_master_output_trigger_source_select(TIMER6, TIMER_TRI_OUT_SRC_UPDATE);
    
    /* enable TIMER */
    timer_enable(TIMER6);
}
