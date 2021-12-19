/*!
    \file    main.c
    \brief   EXTI key interrupt demo

    \version 2021-08-15, V1.0.0, demo for GD32L23x
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
#include "gd32l23x.h"
#include "gd32l233r_eval.h"
#include "systick.h"

static void led_flash(uint8_t times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration*/
    systick_config();

    /* enable the LED2 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED2 GPIO port */
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOC, GPIO_PIN_8);

    /* flash the LED for test */
    led_flash(1);

    /* enable the Tamper key GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* configure button pin as input */
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_13);

    /* enable and set key EXTI interrupt priority */
    nvic_irq_enable(EXTI10_15_IRQn, 2U);
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(TAMPER_KEY_EXTI_PORT_SOURCE, TAMPER_KEY_EXTI_PIN_SOURCE);
    /* configure key EXTI line */
    exti_init(TAMPER_KEY_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(TAMPER_KEY_EXTI_LINE);

    while(1) {
    }
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LED
    \param[out] none
    \retval     none
*/
static void led_flash(uint8_t times)
{
    uint8_t i;
    for(i = 0; i < times; i++) {
        /* delay 500 ms */
        delay_1ms(500);
        /* turn on the LED2 */
        gpio_bit_set(GPIOC, GPIO_PIN_8);

        /* delay 500 ms */
        delay_1ms(500);
        /* turn off the LED2 */
        gpio_bit_reset(GPIOC, GPIO_PIN_8);
    }
}
