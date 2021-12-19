/*!
    \file    main.c
    \brief   Trustzone demo, non-secure code with TZEN = 1

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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

#include "gd32w51x.h"
#include "gd32w515p_eval.h"
#include "main.h"
#include "stdio.h"
#include "systick.h"

extern void entry_cb_func_register(void *callback);
extern void non_secure_print(const char * str);

void toggle_led1(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    entry_cb_func_register((void *)toggle_led1);
    /* configure systick */
    systick_config();
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    while(1){
        /* toggle LED2 */
        gd_eval_led_toggle(LED2);
        non_secure_print("non-secure code toggle LED2.\r\n");
        delay_1ms(1000);
    }
}

/*!
    \brief      toggle LED1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void toggle_led1(void)
{
    /* toggle LED1 */
    gd_eval_led_toggle(LED1);
}

