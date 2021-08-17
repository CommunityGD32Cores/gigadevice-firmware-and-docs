/*!
    \file  main.c
    \brief GPIO keyboard polling demo
    
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
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_GPIO);
    systick_config();
    gd_eval_led_init(LED3);

    while(1){
        if(RESET == gd_eval_key_state_get(KEY_TAMPER)){
            /* delay 50ms for software removing jitter */
            delay_1ms(50);
            if(RESET == gd_eval_key_state_get(KEY_TAMPER)){
                gpio_bit_write(LED3_GPIO_PORT, LED3_PIN, (bit_status)((1 - gpio_output_bit_get(LED3_GPIO_PORT, LED3_PIN))));
                while(RESET == gd_eval_key_state_get(KEY_TAMPER));
            }
        }
    }
}
