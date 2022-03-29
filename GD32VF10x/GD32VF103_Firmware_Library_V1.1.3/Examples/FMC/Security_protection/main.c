/*!
    \file     main.c
    \brief    main flash read protection

    \version 2020-08-04, V1.0.0, firmware for GD32VF103
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

#include "gd32vf103.h"
#include "gd32vf103v_eval.h"

uint8_t spc_enable_flag = 0;
uint8_t spc_disable_flag = 0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize KEYs and LEDs */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_key_init(KEY_A, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_B, KEY_MODE_EXTI);
    gd_eval_led_on(LED4);

    while(1){
        /* check current SPC state */
        if(SET == ob_spc_get()){
            gd_eval_led_on(LED2);
            if(1U == spc_disable_flag){
                /* disable security protection */
                fmc_unlock();
                ob_unlock();
                ob_security_protection_config(FMC_NSPC);
                ob_lock();
                fmc_lock();
                /* reload option bytes and generate a system reset */
                eclic_system_reset();
            }
        }else{
            gd_eval_led_on(LED3);
            if(1U == spc_enable_flag){
                /* enable security protection */
                fmc_unlock();
                ob_unlock();
                ob_security_protection_config(FMC_USPC);
                ob_lock();
                fmc_lock();
                /* reload option bytes and generate a system reset */
                eclic_system_reset();
            }
        }
    }
}
