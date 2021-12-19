/*!
    \file    main.c
    \brief   CMP output timer input capture

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

#include <stdio.h>
#include "systick.h"
#include "gd32l23x.h"
#include "gd32l233r_eval.h"

void rcu_config(void);
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
    /* configure systick */
    systick_config();

    /* configure RCU */
    rcu_config();

    /* configure GPIO */
    gpio_config();

    /* configure leds */
    gd_eval_led_init(LED2);

    /* configure comparator channel0 */
    cmp_mode_init(CMP0, CMP_LOWSPEED, CMP_VREFINT, CMP_HYSTERESIS_NO);
    cmp_voltage_scaler_enable(CMP0);
    cmp_scaler_bridge_enable(CMP0);

    /* enable comparator channel0 */
    cmp_enable(CMP0);
    delay_1ms(1);

    /* configure comparator channel0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_TIMER1IC3, CMP_OUTPUT_POLARITY_NOINVERTED);

    /* configure TIMER */
    timer_config();

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
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_CMP);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* CMP0_IP:PA1 */
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1) ;
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
    timer_ic_parameter_struct timer_icinitpara;

    timer_deinit(TIMER1);

    /* TIMER configuration */
    timer_init_parameter.prescaler         = 71;
    timer_init_parameter.alignedmode       = TIMER_COUNTER_EDGE;
    timer_init_parameter.counterdirection  = TIMER_COUNTER_UP;
    timer_init_parameter.period            = 65535;
    timer_init_parameter.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_init_parameter);

    /* TIMER CH3 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER1, TIMER_CH_3, &timer_icinitpara);

    /* clear channel interrupt bit */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH3);
    /* channel interrupt enable */
    timer_interrupt_enable(TIMER1, TIMER_INT_CH3);

    nvic_irq_enable(TIMER1_IRQn, 0);

    /* enable TIMER1 counter */
    timer_enable(TIMER1);
}
