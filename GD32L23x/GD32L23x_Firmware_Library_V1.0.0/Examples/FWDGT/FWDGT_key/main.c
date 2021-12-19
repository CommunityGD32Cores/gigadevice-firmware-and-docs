/*!
    \file    main.c
    \brief   FWDGT key demo

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l23x.h"
#include "systick.h"
#include "gd32l233r_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable IRC32K */
    rcu_osci_on(RCU_IRC32K);

    /* wait till IRC32K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K)) {
    }
    /* config systick */
    systick_config();

    /* configure LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* configure the Wakeup key which is used to reload FWDGT */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    delay_1ms(500);
    /* confiure FWDGT counter clock: 32KHz(IRC32K) / 64 = 0.5 KHz */
    fwdgt_config(3 * 500, FWDGT_PSC_DIV64);
    /* after 3 seconds to generate a reset */
    fwdgt_enable();

    /* check if the system has resumed from FWDGT reset */
    if(RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)) {
        /* turn on LED2 */
        gd_eval_led_on(LED2);
        /* clear the FWDGT reset flag */
        rcu_all_reset_flag_clear();

        while(1) {
        }
    } else {
        /* turn on LED1 */
        gd_eval_led_on(LED1);
    }
    while(1) {
    }
}
