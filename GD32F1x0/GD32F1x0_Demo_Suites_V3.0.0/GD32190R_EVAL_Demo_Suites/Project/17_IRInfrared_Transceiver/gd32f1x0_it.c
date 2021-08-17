/*!
    \file  gd32f1x0_it.c
    \brief interrupt service routines

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

#include "gd32f1x0_it.h"
#include "rc5_encode.h"
#include "rc5_decode.h"
#include "ir_decode.h"
#include "systick.h"

uint32_t icvalue1 = 0;
uint32_t icvalue2 = 0;

extern uint32_t rc5_frame_manchester_format;
extern __IO trc5_packet_struct rc5_tmp_packet;

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
    while (1);
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
    while (1);
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
    while (1);
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
    while (1);
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
    delay_decrement( );
}

/*!
    \brief      this function handles TIMER15 update interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER15_IRQHandler(void)
{
    /* clear TIMER15 update interrupt */
    timer_interrupt_flag_clear(TIMER15, TIMER_INT_FLAG_UP);
    rc5_encode_signal_generate(rc5_frame_manchester_format);
}

/*!
    \brief      this function handles TIMER2 overflow and update interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
#if defined(GD32F130_150)
    /* ic2 interrupt*/
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_FLAG_CH1))){
        /* clear IR_TIMER CH1 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_FLAG_CH1);
        /* get the input capture value */
        icvalue2 = TIMER_CH1CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue2, 1);
    }  

    /* ic1 interrupt */
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_FLAG_CH0))){
        /* clear IR_TIMER CH0 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_FLAG_CH0);
        /* get the input capture value */
        icvalue1 = TIMER_CH0CV(IR_TIMER) - TIMER_CH1CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue1, 0);
    }

#elif defined(GD32F170_190)
    /* ic1 interrupt*/
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_FLAG_CH0 ))){
        /* clear IR_TIMER CH0 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_FLAG_CH0);
        /* get the input capture value */
        icvalue1 = TIMER_CH0CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue1, 1);
    }

    /* ic2 interrupt */   
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_FLAG_CH1))){
        /* clear IR_TIMER CH1 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_FLAG_CH1);
        /* get the input capture value */
        icvalue2 = TIMER_CH1CV(IR_TIMER); 
        icvalue2 -= TIMER_CH0CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue2, 0);
    }

#endif
}
