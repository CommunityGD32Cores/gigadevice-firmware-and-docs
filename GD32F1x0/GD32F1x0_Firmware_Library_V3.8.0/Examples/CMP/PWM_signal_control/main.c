/*!
    \file    main.c
    \brief   CMP PWM_signal_control demo

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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
#include "systick.h"

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* configure TIMER peripheral */
void timer_config(void);

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

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure CMP peripheral */
    cmp_config();
    /* configure TIMER peripheral */
    timer_config();

    while(1) {
    }
}

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CFGCMP);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PA1 as CMP0 input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);

    /* configure PA8 as PWM output */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);
}

/*!
    \brief      configure CMP peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* initialize CMP0 */
    cmp_deinit(CMP0);
    /* configure CMP0 mode */
    cmp_mode_init(CMP0, CMP_MODE_HIGHSPEED, CMP_INVERTING_INPUT_VREFINT, CMP_HYSTERESIS_NO);
    /* configure CMP0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_TIMER0_OCPRECLR, CMP_OUTPUT_POLARITY_NONINVERTED);

    /* enable CMP0 */
    cmp_enable(CMP0);
    /* delay 1ms */
    delay_1ms(1);
}

/*!
    \brief      configure TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* initialize TIMER0 */
    timer_parameter_struct timer_init_parameter;
    timer_oc_parameter_struct timer_ocintpara;

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_init_parameter.prescaler = 71;
    timer_init_parameter.alignedmode = TIMER_COUNTER_EDGE;
    timer_init_parameter.counterdirection = TIMER_COUNTER_UP;
    timer_init_parameter.period = 9999;
    timer_init_parameter.clockdivision = TIMER_CKDIV_DIV1;
    timer_init_parameter.repetitioncounter = 0;
    timer_init(TIMER0, &timer_init_parameter);

    /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    /* PWM1 mode configure channel0 in PWM1 mode */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 4999);

    /* select OCPRE_CLR_INT as source for clearing OC0PRE */
    timer_channel_output_clear_config(TIMER0, TIMER_CH_0, TIMER_OC_CLEAR_ENABLE);
    timer_ocpre_clear_source_config(TIMER0, TIMER_OCPRE_CLEAR_SOURCE_CLR);

    /* enable TIMER primary output */
    timer_primary_output_config(TIMER0, ENABLE);
    /* enable TIMER0 counter */
    timer_enable(TIMER0);
}
