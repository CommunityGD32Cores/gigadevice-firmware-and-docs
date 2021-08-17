/*!
    \file    main.c
    \brief   running LED
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
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

#include "gd32e10x.h"
#include "gd32e10x_eval.h"
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
    systick_config();
    
    /* enable the LED2 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED2 GPIO port */ 
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOC,GPIO_PIN_0);

    /* enable the LED3 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED3 GPIO port */ 
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2);
    /* reset LED3 GPIO pin */
    gpio_bit_reset(GPIOC,GPIO_PIN_2);
    
    /* enable the LED4 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* configure LED4 GPIO port */ 
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0);
    /* reset LED4 GPIO pin */
    gpio_bit_reset(GPIOE,GPIO_PIN_0);
    
    /* enable the LED5 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* configure LED5 GPIO port */ 
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    /* reset LED5 GPIO pin */
    gpio_bit_reset(GPIOE,GPIO_PIN_1);

    while(1){
        /* turn on LED2, turn off LED5 */
        gpio_bit_set(GPIOC,GPIO_PIN_0);
        gpio_bit_reset(GPIOE,GPIO_PIN_1);
        delay_1ms(1000);
        /* turn on LED3, turn off LED2 */
        gpio_bit_set(GPIOC,GPIO_PIN_2);
        gpio_bit_reset(GPIOC,GPIO_PIN_0);
        delay_1ms(1000);
        /* turn on LED4, turn off LED3 */
        gpio_bit_set(GPIOE,GPIO_PIN_0);
        gpio_bit_reset(GPIOC,GPIO_PIN_2);
        delay_1ms(1000);
        /* turn on LED5, turn off LED4 */
        gpio_bit_set(GPIOE,GPIO_PIN_1);
        gpio_bit_reset(GPIOE,GPIO_PIN_0);
        delay_1ms(1000);
    }
}
