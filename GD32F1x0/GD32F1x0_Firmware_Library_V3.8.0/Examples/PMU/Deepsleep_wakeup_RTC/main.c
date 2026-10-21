/*!
    \file    main.c
    \brief   deepsleep wakeup through RTC alarm interrupt

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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
void nvic_configuration(void);
void rtc_configuration(void);
void fwdgt_configuration(void);
void rtc_alarm_time_configuration(void);
static void _soft_delay_(uint32_t time);

/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0U; i<time*10U; i++){
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* nvic configuration */
    nvic_configuration();
    /* LED configuration */
    led_config();
    /* systick configuration */
    systick_config();
    gd_eval_led_on(LED4);
    delay_1ms(2000U);
    gd_eval_led_off(LED4);
    /* RTC configuration */
    rtc_configuration();
    /* FWDGT configuration */
    fwdgt_configuration();

    while(1){
        delay_1ms(50U);
        /* The following is to prevent Vcore fluctuations caused by frequency switching. 
           It is strongly recommended to include it to avoid issues caused by self-removal. */
        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
        _soft_delay_(100U);
        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV4);
        _soft_delay_(100U);
        /* PMU enters deepsleep mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
        gd_eval_led_toggle(LED2);
    }
}

/*!
    \brief      LED configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED4);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(RTC_IRQn, 2U, 1U);
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
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to backup domain */
    pmu_backup_write_enable();
    /* reset backup domain */
    rcu_bkp_reset_enable();
    rcu_bkp_reset_disable();

    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* select RCU_IRC40K as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    rtc_initpara.rtc_factor_asyn = 0x63U;
    rtc_initpara.rtc_factor_syn = 0x18FU;
    rtc_initpara.rtc_year = 0x16U;
    rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
    rtc_initpara.rtc_month = RTC_APR;
    rtc_initpara.rtc_date = 0x30U;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;

    /* current time configuration */
    rtc_initpara.rtc_hour = 0x00U;
    rtc_initpara.rtc_minute = 0x00U;
    rtc_initpara.rtc_second = 0x00U;
    rtc_init(&rtc_initpara);
    rtc_alarm_disable();
    rtc_alarm.rtc_alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
    rtc_alarm.rtc_weekday_or_date = RTC_ALARM_DATE_SELECTED;
    rtc_alarm.rtc_alarm_day = 0x31U;
    rtc_alarm.rtc_am_pm = RTC_AM;

    /* RTC alarm value */
    rtc_alarm.rtc_alarm_hour = 0x00U;
    rtc_alarm.rtc_alarm_minute = 0x00U;
    rtc_alarm.rtc_alarm_second = 0x02U;

    /* RTC alarm configuration */
    rtc_alarm_config(&rtc_alarm);
    
    rtc_interrupt_enable(RTC_INT_ALARM);
    rtc_alarm_enable();
    
    /* EXTI configuration */
    exti_deinit();
    exti_init(EXTI_17,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    rtc_flag_clear(RTC_FLAG_ALARM0);
    exti_interrupt_flag_clear(EXTI_17);
    exti_interrupt_enable(EXTI_17);
}

/*!
    \brief      configure the FWDGT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fwdgt_configuration(void)
{
    /* confiure FWDGT counter clock: 40KHz(IRC40K) / 256 = 0.15625 KHz */
    fwdgt_config(1250U, FWDGT_PSC_DIV256);  
    while(FWDGT_STAT) {
    }
    /* after 8 seconds to generate a reset */
    fwdgt_enable();
}

/*!
    \brief      configure RTC alarm time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_alarm_time_configuration(void)
{
    rtc_alarm_struct  rtc_alarm;
    rtc_parameter_struct   rtc_initpara;
    /* current time configuration */    
    rtc_initpara.rtc_factor_asyn = 0x63U;
    rtc_initpara.rtc_factor_syn = 0x18FU;
    rtc_initpara.rtc_year = 0x16U;
    rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
    rtc_initpara.rtc_month = RTC_APR;
    rtc_initpara.rtc_date = 0x30U;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;

    rtc_initpara.rtc_hour = 0x00U;
    rtc_initpara.rtc_minute = 0x00U;
    rtc_initpara.rtc_second = 0x00U;
    rtc_init(&rtc_initpara);
    rtc_alarm_disable();
    rtc_alarm.rtc_alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
    rtc_alarm.rtc_weekday_or_date = RTC_ALARM_DATE_SELECTED;
    rtc_alarm.rtc_alarm_day = 0x31U;
    rtc_alarm.rtc_am_pm = RTC_AM;

    /* RTC alarm value */
    rtc_alarm.rtc_alarm_hour = 0x00U;
    rtc_alarm.rtc_alarm_minute = 0x00U;
    rtc_alarm.rtc_alarm_second = 0x02U;

    /* RTC alarm configuration */
    rtc_alarm_config(&rtc_alarm);
    
    rtc_interrupt_enable(RTC_INT_ALARM);
    rtc_alarm_enable();
}
