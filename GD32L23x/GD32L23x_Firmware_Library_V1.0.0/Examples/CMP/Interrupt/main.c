/*!
    \file    main.c
    \brief   compare0 interrupt example

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
#include "gd32l233r_eval.h"


void GPIO_Configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initilize LED */
    gd_eval_led_init(LED2);

    /* turn off LED */
    gd_eval_led_off(LED2);

    /* configure compare0 GPIO */
    GPIO_Configuration();

    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CMP);

    /* enable CMP clock */
    nvic_irq_enable(CMP0_IRQn, 0U);

    /* init compare0, since using 1/2VREFINT as inverting input, the voltage brige and voltage scaler must be enable */
    cmp_disable(CMP0);
    cmp_voltage_scaler_enable(CMP0);
    cmp_scaler_bridge_enable(CMP0);
    cmp_mode_init(CMP0, CMP_HIGHSPEED, CMP_VREFINT, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP0, CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);

    /* init EXTI21 interrupt for compare0 */
    exti_flag_clear(EXTI_21);
    exti_init(EXTI_21, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_enable(EXTI_21);

    /* compare0 enable */
    cmp_enable(CMP0);

    while(1) {
    }
}

/*!
    \brief      configure GPIO for Compare0 input and output
    \param[in]  none
    \param[out] none
    \retval     none
*/
void GPIO_Configuration(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /* config Compare0 plus input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
}
