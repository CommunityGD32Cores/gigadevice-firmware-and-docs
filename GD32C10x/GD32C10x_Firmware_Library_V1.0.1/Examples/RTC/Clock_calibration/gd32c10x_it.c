/*!
    \file    gd32c10x_it.c
    \brief   interrupt service routines
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "gd32c10x_it.h"
#include "gd32c10x_eval.h"
#include "main.h"

extern __IO uint32_t timer_capture_num;
extern __IO uint32_t timer_capture1;
extern __IO uint32_t timer_capture2;

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
}

/*!
    \brief      this function handles RTC global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_IRQHandler(void)
{
    if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_SECOND)){
        /* clear the RTC second interrupt flag*/
        rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);

        gd_eval_led_toggle(LED2);
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
    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)){
        exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);

        /* configure the TIMER4 */
        timer_config();
    }
}

/*!
    \brief      this function handles TIMER4 global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER4_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER4, TIMER_INT_CH3)){
        /* clear channel 3 interrupt bit */
        timer_interrupt_flag_clear(TIMER4, TIMER_INT_CH3);

        timer_capture_num++;
        if(1 == timer_capture_num){
            /* get the input capture value */
            timer_capture1 = timer_channel_capture_value_register_read(TIMER4, TIMER_CH_3);
        }else if(2 == timer_capture_num){
            /* get the input capture value */
            timer_capture2 = timer_channel_capture_value_register_read(TIMER4, TIMER_CH_3);

            /* end capture, disable TIMER4 and CH3 interrupt */
            timer_interrupt_disable(TIMER4, TIMER_INT_CH3);
            timer_disable(TIMER4);
        }
    }
}
