/*!
    \file    main.c
    \brief   SHRTIMER0 timebase config

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include <stdio.h>
#include "main.h"
#include "gd32e507z_eval.h"

void shrtimer_config(void);

/*!
    \brief      configure the SHRTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void shrtimer_config(void)
{
/* -----------------------------------------------------------------------
    fSHRTIMER_CK:180MHz, from CK_SYS
    period: 384/(180M*64)=33.3ns
  ----------------------------------------------------------------------- */
    shrtimer_baseinit_parameter_struct baseinit_para;

    /* clock config */
    rcu_periph_clock_enable(RCU_SHRTIMER);
    rcu_shrtimer_clock_config(RCU_SHRTIMERSRC_CKSYS);

    /* periodic DLL calibration */
    shrtimer_dll_calibration_start(SHRTIMER0, SHRTIMER_CALIBRATION_16384_PERIOD);
    while(RESET == shrtimer_common_flag_get(SHRTIMER0, SHRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    shrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period = 384;
    baseinit_para.prescaler = SHRTIMER_PRESCALER_MUL64;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode = SHRTIMER_COUNTER_MODE_CONTINOUS;
    shrtimer_timers_base_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &baseinit_para);
    /* enable a counter */
    shrtimer_timers_counter_enable(SHRTIMER0, SHRTIMER_ST0_COUNTER);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    shrtimer_config();

    while(1);
}
