/*!
    \file    main.c
    \brief   deepsleep wakeup through RTC alarm interrupt

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

#include "gd32vf103.h"
#include "gd32vf103v_eval.h"
#include "systick.h"
#include "main.h"

void led_configuration(void);
void clic_irqs_configuration(void);
void rtc_configuration(void);
void fwdgt_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure eclic */
    clic_irqs_configuration();
    /* configure LED */
    led_configuration();
    gd_eval_led_on(LED2);
    delay_1ms(2000U);
    gd_eval_led_off(LED2);
    /* configure RTC */
    rtc_configuration();
    /* configure FWDGT */
    fwdgt_configuration();

    while(1){
        delay_1ms(50U);
        /* PMU enters deepsleep mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
        gd_eval_led_toggle(LED1);
    }
}

/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_configuration(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
}

/*!
    \brief      configure the ECLIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clic_irqs_configuration(void)
{
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(RTC_ALARM_IRQn, 1, 1);
}

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();
    /* reset backup domain */
    bkp_deinit();

    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* select RCU_IRC40K as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* enable the RTC alarm interrupt*/
    rtc_interrupt_enable(RTC_INT_ALARM);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* set RTC prescaler: set RTC period to 1s */
    rtc_prescaler_set(40000);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    rtc_counter_set(0U);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    rtc_alarm_config(ALARM_TIME_INTERVAL);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* configure EXTI */
    exti_deinit();
    exti_init(EXTI_17,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    rtc_flag_clear(RTC_FLAG_ALARM);
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
    /* configure FWDGT counter clock: 40KHz(IRC40K) / 256 = 0.15625 KHz */
    fwdgt_config(1250, FWDGT_PSC_DIV256);  
    while(FWDGT_STAT);
    /* after 8 seconds to generate a reset */
    fwdgt_enable();
}
