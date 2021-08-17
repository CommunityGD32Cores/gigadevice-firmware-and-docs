/*!
    \file    main.c
    \brief   use Key to control LED in interrupt mode
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32e23x.h"
#include "gd32e231c_start.h"
#include "systick.h"
#include <stdio.h>

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
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    systick_config();
    gd_eval_led_init(LED1);
    
    /* flash the LED for test */
    led_flash(1);
    delay_1ms(500);
    gd_eval_led_on(LED1);

    while(1){
    }
}

/*!
    \brief      flash the LED for test
    \param[in]  times: times to flash the LED
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for (i = 0;i < times;i++){
        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LEDs */
        gd_eval_led_toggle(LED1);

        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LEDs */
        gd_eval_led_toggle(LED1);
    }
}
