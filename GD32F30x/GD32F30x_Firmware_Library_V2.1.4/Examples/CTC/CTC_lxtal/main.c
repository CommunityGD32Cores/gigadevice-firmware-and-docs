/*!
    \file    main.c
    \brief   CTC is used to trim internal 48MHz RC oscillator with LXTAL clock

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
    \version 2020-09-30, V2.1.0, firmware for GD32F30x
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

#include "gd32f30x.h"
#include "gd32f307c_eval.h"

void ctc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init LED2*/
    gd_eval_led_init(LED2);

    /* enable IRC48M clock */
    RCU_ADDCTL |= RCU_ADDCTL_IRC48MEN; 
    /* wait till IRC48M is ready */
    while(0 == (RCU_ADDCTL & RCU_ADDCTL_IRC48MSTB)){
    }

    /* LXTAL clock config */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    /* enable LXTAL clock */
    RCU_BDCTL |= RCU_BDCTL_LXTALEN;
    /* wait till LXTAL is ready */
    while(0 == (RCU_BDCTL & RCU_BDCTL_LXTALSTB)){
    }

    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);
    /* CTC config */
    ctc_config();

    while(1){
        /* if the clock trim is OK */
        if(ctc_flag_get(CTC_FLAG_CKOK) != RESET){
            gd_eval_led_on(LED2);
        }else{
            gd_eval_led_off(LED2);
        }
    }
}

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_LXTAL);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);
    
    /* config CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0x05B8);
    /* config clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x0001);

    /* CTC counter enable */
    ctc_counter_enable();
}
