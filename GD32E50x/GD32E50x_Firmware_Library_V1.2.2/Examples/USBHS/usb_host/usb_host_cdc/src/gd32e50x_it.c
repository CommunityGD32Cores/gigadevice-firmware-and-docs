/*!
    \file    gd32e50x_it.c
    \brief   main interrupt service routines

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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
#include "usbh_usr.h"
#include "gd32e50x_it.h"

extern usb_core_driver usbh_core;
extern cdc_demo_state_machine cdc_demo;

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
    \brief      this function handles SysTick Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

/*!
    \brief      this function handles EXTI0 Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    uint32_t i = 0U;

    if (exti_interrupt_flag_get (KEY_A_EXTI_LINE) != RESET)
    {
        for (i = 0; i < 5000000; i++);

        if (((cdc_demo.state == CDC_DEMO_WAIT) && (cdc_demo.select < 2))|| 
              ((cdc_demo.state == CDC_DEMO_SEND) && (cdc_demo.select < 4))||
                 ((cdc_demo.state == CDC_DEMO_CONFIGURATION) && (cdc_demo.select < 1)))
        {
            cdc_demo.select ++;
        }

        /* Clear the EXTI line pending bit */
        exti_interrupt_flag_clear(KEY_A_EXTI_LINE);
    }
}

/*!
    \brief      this function handles EXTI10_15 Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    uint32_t i = 0U;

    if(exti_interrupt_flag_get(KEY_C_EXTI_LINE) != RESET){
        for(i = 0; i < 5000000; i++);

        if(cdc_demo.select > 0){
            cdc_demo.select --;
        }

        /* Clear the EXTI line pending bit */
        exti_interrupt_flag_clear(KEY_C_EXTI_LINE);
    }

    if(exti_interrupt_flag_get(KEY_CET_EXTI_LINE) != RESET){
        for (i = 0; i < 10000000; i++);

        cdc_demo.select |= 0x80;

        /* Clear the EXTI line pending bit */
        exti_interrupt_flag_clear(KEY_CET_EXTI_LINE);
    }
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
void USBHS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}
