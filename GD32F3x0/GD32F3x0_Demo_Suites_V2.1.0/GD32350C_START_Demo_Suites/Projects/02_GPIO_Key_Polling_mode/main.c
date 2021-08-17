/*!
    \file    main.c
    \brief   GPIO key polling demo

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
#include "gd32f350c_start.h"
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
    /* initialize KEY and LED, configure SysTick */
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);

    systick_config();

    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7);

    gpio_bit_reset(GPIOF, GPIO_PIN_6 | GPIO_PIN_7);

    /* flash the LED for test */
    led_flash(1);

    while(1){
        if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)){
            /* delay 50ms for software removing jitter */
            delay_1ms(50);
            if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)){
                gpio_bit_toggle(GPIOF, GPIO_PIN_6);
                while(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0));
            }
        }
    }
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for(i = 0;i < times;i++){
        /* delay 250 ms */
        delay_1ms(250);
        /* toggle the LED */ 
        gpio_bit_toggle(GPIOF, GPIO_PIN_6 | GPIO_PIN_7);

        /* delay 250 ms */
        delay_1ms(250);
        /* toggle the LED */ 
        gpio_bit_toggle(GPIOF, GPIO_PIN_6 | GPIO_PIN_7);
    }
}

