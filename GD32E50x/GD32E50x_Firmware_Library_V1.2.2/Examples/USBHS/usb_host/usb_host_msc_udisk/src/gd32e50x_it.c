/*!
    \file    gd32e50x_it.c
    \brief   main interrupt service routines

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
    \version 2021-09-15, V1.2.1, firmware for GD32E50x
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
#include "drv_usb_hw.h"
#include "usbh_core.h"
#include "gd32e50x_it.h"

extern usbh_host usb_host;
extern usb_core_driver usbh_core;

void usb_timer_irq (void);

#if USB_LOW_POWER
/* local function prototypes ('static') */
static void resume_mcu_clk(void);
#endif /* USB_LOW_POWER */

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
    \brief      this function handles Timer2 Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

#if USB_LOW_POWER

/*!
    \brief      this function handles external line 10_15 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    if (exti_interrupt_flag_get(KEY_CET_EXTI_LINE) != RESET) {
        if (usb_host.suspend_flag == 1) {
            usb_host.suspend_flag = 0;
            usb_host.wakeup_mode = 1; /* general wakeup mode */

            /* configure system clock */
            resume_mcu_clk();

#ifdef USE_USB_HS
            /* configure usb clock */
            pllusb_rcu_config();
#endif /* USE_USB_HS */
        }

        /* clear the EXTI line pending bit */
        exti_interrupt_flag_clear(KEY_CET_EXTI_LINE);
    }
}

#endif /* USB_LOW_POWER */

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

#if USB_LOW_POWER

/*!
    \brief      this function handles USBHS wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_WKUP_IRQHandler(void)
{
    if (usb_host.suspend_flag == 1) {
        usb_host.suspend_flag = 0;
        usb_host.wakeup_mode = 2; /* remote wakeup mode */

        /* configure system clock */
        resume_mcu_clk();

#ifdef USE_USB_HS
        /* configure usb clock */
        pllusb_rcu_config();
#endif /* USE_USB_HS */
    }

    exti_interrupt_flag_clear(EXTI_18);
}

/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HSE */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HSE is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    /* enable PLL1 */
    rcu_osci_on(RCU_PLL1_CK);

    /* wait till PLL1 is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLL1STB)){
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLL != rcu_system_clock_source_get()){
    }
}

#endif /* USB_LOW_POWER */
