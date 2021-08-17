/*!
    \file    main.c
    \brief   PWM output by using comparator output

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"
#include <stdio.h>
#include "systick.h"

uint16_t period = 65535;

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
    /* systick configuration */
    systick_config();

    /* configure RCU */
    rcu_config();
    
    /* configure GPIO */
    gpio_config();
    
    /* configure TIMER */
    timer_config();
    
    /* configure comparator */
    cmp_config();
    
    while(1){
    }
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
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
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
    timer_init_parameter.period = 65535;
    timer_init_parameter.clockdivision = TIMER_CKDIV_DIV1;
    
    timer_init(TIMER1, &timer_init_parameter);
    
    /* CH1 configuration in PWM mode */
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
    /* configure CMP0 */
    cmp_mode_init(CMP0, CMP_HIGHSPEED, CMP_VREFINT, CMP_HYSTERESIS_NO);
    
    /* enable CMP0 */
    cmp_enable(CMP0);
    delay_1ms(1U);

    /* configure CMP0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_TIMER1OCPRECLR, CMP_OUTPUT_POLARITY_NOINVERTED);
}
