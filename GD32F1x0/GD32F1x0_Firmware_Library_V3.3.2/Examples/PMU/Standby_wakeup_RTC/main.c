/*!
    \file    main.c
    \brief   wakeup from standby by RTC alarm

    \version 2020-07-16, V1.0.0, firmware for GD32F10x
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
#include "gd32f1x0r_eval.h"
#include "systick.h"
#include "main.h"

void led_config(void);
void rtc_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    /* led configuration and turn on all LEDs */
    led_config();
    gd_eval_led_on(LED1);
    gd_eval_led_on(LED2);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED4);

    /* delay 2s */
    delay_1ms(2000);
    /* PMU clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* configure RTC */
    rtc_configuration();
    /* PMU enters standby mode */
    pmu_to_standbymode(WFI_CMD);
    
    while(1){
    }
}

/*!
    \brief      LED config
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

}

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    rtc_parameter_struct   rtc_initpara;
    rtc_alarm_struct  rtc_alarm;
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* select RCU_IRC40K as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    rtc_initpara.rtc_factor_asyn = 0x63;
    rtc_initpara.rtc_factor_syn = 0x18F;
    rtc_initpara.rtc_year = 0x16;
    rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
    rtc_initpara.rtc_month = RTC_APR;
    rtc_initpara.rtc_date = 0x30;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;

    /* current time configure */
    rtc_initpara.rtc_hour = 0x00;
    rtc_initpara.rtc_minute = 0x00;
    rtc_initpara.rtc_second = 0x00;
    rtc_init(&rtc_initpara);
    rtc_alarm_disable();
    rtc_alarm.rtc_alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
    rtc_alarm.rtc_weekday_or_date = RTC_ALARM_DATE_SELECTED;
    rtc_alarm.rtc_alarm_day = 0x31;
    rtc_alarm.rtc_am_pm = RTC_AM;

    /* RTC alarm value */
    rtc_alarm.rtc_alarm_hour = 0x00;
    rtc_alarm.rtc_alarm_minute = 0x00;
    rtc_alarm.rtc_alarm_second = ALARM_TIME_INTERVAL;

    /* RTC alarm configuration */
    rtc_alarm_config(&rtc_alarm);
    
    rtc_interrupt_enable(RTC_INT_ALARM);
    rtc_alarm_enable();

    rtc_flag_clear(RTC_FLAG_ALARM0);
}
