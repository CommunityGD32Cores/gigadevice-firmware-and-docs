/*!
    \file    main.c
    \brief   CTC is used to trim internal 48MHz RC oscillator with LXTAL clock

    \version 2021-08-15, V1.0.0, demo for GD32L23x
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
#include "gd32l233r_eval.h"
#include "stdio.h"

/* configure the CTC peripheral */
void ctc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LED2*/
    gd_eval_led_init(LED2);

    /* enable IRC48M clock */
    RCU_CTL |= RCU_CTL_IRC48MEN;
    /* wait till IRC48M is ready */
    while(0U == (RCU_CTL & RCU_CTL_IRC48MSTB)) {
    }

    /* configure LXTAL clock */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    /* enable LXTAL clock */
    RCU_BDCTL |= RCU_BDCTL_LXTALEN;
    /* wait till LXTAL is ready */
    while(0U == (RCU_BDCTL & RCU_BDCTL_LXTALSTB)) {
    }

    /* enable CTC peripheral clock */
    rcu_periph_clock_enable(RCU_CTC);
    /* configure CTC */
    ctc_config();

    while(1) {
        /* if the clock trim is OK */
        if(RESET != ctc_flag_get(CTC_FLAG_CKOK)) {
            gd_eval_led_on(LED2);
        } else {
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
    /* configure CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_LXTAL);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* configure hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);

    /* configure CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0x05B8U);
    /* configure clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x0001U);

    /* enable CTC counter */
    ctc_counter_enable();
}
