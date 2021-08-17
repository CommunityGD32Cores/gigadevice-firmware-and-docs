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
#include "drv_usbd_int.h"
#include "custom_hid_core.h"

uint8_t send_buffer1[4] = {0x00, 0x01, 0x00, 0x00};
uint8_t send_buffer2[4] = {0x00, 0x01, 0x00, 0x00};

extern usb_core_driver usbd_hid_printer;
extern uint32_t usbfs_prescaler;

void usb_timer_irq (void);

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
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

/*!
    \brief      this function handles EXTI0_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)) {
        if (USBD_CONFIGURED == usbd_hid_printer.dev.cur_status) {
            send_buffer1[0] = 0x15; 

            if (RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
                if(send_buffer1[1]) {
                    send_buffer1[1] = 0x00;
                } else {
                    send_buffer1[1] = 0x01;
                }
            }

            custom_hid_report_send (&usbd_hid_printer, send_buffer1, 2);
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
        if (USBD_CONFIGURED == usbd_hid_printer.dev.cur_status) {
            send_buffer2[0] = 0x16;

            if (RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                if(send_buffer2[1]) {
                    send_buffer2[1] = 0x00;
                } else {
                    send_buffer2[1] = 0x01;
                }
            }

            custom_hid_report_send (&usbd_hid_printer, send_buffer2, 2);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(TAMPER_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler (void)
{
    usbd_isr (&usbd_hid_printer);
}

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (usbd_hid_printer.bp.low_power) {
        resume_mcu_clk();

        rcu_usb_clock_config(usbfs_prescaler);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&usbd_hid_printer);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

/*!
    \brief      this function handles Timer2 interrupt request.
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
