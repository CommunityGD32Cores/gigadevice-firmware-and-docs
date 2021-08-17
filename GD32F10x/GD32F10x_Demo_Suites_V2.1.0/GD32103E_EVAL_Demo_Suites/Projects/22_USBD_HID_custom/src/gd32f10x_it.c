/*!
    \file  gd32f10x_it.c
    \brief main interrupt service routines

    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-20, V2.0.0, demo for GD32F10x
    \version 2018-07-31, V2.1.0, demo for GD32F10x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd32f10x_it.h"
#include "usbd_int.h"
#include "custom_hid_core.h"
uint8_t Send_Buffer[4] = {0x00, 0x01, 0x00, 0x01};

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
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_CAN0_RX0_IRQHandler (void)
{
    usbd_isr();
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
        if (USBD_CONFIGURED == usb_device_dev.status) {
            Send_Buffer[0] = 0x15; 

            if (RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
                if(Send_Buffer[1]) {
                    Send_Buffer[1] = 0x00;
                } else {
                    Send_Buffer[1] = 0x01;
                }
            }

            custom_hid_report_send (&usb_device_dev, Send_Buffer, 2);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI10_15_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(TAMPER_KEY_EXTI_LINE)) {
        if(USBD_CONFIGURED == usb_device_dev.status) {
            Send_Buffer[2] = 0x16; 

            if (RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                if(Send_Buffer[3]) {
                    Send_Buffer[3] = 0x00;
                } else {
                    Send_Buffer[3] = 0x01;
                }
            }

            custom_hid_report_send (&usb_device_dev, &Send_Buffer[2], 2);
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
void USBD_WKUP_IRQHandler (void)
{
    exti_interrupt_flag_clear(EXTI_18);
}

#endif /* USBD_LOWPWR_MODE_ENABLE */
