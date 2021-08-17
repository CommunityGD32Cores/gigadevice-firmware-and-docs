/*!
    \file    gd32e10x_it.c
    \brief   interrupt service routines
    
    \version 2018-03-26, V1.0.0, demo for GD32E103
    \version 2020-09-30, V1.1.0, demo for GD32E103
    \version 2020-12-31, V1.2.0, demo for GD32E103
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

#include "gd32e10x_it.h"
#include "systick.h"
#include "gd32e103v_eval.h"
#include "rtc.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles RTC global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_IRQHandler(void)
{
    uint32_t temp = 0, max_counter = 0, reset_value = 0, year = 0;
    
    if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_SECOND)){
        /* clear the RTC second interrupt flag*/
        rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);    

        temp = rtc_counter_get();
        print_calendar_on_lcd(temp);

        year = (temp >> 25) + MIN_YEAR;
        max_counter = IS_LEAP_YEAR(year) ? (366 * 24 * 3600) : (365 * 24 * 3600);

        /* reset RTC counter when time is 31st Dec. 23:59:59 */
        if((temp & 0x01FFFFFF) == (max_counter - 1)){
            year++;
            if(year > MAX_YEAR){
                year = MIN_YEAR;
            }
            reset_value = ((year - MIN_YEAR) << 25);

            /* set new year's value to RTC counter register */
            rtc_lwoff_wait();
            rtc_counter_set(reset_value);
            rtc_lwoff_wait();
        }
    }

    if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_ALARM)){
        /* clear the RTC alarm interrupt flag*/
        rtc_interrupt_flag_clear(RTC_INT_FLAG_ALARM);

        /* toggle LEDs when the alarm occurs */
        gd_eval_led_toggle(LED2);
        gd_eval_led_toggle(LED3);
        gd_eval_led_toggle(LED4);
        gd_eval_led_toggle(LED5);
    }
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    uint32_t current_counter = 0, max_counter = 0, alarm_counter = 0, year = 0;;

    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)){
        exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);

        current_counter = rtc_counter_get();
        year = (current_counter >> 25) + MIN_YEAR;
        max_counter = IS_LEAP_YEAR(year) ? (366 * 24 * 3600) : (365 * 24 * 3600);

        /* calculate the alarm_counter = current_counter + 10 second */
        if(((current_counter & 0x01FFFFFF) + 10) < max_counter){
            alarm_counter = current_counter + 10;
        }else{
            year++;
            if(year > MAX_YEAR){
                year = MIN_YEAR;
            }
            
            alarm_counter = ((year - MIN_YEAR) << 25) + (((current_counter & 0x01FFFFFF) + 10) - max_counter);
        }
        
        /* set the alarm_counter */
        rtc_lwoff_wait();
        rtc_alarm_config(alarm_counter);
        rtc_lwoff_wait();
    }
}
