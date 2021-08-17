/*!
    \file  main.c
    \brief GPIO key interrupt mode demo

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

void led_flash(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,GPIO_PIN_0);
    rcu_periph_clock_enable(RCU_CFGCMP);

    nvic_irq_enable(EXTI0_1_IRQn,2u,0u);
    
    /* connect EXTI_0 Line to PA0 pin */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);

    /* configure key EXTI line */
    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_0);

    systick_config();

    /* configure led GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_1 | GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1 | GPIO_PIN_2);
    gpio_bit_reset(GPIOA, GPIO_PIN_1 | GPIO_PIN_2);

    led_flash(1);

    while(1){
    }
}

/*!
    \brief      flash the LEDs for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    uint32_t i;
    for (i = 0;i < times;i++){
        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LED1 and LED2 */
        gpio_bit_set(GPIOA, GPIO_PIN_1 | GPIO_PIN_2);

        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LED2 and turn off LED1,LED3,LED4 */
        gpio_bit_reset(GPIOA, GPIO_PIN_1 | GPIO_PIN_2);
    }
}
