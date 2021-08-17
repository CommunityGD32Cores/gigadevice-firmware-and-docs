/*!
    \file    gd32vf103_it.c
    \brief   interrupt service routines

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

#include "gd32vf103_it.h"
#include "main.h" 

/*!
    \brief      this function handles RTC alarm interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Alarm_IRQHandler(void)
{
    if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_ALARM)){
        /* clear the RTC alarm and EXTI_17 interrupt flags */
        rtc_interrupt_flag_clear(RTC_INT_FLAG_ALARM);
        exti_interrupt_flag_clear(EXTI_17);
        /* update RTC alarm time */
        rtc_register_sync_wait();
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        rtc_counter_set(0U);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        rtc_alarm_config(ALARM_TIME_INTERVAL);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        /* feed dog */
        fwdgt_counter_reload();
    }
}
