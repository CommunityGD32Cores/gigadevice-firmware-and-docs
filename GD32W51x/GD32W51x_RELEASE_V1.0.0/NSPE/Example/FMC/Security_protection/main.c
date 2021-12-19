/*!
    \file     main.c
    \brief    main flash read protection

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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


#include "gd32w51x.h"
#include "gd32w515p_eval.h"

volatile uint16_t read_protection_flag = 0;
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
    /* initialize KEYs and LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_EXTI);
    /* turn on LED3 */
    gd_eval_led_on(LED3);

    /* check the write protection result */
    if(RESET == ob_security_protection_flag_get(OB_FLAG_SPC1)){
        /* indicates the page is not in read protection */
        read_protection_flag = 0;
    }else{
        /* indicates the page is in read protection */
        read_protection_flag = 1;
    }

    while(1){
        /* check current SPC state */
        if(SET == ob_security_protection_flag_get(OB_FLAG_SPC1)){
            /* turn on LED1 */
            gd_eval_led_on(LED1);
            if(1U == spc_disable_flag){
                /* disable security protection */
                fmc_unlock();
                ob_unlock();
                ob_security_protection_config(FMC_NSPC);
                ob_start();
                ob_reload();
            }
        }else if(RESET == ob_security_protection_flag_get(OB_FLAG_SPC1)){
            /* turn on LED2 */
            gd_eval_led_on(LED2);
            if(1U == spc_enable_flag){
                /* enable security protection */
                fmc_unlock();
                ob_unlock();
                ob_security_protection_config(FMC_SPC_P1);
                ob_start();
                ob_reload();
            }
        }
    }
}
