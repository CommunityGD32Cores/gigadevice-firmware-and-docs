/*!
    \file    gd32f30x_it.c
    \brief   main interrupt service routines

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
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

#include "gd32f30x_it.h"
#include "usbd_lld_int.h"
#include "hid_printer_wrapper.h"

uint8_t Send_Buffer[4] = {0x00, 0x01, 0x00, 0x01};

extern usb_dev usbd_hid_printer;

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
    \brief      this function handles EXTI0_1_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)) {
        if (USBD_CONFIGURED == usbd_hid_printer.cur_status) {
            Send_Buffer[0] = 0x15U; 

            if (RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
                if(Send_Buffer[1]) {
                    Send_Buffer[1] = 0x00U;
                } else {
                    Send_Buffer[1] = 0x01U;
                }
            }

            custom_hid_report_send (&usbd_hid_printer, Send_Buffer, 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI14_15_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(USER_KEY1_EXTI_LINE)) {
        if(USBD_CONFIGURED == usbd_hid_printer.cur_status) {
            Send_Buffer[2] = 0x16U;

            if (RESET == gd_eval_key_state_get(KEY_USER1)) {
                if(Send_Buffer[3]) {
                    Send_Buffer[3] = 0x00U;
                } else {
                    Send_Buffer[3] = 0x01U;
                }
            }

            custom_hid_report_send (&usbd_hid_printer, &Send_Buffer[2], 2U);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(USER_KEY1_EXTI_LINE);
    }
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_CAN0_RX0_IRQHandler (void)
{
    usbd_isr();
}

#ifdef USBD_LOWPWR_MODE_ENABLE

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_WKUP_IRQHandler (void)
{
    exti_interrupt_flag_clear(EXTI_18);
}

#endif /* USBD_LOWPWR_MODE_ENABLE */
