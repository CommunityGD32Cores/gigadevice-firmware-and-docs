/*!
    \file  main.c
    \brief GPIO running led
    
    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-30, V2.0.0, demo for GD32F10x
    \version 2018-07-31, V2.1.0, demo for GD32F10x
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

#include "gd32f10x.h"
#include "gd32f107c_eval.h"
#include "systick.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
    
    systick_config();
    
    while(1){
        /* turn on LED2 */
        gd_eval_led_on(LED2);
        /* insert 200 ms delay */
        delay_1ms(200);
        
        /* turn on LED3 */
        gd_eval_led_on(LED3);
        /* insert 200 ms delay */
        delay_1ms(200);
        
        /* turn on LED4 */
        gd_eval_led_on(LED4);
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED5 */
        gd_eval_led_on(LED5);
        /* insert 200 ms delay */
        delay_1ms(200);         

        /* turn off LEDs */
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
        gd_eval_led_off(LED5);
        
        /* insert 200 ms delay */
        delay_1ms(200);
    }
}
