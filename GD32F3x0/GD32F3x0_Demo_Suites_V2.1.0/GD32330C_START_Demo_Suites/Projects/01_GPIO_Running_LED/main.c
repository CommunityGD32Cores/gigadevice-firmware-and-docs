/*!
    \file    main.c
    \brief   GPIO running led
    
    \version 2017-06-28, V1.0.0, demo for GD32F3x0
    \version 2019-06-01, V2.0.0, demo for GD32F3x0
    \version 2021-05-31, V2.1.0, demo for GD32F3x0
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

#include "gd32f3x0.h"
#include "gd32f330c_start.h"
#include "systick.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    systick_config();
    /* enable the LED GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7);
    
    gpio_bit_reset(GPIOF, GPIO_PIN_6|GPIO_PIN_7);
    
    while(1){
        /* turn on led1, turn off led4 */
        gpio_bit_write(GPIOF, GPIO_PIN_6, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_7, RESET);
        delay_1ms(1000);
        
        /* turn on led2, turn off led1 */
        gpio_bit_write(GPIOF, GPIO_PIN_7, SET);
        gpio_bit_write(GPIOF, GPIO_PIN_6, RESET);
        delay_1ms(1000);
    }
}

