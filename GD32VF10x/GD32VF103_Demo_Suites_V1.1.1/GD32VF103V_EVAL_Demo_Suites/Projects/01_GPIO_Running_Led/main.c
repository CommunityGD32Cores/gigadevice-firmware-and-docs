/*!
    \file    main.c
    \brief   running led

    \version 2019-06-05, V1.0.0, demo for GD32VF103
    \version 2020-12-18, V1.1.0, demo for GD32VF103
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

#include "gd32vf103.h"
#include "gd32vf103v_eval.h"
#include "systick.h"
#include <stdio.h>

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the LED clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure LED GPIO port */
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
    gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);

    while(1){
        /* turn on LED1 */
        gpio_bit_set(GPIOC, GPIO_PIN_0);
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED2 */
        gpio_bit_set(GPIOC, GPIO_PIN_2);
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED3 */
        gpio_bit_set(GPIOE, GPIO_PIN_0);
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED4 */
        gpio_bit_set(GPIOE, GPIO_PIN_1);
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off four LEDs */
        gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
        gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
        /* insert 200 ms delay */
        delay_1ms(200);
    }
}
