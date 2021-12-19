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

#include "gd32w51x_it.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"

extern usb_core_driver hid_keyboard;
extern uint32_t usb_prescaler;
extern void usb_timer_irq (void);

/* local function prototypes ('static') */
static void resume_mcu_clk(void);

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
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

/*!
    \brief      this function handles USBFS interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler (void)
{
    usbd_isr (&hid_keyboard);
}

/*!
    \brief      this function handles EXTI0_IRQ Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI2_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(TAMPER_WAKEUP_KEY_EXTI_LINE)) {
        if (hid_keyboard.dev.pm.dev_remote_wakeup) {
            resume_mcu_clk();

            #ifdef USE_IRC48M
                /* enable IRC48M clock */
                rcu_osci_on(RCU_IRC48M);

                /* wait till IRC48M is ready */
                while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
                }

                rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
            #endif /* USE_IRC48M */

            rcu_periph_clock_enable(RCU_USBFS);

            usb_clock_active(&hid_keyboard);

            usb_rwkup_set(&hid_keyboard);

            /* add delay time */
            for(uint16_t i = 0; i < 1000; i++){
                for(uint16_t i = 0; i < 200; i++);
            }

            usb_rwkup_reset(&hid_keyboard);

            hid_keyboard.dev.cur_status = hid_keyboard.dev.backup_status;

            hid_keyboard.dev.pm.dev_remote_wakeup = 0U;
        }

        /* clear the exti line pending bit */
        exti_interrupt_flag_clear(TAMPER_WAKEUP_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles USBFS wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (hid_keyboard.bp.low_power) {
        resume_mcu_clk();

        #ifdef USE_IRC48M
            /* enable IRC48M clock */
            rcu_osci_on(RCU_IRC48M);

            /* wait till IRC48M is ready */
            while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
            }

            rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
        #endif /* USE_IRC48M */

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&hid_keyboard);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

/*!
    \brief      this function handles Timer0 update interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
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
