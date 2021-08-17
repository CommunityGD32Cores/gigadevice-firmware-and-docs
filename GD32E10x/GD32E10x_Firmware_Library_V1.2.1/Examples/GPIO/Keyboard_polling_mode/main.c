/*!
    \file    main.c
    \brief   keyboard polling mode
    
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
#include "systick.h"
#include "gd32e10x_eval.h"
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
    
    /* enable the key GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure button pin as input */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_13);

    while(1){
        /* check whether the button is pressed */
        if(RESET == gpio_input_bit_get(GPIOC,GPIO_PIN_13)){
            delay_1ms(100);

            /* check whether the button is pressed */
            if(RESET == gpio_input_bit_get(GPIOC,GPIO_PIN_13)){
                gpio_bit_write(GPIOC, GPIO_PIN_0, 
                (bit_status)(1-gpio_input_bit_get(GPIOC, GPIO_PIN_0)));
            }
        }
    }
}
