/*!
    \file    main.c
    \brief   TIMER1 external trigger demo

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l23x.h"
#include <stdio.h>

void gpio_config(void);
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gpio_config();
    timer_config();
    while(1);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /*configure PA0(TIMER1 CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_0);

    /*configure PA1(TIMER1 CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_1);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* Timer with an external trigger -------------------------------------
    TIMER1 is configured as slave timer for an external trigger connected
    to TIMER1 CI0 pin :
    - The TIMER1 CI0FE0 is used as trigger input
    - rising edge is used to start the TIMER1: trigger mode.
    - TIMER1 CH1 is used PWM0 Mode
    the starts of the TIMER1 counter are controlled by the
    external trigger.
    -------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler        = 63;
    timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period           = 7999;
    timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate    = TIMER_CCX_ENABLE;
    timer_ocinitpara.ocpolarity     = TIMER_OC_POLARITY_HIGH;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocinitpara);

    /* CH1 configuration in PWM0 mode */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 3999);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* TIMER1 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x02;
    timer_input_capture_config(TIMER1, TIMER_CH_0, &timer_icinitpara);

    /* slave mode selection : TIMER1 */
    /* TIMER1 input trigger : external trigger connected to CI0 */
    timer_input_trigger_source_select(TIMER1, TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(TIMER1, TIMER_SLAVE_MODE_EVENT);

}
