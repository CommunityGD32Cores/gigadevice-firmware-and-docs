/*!
    \file    gd32f1x0_it.c
    \brief   main interrupt service routines

    \version 2020-07-23, V3.0.0, firmware for GD32F1x0
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

#include "gd32f1x0_it.h"
#include "usbd_lld_int.h"
#include "ccid_cmd.h"

extern usb_dev usbd_ccid;

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
    \brief      this function handles handles SC_USART interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SC_USART_IRQHandler(void)
{
    /* if a frame error is signaled by the card */
    if(RESET != usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_FERR)){
        usart_data_receive(SC_USART);

        /* clear the frame error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_FERR);

        /* resend the byte that failed to be received */
        sc_parity_error_handler();
    }

    /* if the SC_USART detects a parity error */
    if(RESET != usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_PERR)){
        /* enable SC_USART RBNE interrupt (until receiving the corrupted byte) */
        usart_interrupt_enable(SC_USART, USART_INT_RBNE);

        /* clear the parity error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_PERR);

        /* flush the SC_USART RDATA register */
        usart_data_receive(SC_USART);
    }

    if(RESET != usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_RBNE)){
        /* Disable SC_USART RXNE Interrupt */
        usart_interrupt_disable(SC_USART, USART_INT_RBNE);

        usart_data_receive(SC_USART);
    }

    /* if a overrun error is signaled by the card */
    if(RESET != usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_ORERR)){
        usart_data_receive(SC_USART);

        /* clear the overrun error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_ORERR);
    }

    /* if a noise error is signaled by the card */
    if(RESET != usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_NERR)){
        usart_data_receive(SC_USART);

        /* clear the noise error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_NERR);
    }
}

/*!
    \brief      this function handles handles the smartcard detection interrupt 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SC_OFF_EXTI_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(SC_DETECT_EXTI_LINE)){
        /* clear EXTI Line pending bit */
        exti_interrupt_flag_clear(SC_DETECT_EXTI_LINE);

        /* smartcard detected */
        CCID_update_slot_change(&usbd_ccid, 1);

        /* enable CMDVCC */
        sc_power_cmd(ENABLE);
    }
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBD_LP_IRQHandler (void)
{
    usbd_isr();
}

/*!
    \brief      this function handles USB WakeUp interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBDWakeUp_IRQHandler (void)
{
    exti_interrupt_flag_clear(EXTI_18);
}
