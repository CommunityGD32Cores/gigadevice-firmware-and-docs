/*!
    \file  main.c
    \brief SLCD digit display

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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
#include <stdio.h>
#include "systick.h"
#include "slcd_seg.h"

void rcu_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t num = 0;
    /* system clocks configuration */
    rcu_configuration();
    /* systick config: 1ms delay */
    systick_config();
    /* slcd interface configuration */
    slcd_seg_configuration();

    while(1){
        slcd_seg_number_display(num++);
        delay_1ms(1000);
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_configuration(void)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* PMU backup domain write enable */
    pmu_backup_write_enable();
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait for IRC40K stabilization flags */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* configure the RTC clock source selection */
    rcu_slcd_clock_config(RCU_SLCDSRC_IRC40K);
}
