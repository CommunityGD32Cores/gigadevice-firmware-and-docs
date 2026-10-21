/*!
    \file    gd32f1x0_it.c
    \brief   main interrupt service routines

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)
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

#include "gd32f1x0_it.h"
#include "usbd_lld_int.h"
#include "custom_hid_core.h"

extern usb_dev usbd_custom_hid;

uint8_t send_buffer[4] = {0x00U, 0x01U, 0x00U, 0x01U};

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1) {
    }
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
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
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
    while(1) {
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
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles USBD low priority interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_IRQHandler(void)
{
    usbd_isr();
}

/*!
    \brief      this function handles EXTI0_1_IRQ handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_1_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)) {
        if(USBD_CONFIGURED == usbd_custom_hid.cur_status) {
            send_buffer[0] = 0x15U;

            if(RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
                if(send_buffer[1]) {
                    send_buffer[1] = 0x00U;
                } else {
                    send_buffer[1] = 0x01U;
                }
            }

            custom_hid_report_send(&usbd_custom_hid, send_buffer, 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI4_15_IRQ handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI4_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(TAMPER_KEY_EXTI_LINE)) {
        if(USBD_CONFIGURED == usbd_custom_hid.cur_status) {
            send_buffer[2] = 0x16U;

            if(RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                if(send_buffer[3]) {
                    send_buffer[3] = 0x00U;
                } else {
                    send_buffer[3] = 0x01U;
                }
            }
            custom_hid_report_send(&usbd_custom_hid, &send_buffer[2], 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(TAMPER_KEY_EXTI_LINE);
    }
}

#ifdef USBD_LOWPWR_MODE_ENABLE

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBDWakeUp_IRQHandler(void)
{
    exti_int_flag_clear(EXTI_18);
}

#endif /* USBD_LOWPWR_MODE_ENABLE */
