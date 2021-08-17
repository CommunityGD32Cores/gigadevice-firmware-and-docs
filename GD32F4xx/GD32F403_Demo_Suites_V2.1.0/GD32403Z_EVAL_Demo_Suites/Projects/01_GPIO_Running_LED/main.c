/*!
    \file  main.c
    \brief GPIO running led demo
    
    \version 2017-02-10, V1.0.0, demo for GD32F403
    \version 2018-10-10, V1.1.0, demo for GD32F403
    \version 2018-12-25, V2.0.0, demo for GD32F403
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd32f403.h"
#include "gd32f403z_eval.h"
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
    /* configure systick */
    systick_config();
    
    /* enable the LEDs GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure LED2 GPIO port */ 
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOF, GPIO_PIN_0);

    /* configure LED3 GPIO port */ 
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    /* reset LED3 GPIO pin */
    gpio_bit_reset(GPIOF, GPIO_PIN_1);
    
    /* configure LED4 GPIO port */ 
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* reset LED4 GPIO pin */
    gpio_bit_reset(GPIOF, GPIO_PIN_2);
    
    /* configure LED5 GPIO port */ 
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    /* reset LED5 GPIO pin */
    gpio_bit_reset(GPIOF, GPIO_PIN_3);

    while(1){
        /* turn on LED2, turn off LED5 */
        gpio_bit_set(GPIOF, GPIO_PIN_0);
        gpio_bit_reset(GPIOF, GPIO_PIN_3);
        delay_1ms(1000);

        /* turn on LED3, turn off LED2 */
        gpio_bit_set(GPIOF, GPIO_PIN_1);
        gpio_bit_reset(GPIOF, GPIO_PIN_0);
        delay_1ms(1000);

        /* turn on LED4, turn off LED3 */
        gpio_bit_set(GPIOF, GPIO_PIN_2);
        gpio_bit_reset(GPIOF, GPIO_PIN_1);
        delay_1ms(1000);
        
        /* turn on LED5, turn off LED4 */
        gpio_bit_set(GPIOF, GPIO_PIN_3);
        gpio_bit_reset(GPIOF, GPIO_PIN_2);
        delay_1ms(1000);
    }
}
