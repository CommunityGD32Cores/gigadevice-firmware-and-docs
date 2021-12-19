/*!
    \file    main.c
    \brief   EXTI key interrupt demo

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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
#include <stdio.h>
#include "systick.h"

static void led_flash(uint8_t times);
void key_exti_init(void);

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
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure LED2 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_166MHZ, GPIO_PIN_15);
    
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOA, GPIO_PIN_15);

    /* flash the LED for test */
    led_flash(1);

    key_exti_init();

    while(1){
    }
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
static void led_flash(uint8_t times)
{
    uint8_t i;
    for (i = 0;i < times;i++){
        /* delay 500 ms */
        delay_1ms(500);
        /* turn on the LED */
        gpio_bit_set(GPIOA, GPIO_PIN_15);

        /* delay 500 ms */
        delay_1ms(500);
        /* turn off the LED */
        gpio_bit_reset(GPIOA, GPIO_PIN_15);
    }
}

    /*!
    \brief      initialize the EXTI configuration of the key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_exti_init(void)
{
    /* enable the CFGCMP clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* configure NVIC priority group */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    /* enable and set key EXTI interrupt to the specified priority */
    nvic_irq_enable(EXTI2_IRQn, 1U, 1U);

    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN2);

    /* configure key EXTI line */
    exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_2);
}
