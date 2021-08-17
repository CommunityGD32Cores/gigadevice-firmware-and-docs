/*!
    \file  main.c
    \brief GPIO running led

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "gd32f450z_eval.h"
#include "systick.h"

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

    /* enable the LEDs GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure LED1 and LED2 GPIO port */ 
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
    /* reset LED1 and LED2 GPIO pin */
    gpio_bit_reset(GPIOE, GPIO_PIN_4 | GPIO_PIN_5);

    /* configure LED3 GPIO port */ 
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    /* reset LED3 GPIO pin */
    gpio_bit_reset(GPIOG, GPIO_PIN_3);

    while(1){
        /* turn on LED1, turn off LED2 and LED3 */
        gpio_bit_set(GPIOD, GPIO_PIN_4);
        gpio_bit_reset(GPIOD, GPIO_PIN_5);
        gpio_bit_reset(GPIOG, GPIO_PIN_3);
        delay_1ms(400);

        /* turn on LED2, turn off LED1 and LED3 */
        gpio_bit_set(GPIOD, GPIO_PIN_5);
        gpio_bit_reset(GPIOD, GPIO_PIN_4);
        gpio_bit_reset(GPIOG, GPIO_PIN_3);
        delay_1ms(400);

        /* turn on LED3, turn off LED1 and LED2 */
        gpio_bit_set(GPIOG, GPIO_PIN_3);
        gpio_bit_reset(GPIOD, GPIO_PIN_4);
        gpio_bit_reset(GPIOD, GPIO_PIN_5);
        delay_1ms(400);
    }
}
