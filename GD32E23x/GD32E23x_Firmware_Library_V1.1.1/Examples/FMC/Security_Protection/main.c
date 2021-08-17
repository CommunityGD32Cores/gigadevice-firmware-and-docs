/*!
    \file    main.c
    \brief   main flash read protection

    \version 2020-12-12, V1.0.0, firmware for GD32E23x
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

#include "gd32e230c_eval.h"
#include <stdio.h>

volatile uint16_t spc_enable_flag = 0;
volatile uint16_t spc_disable_flag = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LED1-LED3 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    /* configure the Wakeup key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    /* configure the Tamper key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    /* unlock flash */
    fmc_unlock();
    ob_unlock();
    
    /* check the write protection result */
    if(OB_OBSTAT_PLEVEL_NO == ob_obstat_plevel_get()){
        /* indicates the page is not in read protection */
        gd_eval_led_on(LED1);
        gd_eval_led_off(LED2);
    }else{
        /* indicates the page is in read protection */
        gd_eval_led_on(LED2);
    }
    
    while(1){
        if(OB_OBSTAT_PLEVEL_LOW == ob_obstat_plevel_get()){
            if(1U == spc_disable_flag){
                /* disable SPC */
                ob_security_protection_config(FMC_NSPC);
                ob_lock();
                fmc_lock();
                ob_reset();
            }
        }else if(OB_OBSTAT_PLEVEL_NO == ob_obstat_plevel_get()){
            if(1U == spc_enable_flag){
                /* enable SPC */
                ob_security_protection_config(FMC_LSPC);
                ob_lock();
                fmc_lock();
                ob_reset();
            }
        }
        gd_eval_led_on(LED3);
    }
}
