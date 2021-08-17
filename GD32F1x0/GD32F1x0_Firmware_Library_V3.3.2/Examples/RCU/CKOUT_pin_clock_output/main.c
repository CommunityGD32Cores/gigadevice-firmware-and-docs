/*!
    \file    main.c
    \brief   clock output via CKOUT pin example

    \version 2020-07-16, V1.0.0, firmware for GD32F1x0
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f1x0r_eval.h"

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
    
#ifdef GD32F130_150
    /* configure CKOUT to output system clock */
    rcu_ckout_config(RCU_CKOUTSRC_CKSYS, RCU_CKOUT_DIV4);
#elif defined (GD32F170_190)
    /* configure CKOUT0 to output system clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS, RCU_CKOUT0_DIV4);
#endif /* GD32F130_150 */
    delay_1ms(1000);
    
    /* enable IRC8M clock */
    rcu_osci_on(RCU_IRC8M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC8M)){
        while(1){
        }
    }
#ifdef GD32F130_150
    /* configure CKOUT to output IRC8M clock */
    rcu_ckout_config(RCU_CKOUTSRC_IRC8M, RCU_CKOUT_DIV1);
#elif defined (GD32F170_190)
    /* configure CKOUT0 to output IRC8M clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_IRC8M, RCU_CKOUT0_DIV1);
#endif /* GD32F130_150 */
    delay_1ms(1000);
    
    /* enable HXTAL clock */
    rcu_osci_on(RCU_HXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)){
        while(1){
        }
    }
#ifdef GD32F130_150
    /* configure CKOUT to output HXTAL clock */
    rcu_ckout_config(RCU_CKOUTSRC_HXTAL, RCU_CKOUT_DIV1);
#elif defined (GD32F170_190)
    /* configure CKOUT0 to output HXTAL clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
#endif /* GD32F130_150 */
    
    while(1){
    }
}
