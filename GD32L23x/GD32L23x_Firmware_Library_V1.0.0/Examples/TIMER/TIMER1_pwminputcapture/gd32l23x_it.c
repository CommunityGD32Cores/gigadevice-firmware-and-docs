/*!
    \file    gd32l23x_it.c
    \brief   interrupt service routines

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l23x_it.h"
#include "systick.h"
uint32_t ic1value = 0, ic2value = 0;
__IO uint16_t dutycycle = 0;
__IO uint16_t frequency = 0;
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
    while(1) {
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
    \brief      this function handles TIMER1 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_CH0)) {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH0);
        /* read channel 0 capture value */
        ic1value = timer_channel_capture_value_register_read(TIMER1, TIMER_CH_0) + 1;

        if(0 != ic1value) {
            /* read channel 1 capture value */
            ic2value = timer_channel_capture_value_register_read(TIMER1, TIMER_CH_1) + 1;

            /* calculate the duty cycle value */
            dutycycle = (ic2value * 100) / ic1value;
            /* calculate the frequency value */
            frequency = 1000000 / ic1value;

        } else {
            dutycycle = 0;
            frequency = 0;
        }
    }
}
