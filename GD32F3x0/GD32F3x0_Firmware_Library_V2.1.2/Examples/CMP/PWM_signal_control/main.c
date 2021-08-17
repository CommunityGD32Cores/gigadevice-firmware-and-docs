/*!
    \file    main.c
    \brief   PWM output by using comparator output

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
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

#include "gd32f3x0.h"
#include <stdio.h>
#include "systick.h"

uint16_t period = 30000;

void rcu_config(void);
void gpio_config(void);
void timer_config(void);
void cmp_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure RCU */
    rcu_config();
    
    /* configure GPIO */
    gpio_config();
    
    /* configure TIMER */
    timer_config();
    
    /* configure comparator */
    cmp_config();
    
    while(1);
}

/*!
    \brief      configure RCU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_CFGCMP);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PB3 as PWM output */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_3);
    
    /* configure PA1 as comparator input */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    
}

/*!
    \brief      configure TIMER
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* initialize TIMER1 */
    timer_parameter_struct timer_init_parameter;
    timer_oc_parameter_struct timer_ocintpara;
    
    timer_deinit(TIMER1);
    
    /* TIMER1 configuration */
    timer_init_parameter.prescaler = 71;
    timer_init_parameter.alignedmode = TIMER_COUNTER_EDGE;
    timer_init_parameter.counterdirection = TIMER_COUNTER_UP;
    timer_init_parameter.period = period;
    timer_init_parameter.clockdivision = TIMER_CKDIV_DIV1;
    timer_init_parameter.repetitioncounter = 0;
    timer_init(TIMER1, &timer_init_parameter);
    
     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);

    /* PWM1 mode configure channel1 in PWM1 mode */
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, (period/2) + 1);

    /* select OCREFCLR as source for clearing OC2REF */
    timer_channel_output_clear_config(TIMER1, TIMER_CH_1, TIMER_OC_CLEAR_ENABLE);
    timer_ocpre_clear_source_config(TIMER1, TIMER_OCPRE_CLEAR_SOURCE_CLR);

    /* enable TIMER1 counter */
    timer_enable(TIMER1);
}

/*!
    \brief      configure comparator
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_HIGHSPEED, CMP_VREFINT, CMP_HYSTERESIS_NO);
    
    /* enable comparator channel0 */
    cmp_enable(CMP0);
    delay_1ms(1);

    /* configure comparator channel0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_TIMER1OCPRECLR, CMP_OUTPUT_POLARITY_NOINVERTED);
}
