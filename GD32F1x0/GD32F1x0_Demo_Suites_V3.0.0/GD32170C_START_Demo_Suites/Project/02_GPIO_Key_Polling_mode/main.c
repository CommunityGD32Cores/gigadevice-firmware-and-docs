/*!
    \file  main.c
    \brief GPIO KEY polling mode demo

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
#include "gd32f1x0_libopt.h"
#include "systick.h"

void rcu_config(void);
void led_key_config(void);
void led_flash(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_config();
    led_key_config();
    systick_config();

    /* flash all the LEDs for test */
    led_flash(1);

    while(1){ 
        /* check if key is pressed */
        if(RESET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)){
            /* delay 50ms for software removing jitter */
            delay_1ms(50);
            if(RESET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)){
                /* toggle LEDs */
                gpio_bit_toggle(GPIOF, GPIO_PIN_6);
                gpio_bit_toggle(GPIOF, GPIO_PIN_7);
                while(RESET == gpio_input_bit_get(GPIOA, GPIO_PIN_0));
            }
        }
    }
}

/*!
    \brief      configure LED and Key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_key_config(void)
{
    /* configure LED GPIO port */ 
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_bit_reset(GPIOF, GPIO_PIN_6 | GPIO_PIN_7);

    /* configure the PA0 key */
    gpio_mode_set(RCU_GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}

/*!
    \brief      clock configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOF);
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
    for (i = 0;i < times;i++){
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
