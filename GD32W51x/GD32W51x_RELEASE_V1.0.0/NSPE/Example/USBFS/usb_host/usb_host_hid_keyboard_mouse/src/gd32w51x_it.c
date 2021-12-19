/*!
    \file    gd32w51x_it.c
    \brief   main interrupt service routines

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#include "drv_usbh_int.h"
#include "usbh_core.h"
#include "gd32w51x_it.h"

extern usb_core_driver usbh_core;

void usb_timer_irq (void);

/*!
    \brief      this function handles NMI exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles external line 1 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI1_IRQHandler(void)
{

}

/*!
    \brief      this function handles hard fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles memory manage exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles bus fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles usage fault exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
    \brief      this function handles SVCall exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles Debug Monitor exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSVC exception.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles Timer2 Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

/*!
    \brief      this function handles USBHS global interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}

/*!
    \brief      this function handles SysTick Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}
