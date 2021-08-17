/*!
    \file    gd32f4xx_it.c
    \brief   main interrupt service routines

    \version 2020-09-04, V3.0.0, demo for GD32F4xx
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

#include "drv_usbh_int.h"
#include "usbh_core.h"
#include "gd32f4xx_it.h"

extern usbh_host usb_host;
extern usb_core_driver usbh_core;

void usb_timer_irq (void);

#ifdef USB_LOW_PWR_ENABLE
/* local function prototypes ('static') */
static void resume_mcu_clk(void);
#endif /* USB_LOW_PWR_ENABLE */

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
    /* go to infinite loop when hard fault exception occurs */
    while (1) {
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
    /* go to infinite loop when memory manage exception occurs */
    while (1) {
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
    /* go to infinite loop when bus fault exception occurs */
    while (1) {
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
    /* go to infinite loop when usage fault exception occurs */
    while (1) {
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
    \brief      this function handles SysTick Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

#ifdef USB_LOW_PWR_ENABLE

/*!
    \brief      this function handles external line 10_15 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    if (exti_interrupt_flag_get(USER_KEY_EXTI_LINE) != RESET) {
        if (usb_host.suspend_flag == 1) {
            usb_host.suspend_flag = 0;

            /* configure system clock */
            resume_mcu_clk();

            /* resume from suspend mode */
            usb_hwp_resume(&usbh_core);
        }

        /* clear the EXTI line pending bit */
        exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
    }
}

#endif

#ifdef USB_LOW_PWR_ENABLE

#ifdef USE_USB_FS

/*!
    \brief      this function handles USBFS wakeup interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (usb_host.suspend_flag == 1) {
        usb_host.suspend_flag = 0;

        /* configure system clock */
        resume_mcu_clk();

        /* resume from suspend mode */
        usb_hwp_resume(&usbh_core);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

#elif defined(USE_USB_HS)

/*!
    \brief      this function handles USBHS wakeup interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_WKUP_IRQHandler(void)
{
    if (usb_host.suspend_flag == 1) {
        usb_host.suspend_flag = 0;

        /* configure system clock */
        resume_mcu_clk();

        /* resume from suspend mode */
        usb_hwp_resume(&usbh_core);
    }

    exti_interrupt_flag_clear(EXTI_20);
}

#endif /* USE_USBFS */

#endif /* USB_LOW_PWR_ENABLE */

#ifdef USE_USB_FS

/*!
    \brief      this function handles USBFS IRQ Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}

#elif defined(USE_USB_HS)

/*!
    \brief      this function handles USBHS IRQ Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_IRQHandler(void)
{
    usbh_isr(&usbh_core);
}

#endif /* USE_USBFS */

#ifdef USB_LOW_PWR_ENABLE

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

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLLP);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLLP != rcu_system_clock_source_get()){
    }
}

#endif /* USB_LOW_PWR_ENABLE */
