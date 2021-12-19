/*!
    \file    gd32l23x_it.c
    \brief   main interrupt service routines

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
#include "usbd_lld_int.h"
#include "usbd_lld_core.h"

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
    while(1);
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
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_IRQHandler(void)
{
    usbd_isr();
}

/*!
    \brief      this function handles EXTI0_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)) {
        /* check if the remote wakeup feature is enabled */
        if(usbd_core.dev->pm.remote_wakeup) {
            /* exit low power mode and re-configure clocks */
            usbd_remote_wakeup_active(usbd_core.dev);

            usbd_core.dev->cur_status = usbd_core.dev->backup_status;

            /* disable Remote wakeup feature*/
            usbd_core.dev->pm.remote_wakeup = 0U;
        }
#ifdef LPM_ENABLED
        else if(1U == usbd_core.dev->lpm.L1_remote_wakeup) {
            /* exit low power mode and re-configure clocks */
            usbd_remote_wakeup_active(usbd_core.dev);

            usbd_core.dev->cur_status = usbd_core.dev->backup_status;

            /* disable Remote wakeup feature*/
            usbd_core.dev->lpm.L1_remote_wakeup = 0U;
        }
#endif /* LPM_ENABLED */

        /* clear the EXTI line pending bit */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

#ifdef USBD_LOWPWR_MODE_ENABLE

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_WKUP_IRQHandler(void)
{
    exti_interrupt_flag_clear(EXTI_18);
}

#endif /* USBD_LOWPWR_MODE_ENABLE */
