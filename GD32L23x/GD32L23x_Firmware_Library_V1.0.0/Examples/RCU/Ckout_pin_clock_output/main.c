/*!
    \file    main.c
    \brief   clock output via CKOUT pin example

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
#include <stdio.h>
#include "gd32l233r_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the Systick */
    systick_config();
    /* enable peripheral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure clock output pin */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_8);

    /* configure CKOUT0 to output system clock */
    rcu_ckout_config(RCU_CKOUTSRC_CKSYS, RCU_CKOUT_DIV4);
    delay_1ms(1000);

    /* enable IRC16M clock */
    rcu_osci_on(RCU_IRC16M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC16M)) {
        while(1) {
        }
    }
    /* configure CKOUT0 to output IRC16M clock */
    rcu_ckout_config(RCU_CKOUTSRC_IRC16M, RCU_CKOUT_DIV1);
    delay_1ms(1000);

    /* enable HXTAL clock */
    rcu_osci_on(RCU_HXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)) {
        while(1) {
        }
    }
    /* configure CKOUT0 to output HXTAL clock */
    rcu_ckout_config(RCU_CKOUTSRC_HXTAL, RCU_CKOUT_DIV1);

    while(1) {
    }
}
