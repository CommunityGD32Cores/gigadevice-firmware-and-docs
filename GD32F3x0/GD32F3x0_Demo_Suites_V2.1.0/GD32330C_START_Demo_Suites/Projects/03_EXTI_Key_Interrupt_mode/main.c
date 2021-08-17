/*!
    \file    main.c
    \brief   GPIO KeyBoard Interrupt mode demo
    
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
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);
    systick_config();
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    /* flash the LED for test */
    led_flash(1);

    while(1){
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
    for (i = 0;i < times;i++){
        /* delay 250 ms */
        delay_1ms(250);
        /* toggle the LED */
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);

        /* delay 250 ms */
        delay_1ms(250);
        /* toggle the LED */
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
    }
}
